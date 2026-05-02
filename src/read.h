#pragma once
#include "godot_cpp/classes/node2d.hpp"
#include "godot_cpp/classes/sprite2d.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/classes/image.hpp"
#include "godot_cpp/classes/image_texture.hpp"
#include <godot_cpp/classes/mutex.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/thread.hpp>
#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/os.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/classes/thread.hpp>
#include <exception>
#include <sstream>
#include <string>
#include "utils.h"
#include <string>
#include "lock.h"
#include <atomic>
#include <vector>
#define PIXMAN_FORMAT(bpp,type,a,r,g,b)	(((bpp) << 24) |  \
					 ((type) << 16) | \
					 ((a) << 12) |	  \
					 ((r) << 8) |	  \
					 ((g) << 4) |	  \
					 ((b)))

#define PIXMAN_FORMAT_BYTE(bpp,type,a,r,g,b) \
	(((bpp >> 3) << 24) | \
	(3 << 22) | ((type) << 16) | \
	((a >> 3) << 12) | \
	((r >> 3) << 8) | \
	((g >> 3) << 4) | \
	((b >> 3)))
#define PIXMAN_TYPE_OTHER	0
#define PIXMAN_TYPE_A		1
#define PIXMAN_TYPE_ARGB	2
#define PIXMAN_TYPE_ABGR	3
#define PIXMAN_TYPE_COLOR	4
#define PIXMAN_TYPE_GRAY	5
#define PIXMAN_TYPE_YUY2	6
#define PIXMAN_TYPE_YV12	7
#define PIXMAN_TYPE_BGRA	8
#define PIXMAN_TYPE_RGBA	9
#define PIXMAN_TYPE_ARGB_SRGB	10
#define PIXMAN_TYPE_RGBA_FLOAT	11

#define PIXMAN_FORMAT_COLOR(f)				\
	(PIXMAN_FORMAT_TYPE(f) == PIXMAN_TYPE_ARGB ||	\
	 PIXMAN_FORMAT_TYPE(f) == PIXMAN_TYPE_ABGR ||	\
	 PIXMAN_FORMAT_TYPE(f) == PIXMAN_TYPE_BGRA ||	\
	 PIXMAN_FORMAT_TYPE(f) == PIXMAN_TYPE_RGBA ||	\
	 PIXMAN_FORMAT_TYPE(f) == PIXMAN_TYPE_RGBA_FLOAT)

