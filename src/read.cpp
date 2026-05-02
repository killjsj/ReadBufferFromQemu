#include "read.h"
#include "mapper.hpp"
#include "godot_cpp/classes/file_access.hpp"
#ifdef _WIN32
#include <windows.h>
#else
#include <semaphore.h>
#endif
#include <atomic>
#ifdef _WIN32
// 模拟 __atomic_load_n(p, __ATOMIC_ACQUIRE)
static inline uint32_t atomic_load_acquire(const uint32_t *ptr) {
    return *(const volatile uint32_t *)ptr;
}

// 原子写 + 释放语义（使用 InterlockedExchange 自带全屏障）
static inline void atomic_store_release(uint32_t *ptr, uint32_t val) {
    InterlockedExchange((volatile LONG *)ptr, (LONG)val);
}
#else
// Linux 下继续使用 GCC 内建函数（原实现即可）
static inline uint32_t atomic_load_acquire(const uint32_t *ptr) {
    return __atomic_load_n(ptr, __ATOMIC_ACQUIRE);
}
static inline void atomic_store_release(uint32_t *ptr, uint32_t val) {
    __atomic_store_n(ptr, val, __ATOMIC_RELEASE);
}
#endif
// #include "godot_cpp/"

ReaderClass::ReaderClass() {}
ReaderClass::~ReaderClass() { stopMachine(); }

void ReaderClass::_bind_methods() {
	ClassDB::bind_method(D_METHOD("startMachine", "args"), &ReaderClass::startMachine);
	ClassDB::bind_method(D_METHOD("stopMachine"), &ReaderClass::stopMachine);
	ClassDB::bind_method(D_METHOD("sync_connection"), &ReaderClass::sync_connection);

	ClassDB::bind_method(D_METHOD("get_usable", "id"), &ReaderClass::get_usable);

	// 绑定多屏幕相关方法
	ClassDB::bind_method(D_METHOD("get_screen_count"), &ReaderClass::get_screen_count);
	ClassDB::bind_method(D_METHOD("get_width", "index"), &ReaderClass::get_width);
	ClassDB::bind_method(D_METHOD("get_height", "index"), &ReaderClass::get_height);
	ClassDB::bind_method(D_METHOD("get_texture", "index"), &ReaderClass::get_texture);
	ClassDB::bind_method(D_METHOD("threaded_function"), &ReaderClass::updatemessgae);
	ClassDB::bind_method(D_METHOD("send_key_event", "godot_key", "pressed"), &ReaderClass::send_key_event);
	ClassDB::bind_method(D_METHOD("send_mouse_button_state", "button_state"), &ReaderClass::send_mouse_button_state);
	ClassDB::bind_method(D_METHOD("send_mouse_motion", "x", "y", "godot_w", "godot_h"), &ReaderClass::send_mouse_motion);
	ClassDB::bind_method(D_METHOD("send_mouse_wheel", "delta_y"), &ReaderClass::send_mouse_wheel);
}
void ReaderClass::send_key_event(int godot_key, bool pressed) {
    if (!qemu_conn.BS) return;
    int qcode = map_godot_key_to_qemu_qcode(godot_key);
    if (qcode == Q_KEY_CODE_UNMAPPED) return;

    BufferStruct* bs = qemu_conn.BS;
    uint32_t writer = atomic_load_acquire(&bs->input_write_idx);
    uint32_t reader = atomic_load_acquire(&bs->input_read_idx);
    GodotInputEvent& ev = bs->input_events[writer & (INPUT_RING_SIZE - 1)];
    ev.type = INPUT_EVENT_KEY;
    ev.console_index = 0;
    ev.keycode = qcode;
    ev.pressed = pressed;
    atomic_store_release(&bs->input_write_idx, writer + 1);
}
void ReaderClass::send_mouse_button_state(uint32_t button_state) {
    if (!qemu_conn.BS) return;

    BufferStruct* bs = qemu_conn.BS;
    uint32_t writer = atomic_load_acquire(&bs->input_write_idx);
    uint32_t reader = atomic_load_acquire(&bs->input_read_idx);
    if ((writer - reader) >= INPUT_RING_SIZE) return;

    GodotInputEvent& ev = bs->input_events[writer & (INPUT_RING_SIZE - 1)];
    ev.type = INPUT_EVENT_MOUSE_BUTTON_STATE;
    ev.console_index = 0;
    ev.button_state = button_state;

    atomic_store_release(&bs->input_write_idx, writer + 1);
}
void ReaderClass::send_mouse_motion(int x, int y, int godot_w, int godot_h) {
    if (!qemu_conn.BS) return;

    BufferStruct* bs = qemu_conn.BS;
    uint32_t writer = atomic_load_acquire(&bs->input_write_idx);
    uint32_t reader = atomic_load_acquire(&bs->input_read_idx);
    if ((writer - reader) >= INPUT_RING_SIZE) return;

    GodotInputEvent& ev = bs->input_events[writer & (INPUT_RING_SIZE - 1)];
    ev.type = INPUT_EVENT_MOUSE_MOVE;
    ev.console_index = 0;
    ev.mouse_x = x;
    ev.mouse_y = y;
    ev.godot_w = godot_w;
    ev.godot_h = godot_h;

    atomic_store_release(&bs->input_write_idx, writer + 1);
}
void ReaderClass::send_mouse_wheel(int delta_y) {
    if (!qemu_conn.BS) return;
    BufferStruct* bs = qemu_conn.BS;

    uint32_t writer = atomic_load_acquire(&bs->input_write_idx);
    uint32_t reader = atomic_load_acquire(&bs->input_read_idx);
    if ((writer - reader) >= INPUT_RING_SIZE) return;  // 满，丢弃

    GodotInputEvent& ev = bs->input_events[writer & (INPUT_RING_SIZE - 1)];
    ev.type = INPUT_EVENT_MOUSE_WHEEL;
    ev.console_index = 0;
    ev.wheel_delta = delta_y;   // 正负区分方向

    atomic_store_release(&bs->input_write_idx, writer + 1);
}
void ReaderClass::_notification(int p_what) {
	// Prevents this from running in the editor, only during game mode. In Godot 4.3+ use Runtime classes.
	if (godot::Engine::get_singleton()->is_editor_hint()) {
		return;
	}

	switch (p_what) {
		case NOTIFICATION_READY: {
			screens_mutex.instantiate();
		} break;
		case NOTIFICATION_EXIT_TREE: { // Thread must be disposed (or "joined"), for portability.
			// Wait until it exits.
			running = false;
			if (worker.is_valid()) {
				worker->wait_to_finish();
			}
			worker.unref();
			screens_mutex.unref();
		} break;
	}
}