typedef enum {
/* 128bpp formats */
    PIXMAN_rgba_float =	PIXMAN_FORMAT_BYTE(128,PIXMAN_TYPE_RGBA_FLOAT,32,32,32,32),
/* 96bpp formats */
    PIXMAN_rgb_float =	PIXMAN_FORMAT_BYTE(96,PIXMAN_TYPE_RGBA_FLOAT,0,32,32,32),

/* 32bpp formats */
    PIXMAN_a8r8g8b8 =	 PIXMAN_FORMAT(32,PIXMAN_TYPE_ARGB,8,8,8,8),
    PIXMAN_x8r8g8b8 =	 PIXMAN_FORMAT(32,PIXMAN_TYPE_ARGB,0,8,8,8),
    PIXMAN_a8b8g8r8 =	 PIXMAN_FORMAT(32,PIXMAN_TYPE_ABGR,8,8,8,8),
    PIXMAN_x8b8g8r8 =	 PIXMAN_FORMAT(32,PIXMAN_TYPE_ABGR,0,8,8,8),
    PIXMAN_b8g8r8a8 =	 PIXMAN_FORMAT(32,PIXMAN_TYPE_BGRA,8,8,8,8),
    PIXMAN_b8g8r8x8 =	 PIXMAN_FORMAT(32,PIXMAN_TYPE_BGRA,0,8,8,8),
    PIXMAN_r8g8b8a8 =	 PIXMAN_FORMAT(32,PIXMAN_TYPE_RGBA,8,8,8,8),
    PIXMAN_r8g8b8x8 =	 PIXMAN_FORMAT(32,PIXMAN_TYPE_RGBA,0,8,8,8),
    PIXMAN_x14r6g6b6 =	 PIXMAN_FORMAT(32,PIXMAN_TYPE_ARGB,0,6,6,6),
    PIXMAN_x2r10g10b10 = PIXMAN_FORMAT(32,PIXMAN_TYPE_ARGB,0,10,10,10),
    PIXMAN_a2r10g10b10 = PIXMAN_FORMAT(32,PIXMAN_TYPE_ARGB,2,10,10,10),
    PIXMAN_x2b10g10r10 = PIXMAN_FORMAT(32,PIXMAN_TYPE_ABGR,0,10,10,10),
    PIXMAN_a2b10g10r10 = PIXMAN_FORMAT(32,PIXMAN_TYPE_ABGR,2,10,10,10),

/* sRGB formats */
    PIXMAN_a8r8g8b8_sRGB = PIXMAN_FORMAT(32,PIXMAN_TYPE_ARGB_SRGB,8,8,8,8),

/* 24bpp formats */
    PIXMAN_r8g8b8 =	 PIXMAN_FORMAT(24,PIXMAN_TYPE_ARGB,0,8,8,8),
    PIXMAN_b8g8r8 =	 PIXMAN_FORMAT(24,PIXMAN_TYPE_ABGR,0,8,8,8),

/* 16bpp formats */
    PIXMAN_r5g6b5 =	 PIXMAN_FORMAT(16,PIXMAN_TYPE_ARGB,0,5,6,5),
    PIXMAN_b5g6r5 =	 PIXMAN_FORMAT(16,PIXMAN_TYPE_ABGR,0,5,6,5),

    PIXMAN_a1r5g5b5 =	 PIXMAN_FORMAT(16,PIXMAN_TYPE_ARGB,1,5,5,5),
    PIXMAN_x1r5g5b5 =	 PIXMAN_FORMAT(16,PIXMAN_TYPE_ARGB,0,5,5,5),
    PIXMAN_a1b5g5r5 =	 PIXMAN_FORMAT(16,PIXMAN_TYPE_ABGR,1,5,5,5),
    PIXMAN_x1b5g5r5 =	 PIXMAN_FORMAT(16,PIXMAN_TYPE_ABGR,0,5,5,5),
    PIXMAN_a4r4g4b4 =	 PIXMAN_FORMAT(16,PIXMAN_TYPE_ARGB,4,4,4,4),
    PIXMAN_x4r4g4b4 =	 PIXMAN_FORMAT(16,PIXMAN_TYPE_ARGB,0,4,4,4),
    PIXMAN_a4b4g4r4 =	 PIXMAN_FORMAT(16,PIXMAN_TYPE_ABGR,4,4,4,4),
    PIXMAN_x4b4g4r4 =	 PIXMAN_FORMAT(16,PIXMAN_TYPE_ABGR,0,4,4,4),

/* 8bpp formats */
    PIXMAN_a8 =		 PIXMAN_FORMAT(8,PIXMAN_TYPE_A,8,0,0,0),
    PIXMAN_r3g3b2 =	 PIXMAN_FORMAT(8,PIXMAN_TYPE_ARGB,0,3,3,2),
    PIXMAN_b2g3r3 =	 PIXMAN_FORMAT(8,PIXMAN_TYPE_ABGR,0,3,3,2),
    PIXMAN_a2r2g2b2 =	 PIXMAN_FORMAT(8,PIXMAN_TYPE_ARGB,2,2,2,2),
    PIXMAN_a2b2g2r2 =	 PIXMAN_FORMAT(8,PIXMAN_TYPE_ABGR,2,2,2,2),

    PIXMAN_c8 =		 PIXMAN_FORMAT(8,PIXMAN_TYPE_COLOR,0,0,0,0),
    PIXMAN_g8 =		 PIXMAN_FORMAT(8,PIXMAN_TYPE_GRAY,0,0,0,0),

    PIXMAN_x4a4 =	 PIXMAN_FORMAT(8,PIXMAN_TYPE_A,4,0,0,0),

    PIXMAN_x4c4 =	 PIXMAN_FORMAT(8,PIXMAN_TYPE_COLOR,0,0,0,0),
    PIXMAN_x4g4 =	 PIXMAN_FORMAT(8,PIXMAN_TYPE_GRAY,0,0,0,0),

/* 4bpp formats */
    PIXMAN_a4 =		 PIXMAN_FORMAT(4,PIXMAN_TYPE_A,4,0,0,0),
    PIXMAN_r1g2b1 =	 PIXMAN_FORMAT(4,PIXMAN_TYPE_ARGB,0,1,2,1),
    PIXMAN_b1g2r1 =	 PIXMAN_FORMAT(4,PIXMAN_TYPE_ABGR,0,1,2,1),
    PIXMAN_a1r1g1b1 =	 PIXMAN_FORMAT(4,PIXMAN_TYPE_ARGB,1,1,1,1),
    PIXMAN_a1b1g1r1 =	 PIXMAN_FORMAT(4,PIXMAN_TYPE_ABGR,1,1,1,1),

    PIXMAN_c4 =		 PIXMAN_FORMAT(4,PIXMAN_TYPE_COLOR,0,0,0,0),
    PIXMAN_g4 =		 PIXMAN_FORMAT(4,PIXMAN_TYPE_GRAY,0,0,0,0),

/* 1bpp formats */
    PIXMAN_a1 =		 PIXMAN_FORMAT(1,PIXMAN_TYPE_A,1,0,0,0),

    PIXMAN_g1 =		 PIXMAN_FORMAT(1,PIXMAN_TYPE_GRAY,0,0,0,0),

/* YUV formats */
    PIXMAN_yuy2 =	 PIXMAN_FORMAT(16,PIXMAN_TYPE_YUY2,0,0,0,0),
    PIXMAN_yv12 =	 PIXMAN_FORMAT(12,PIXMAN_TYPE_YV12,0,0,0,0)
} pixman_format_code_t;