void ReaderClass::updatemessgae() {
	while (running) {
		OS::get_singleton()->delay_msec(2);

		if (godot::Engine::get_singleton()->is_editor_hint()) {
			continue;
		}
		if (!qemu_conn.BS) {
			continue;
		}
		if (!qemu_conn.haveId || qemu_conn.BS == nullptr) {
			continue;
		}
		int screen_count = qemu_conn.BS->screen_count;
		screens_mutex->lock();

		if (screens_state.size() < screen_count) {
			screens_state.resize(screen_count);
			for (auto &it : screens_state) {
				if (it.swap_mutex.is_null()) {
					it.swap_mutex.instantiate();
				}
			}
		}
		screens_mutex->unlock();

		// 遍历所有屏幕
		for (int idx = 0; idx < screen_count; idx++) {
			SingleScreen *ss = &qemu_conn.BS->screens[idx];
			ScreenState &state = screens_state[idx];

			state.width = ss->w;
			state.height = ss->h;
			uint64_t new_total_size = ss->total_data_size;
			// 1. 处理内存重分配逻辑 (支持多窗口：注意把 idx 拼接到名字里)
			if (ss->ServerSwitched) {
				char data_name[128];
				HANDLE hNewMap = 0;
				strcpy(data_name, ss->shmName);
				void *pNewAddr;
				bool ret = false;
				for (int retry = 0; retry < 5; retry++) {
					ret = connect_to_data_shm(data_name, ss->total_data_size, hNewMap, (uint8_t **)&pNewAddr);
					godot::UtilityFunctions::print("connect_to_data_shm result,", ret);
					if (ret && pNewAddr)
						break;
					if (hNewMap) {
						#ifdef _WIN32
						CloseHandle(hNewMap);
						#else
						close(hNewMap);
						#endif
						hNewMap = 0;
					}
					std::this_thread::sleep_for(std::chrono::milliseconds(3));
				}
				if (ret && pNewAddr != nullptr) {
					if ((void *)state.data_ptr != nullptr) {
						disconnect_shm(state.data_ptr, state.hMap, 0);
						state.data_ptr = nullptr;
					}
					state.data_ptr = (uint8_t *)pNewAddr;
					state.hMap = hNewMap;
					state.data_version = ss->data_version;
					// CACHE the properties explicitly linked to THIS memory mapping
					state.width = ss->w;
					state.height = ss->h;
					state.ImageFormat = ss->ImageFormat;
					state.mapped_size = new_total_size;
					state.dataA_offset = ss->dataA_offset; // Store these in ScreenState
					state.dataB_offset = ss->dataB_offset;

					ss->ServerSwitched = false; // Acknowledge
					InitLockClient(ss);
					godot::UtilityFunctions::print("Successfully switched to ", data_name);
				}
			}

			if (!state.data_ptr || state.data_ptr == nullptr) {
				continue;
			}
			int safe_w = state.width;
			int safe_h = state.height;
			int safe_format = state.ImageFormat;
			int frame_size = safe_w * safe_h * 4;
			if (LockScreen(ss, false, 1)) {
				if(!ss->isNewFrame){
					UnlockScreen(ss, false);
					continue;
				}
				PackedByteArray temp_buffer;
				temp_buffer.resize(frame_size);

				uint8_t *dst = temp_buffer.ptrw();
				bool readfromb = false;
				if (ss->Server_B_Available) {
					if (!ss->ServerWritingB) {
						readfromb = true;
						ss->ClientReadingB = true;
					} else {
						readfromb = false;
						ss->ClientReadingA = true;
					}
				} else {
					if (!ss->ServerWritingA) {
						readfromb = false;
						ss->ClientReadingA = true;
					} else {
						readfromb = true;
						ss->ClientReadingB = true;
					}
				}
				MemsBar();
				uint32_t safe_offset = readfromb ? state.dataB_offset : state.dataA_offset;
				// if (safe_offset + frame_size > state.mapped_size) {
				// 	// 内存不足以读取这一帧，放弃并释放锁
				// 	if (readfromb) ss->ClientReadingB = false;
				// 	else ss->ClientReadingA = false;
				// 	MemsBar();
				// 	continue;
				// }
				uint8_t *src = state.data_ptr + safe_offset;
				if (src == nullptr) {
					if (readfromb) {
						ss->ClientReadingB = false;
					} else {
						ss->ClientReadingA = false;
					}
					MemsBar();
					godot::UtilityFunctions::print("continue2");
					continue;
				}
				int type_group = PIXMAN_FORMAT_TYPE(ss->ImageFormat);
				for (int i = 0; i < frame_size; i += 4) {
					// 兼容 PIXMAN_a8r8g8b8, PIXMAN_x8r8g8b8, PIXMAN_r8g8b8 等所有 BGR 排列
					if (type_group == PIXMAN_TYPE_ARGB || type_group == PIXMAN_TYPE_BGRA) {
						dst[i + 0] = src[i + 2]; // R = 内存中的 B
						dst[i + 1] = src[i + 1]; // G
						dst[i + 2] = src[i + 0]; // B = 内存中的 R
						dst[i + 3] = 255; // 强制 Alpha，防止黑屏
					}
					// 兼容 PIXMAN_a8b8g8r8, PIXMAN_rgba 等 RGB 排列
					else if (type_group == PIXMAN_TYPE_ABGR || type_group == PIXMAN_TYPE_RGBA) {
						dst[i + 0] = src[i + 0]; // R
						dst[i + 1] = src[i + 1]; // G
						dst[i + 2] = src[i + 2]; // B
						dst[i + 3] = 255;
					} else {
						// 保底：即使是未知格式，也将 Alpha 设为 255
						memcpy(dst + i, src + i, 4);
						dst[i + 3] = 255;
					}
				}
				if (readfromb) {
					ss->ClientReadingB = false;
				} else {
					ss->ClientReadingA = false;
				}
				ss->isNewFrame = false;
				MemsBar();
				UnlockScreen(ss, false);
				{
					state.swap_mutex->lock();
					state.ready_buffer = temp_buffer;
					state.ready_width = safe_w;
					state.ready_height = safe_h;
					state.has_new_frame = true;
					state.swap_mutex->unlock();
				}
			}
		}
	}
}
bool ReaderClass::get_usable(int id) {
	if (qemu_conn.BS == nullptr)
		return false;
	BufferStruct *bs = qemu_conn.BS;
	if (!bs) {
		godot::UtilityFunctions::print("BufferStruct");

		return false;
	}
	int screen_count = bs->screen_count;
	if (id < 0 || id >= screen_count) {
		godot::UtilityFunctions::print("id out:", id);

		return false;
	}

	ScreenState &state = screens_state[id];

	// 即使 data_ptr 暂时为空，只要宽高有效，就认为可用（可以显示旧帧或占位）
	return (state.data_ptr != nullptr) || (state.width > 0 && state.height > 0);
}
int session_id = 0;