#define PIXMAN_FORMAT_TYPE(f)	(((f) >> 16) & 0x3f)
static inline void MemsBar(void) {
#ifdef _WIN32
    MemoryBarrier();
#else
    __sync_synchronize();
#endif
}
using namespace godot;

// --- 新增：为每个屏幕封装一个状态结构体 ---
struct ScreenState {
    uint32_t data_version = 0xFFFFFFFF;
    uint8_t* data_ptr = nullptr;
    uint32_t dataA_offset;    /* 始终为 0 */
    uint32_t dataB_offset;

    HANDLE hMap = 0;
    Ref<Image> image;
    Ref<ImageTexture> texture;
    int width = 0;
    int height = 0;
	int ImageFormat = 0;       // ADD THIS
    size_t mapped_size = 0;    // ADD THIS
	PackedByteArray pending_buffer;  // 后台线程填充
    PackedByteArray ready_buffer;    // 主线程读取
    int ready_width = 0;
    int ready_height = 0;
    bool has_new_frame = false;
    Ref<Mutex> swap_mutex;           // 保护 pending/ready 交换

};

class ReaderClass : public Node2D {
    GDCLASS(ReaderClass, Node2D)

protected:
    static void _bind_methods();
	void _notification(int p_what);

private:
    SharedBufferConnection qemu_conn;
    int current_qemu_id = 0;
    std::string current_qemu_id_str; // 用于安全地格式化字符串
	Ref<Thread> worker;
	bool running;
    // --- 新增：使用 vector 动态存储多个屏幕的状态 ---
    std::vector<ScreenState> screens_state;
	Ref<Mutex> screens_mutex;
    uint32_t current_button_state;        // 跟踪按钮状态
public:
    ReaderClass();
    ~ReaderClass() override;
    void _ready()override;
	bool get_usable(int id);

	void _exit_tree()override;

	void startMachine(TypedArray<String> args);
    void stopMachine();

    // --- 修改：增加 index 参数，获取特定屏幕的数据 ---
    int get_screen_count() const;
    int get_width(int index) const;
    int get_height(int index) const;
    Ref<ImageTexture> get_texture(int index) const;
	void updatemessgae();
    void sync_connection(double delta);
	    void send_key_event(int godot_key, bool pressed);
    void send_mouse_button_state(uint32_t button_state);
    void send_mouse_motion(int x, int y, int godot_w, int godot_h);
	void send_mouse_wheel(int delta_y);
};