void _initialize_session_id() {
	// 存储路径：user:// 永远指向程序专属的可写目录
	godot::String path = "user://last_qemu_id.dat";

	// 1. 尝试读取
	if (godot::FileAccess::file_exists(path)) {
		godot::Ref<godot::FileAccess> f = godot::FileAccess::open(path, godot::FileAccess::READ);
		if (f.is_valid()) {
			session_id = f->get_32(); // 读取上一次的 ID
			f->close();
		}
	}

	// 2. 自增
	session_id++;

	// 3. 立即写回，确保下次启动拿到的是新值
	godot::Ref<godot::FileAccess> f = godot::FileAccess::open(path, godot::FileAccess::WRITE);
	if (f.is_valid()) {
		f->store_32(session_id);
		f->close();
	}

	godot::UtilityFunctions::print("QEMU Session Started. Unique ID: ", session_id);
}
void ReaderClass::_ready() {
}
void ReaderClass::_exit_tree() {
	godot::UtilityFunctions::print("_exit_tree");
	if (!qemu_conn.BS)
		return;
	stopMachine();
}
void ReaderClass::startMachine(TypedArray<String> args) {
		godot::UtilityFunctions::print("startMachine");
		std::vector<std::string> qemu_args;
		for (int i = 0; i < args.size(); i++) {
			qemu_args.push_back(args[i].operator String().utf8().get_data());
		}
		std::string exe_path;
		std::string arch = godot::Engine::get_singleton()->get_architecture_name().utf8().ptr();
		#ifdef _WIN32
			std::string exe_ext = ".exe";
		#else
			std::string exe_ext = "";
		#endif
		if (!godot::Engine::get_singleton()->is_embedded_in_editor()) {
			godot::String exe_path_Godot = godot::OS::get_singleton()->get_executable_path().get_base_dir();
			godot::UtilityFunctions::print("exe_path_Godot:", exe_path_Godot);
			godot::String qemu_name = godot::String("qemu-system-") + arch.c_str() + exe_ext.c_str();
			exe_path_Godot = exe_path_Godot.path_join("bin").path_join(qemu_name);
			exe_path = exe_path_Godot.utf8().ptr();
		}else{
			godot::String project_path_godot = godot::ProjectSettings::get_singleton()->globalize_path("res://");
			godot::String qemu_name = godot::String("qemu-system-") + arch.c_str() + exe_ext.c_str();
			project_path_godot = project_path_godot.path_join("bin").path_join(qemu_name);
			exe_path = project_path_godot.utf8().ptr();
			godot::UtilityFunctions::print("project_path_godot:", project_path_godot);
		}
		godot::UtilityFunctions::print("guess qemu path:", exe_path.c_str());
		qemu_conn = startQemuAndConnectToBuffer(exe_path, qemu_args, session_id);
		if (qemu_conn.haveId) {
			current_qemu_id = qemu_conn.id;
			std::stringstream ss;
			ss << "sharedbuf_" << current_qemu_id;
			current_qemu_id_str = ss.str();
		}
		worker.instantiate();
		worker->start(callable_mp(this, &ReaderClass::updatemessgae), Thread::PRIORITY_NORMAL);
		running = true;
}

void ReaderClass::stopMachine() {
	if (!qemu_conn.BS)
		return;
	godot::UtilityFunctions::print("stopMachine");
	shutdownQemuID(current_qemu_id);

	// 释放所有屏幕的共享内存
	for (auto &state : screens_state) {
		if (state.data_ptr) {
			disconnect_shm(state.data_ptr, state.hMap, 0);
			state.data_ptr = nullptr;
		}
		if (state.swap_mutex.is_valid()) {
			state.swap_mutex.unref();
		}
	}
	screens_state.clear();
	qemu_conn.BS = nullptr;
}

int ReaderClass::get_screen_count() const {
	if (!qemu_conn.BS)
		return 0;
	return screens_state.size();
}

int ReaderClass::get_width(int index) const {
	if (index >= 0 && index < screens_state.size())
		return screens_state[index].width;
	return 0;
}

int ReaderClass::get_height(int index) const {
	if (index >= 0 && index < screens_state.size())
		return screens_state[index].height;
	return 0;
}

Ref<ImageTexture> ReaderClass::get_texture(int index) const {
	if (index >= 0 && index < screens_state.size())
		return screens_state[index].texture;
	return Ref<ImageTexture>();
}

void ReaderClass::sync_connection(double delta) {
	if (!qemu_conn.BS)
		return;
	int screen_count;
	{
		screens_mutex->lock();
		screen_count = screens_state.size();

		for (int idx = 0; idx < screen_count; idx++) {
			ScreenState &state = screens_state[idx];

			PackedByteArray frame_data;
			int w, h;
			bool new_frame = false;
			if (state.has_new_frame) {
				{
					if (state.has_new_frame) {
						frame_data = state.ready_buffer;
						w = state.ready_width;
						h = state.ready_height;
						state.has_new_frame = false;
						new_frame = true;
					}
					state.swap_mutex->unlock();
				}
			}
			if (new_frame && frame_data.size() > 0) {
				// 更新图像
				if (state.image.is_null() || state.image->get_width() != w || state.image->get_height() != h) {
					state.image = Image::create_from_data(w, h, false, Image::FORMAT_RGBA8, frame_data);
				} else {
					state.image->set_data(w, h, false, Image::FORMAT_RGBA8, frame_data);
				}

				// 更新纹理
				if (state.texture.is_null() || state.texture->get_width() != w || state.texture->get_height() != h) {
					state.texture = ImageTexture::create_from_image(state.image);
				} else {
					state.texture->update(state.image);
				}
			}
		}
		screens_mutex->unlock();
	}
}
