#pragma once
#include "utils.h"
#include <godot_cpp/variant/utility_functions.hpp>
using namespace godot;
BufferStruct *connect_to_control_shm(const char *name, HANDLE &hMap) {
	void *ptr = nullptr;
#ifdef _WIN32
	hMap = OpenFileMappingA(
			FILE_MAP_ALL_ACCESS,
			FALSE,
			name);

	if (hMap == NULL) {
		return nullptr;
	}
	ptr = MapViewOfFile(
			hMap,
			FILE_MAP_ALL_ACCESS,
			0,
			0,
			0);
#else
	int fd = shm_open(name, O_RDWR, 0666);
	if (fd < 0)
		return nullptr;
    struct stat statbuf;
	if (fstat(fd, &statbuf) < 0) {
		close(fd);
        godot::UtilityFunctions::print("shm_open failed! errno: ", errno, " name: ", name);
		return nullptr;
	}

	hMap = fd;
	// 修正：映射 statbuf.st_size 而不是 sizeof(BufferStruct)
	ptr = mmap(NULL, statbuf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	close(fd);
#endif

	return (BufferStruct *)ptr;
}

bool connect_to_data_shm(const char *name, size_t size, HANDLE &hMap,uint8_t **out) {
    void *ptr = nullptr;
#ifdef _WIN32
    hMap = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, name);
	godot::UtilityFunctions::print("OpenFileMappingA "," name:",name);
    if (hMap == NULL){
		godot::UtilityFunctions::print("OpenFileMappingA NULL!",(int)GetLastError()," name:",name);
		return false;
	}
    ptr = MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, size);
	if(ptr == NULL){
		godot::UtilityFunctions::print("MapViewOfFile NULL!",(int)GetLastError());
		return false;
	}
#else
    hMap = shm_open(name, O_RDWR, 0666);
    if (hMap < 0) return false;
    ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, hMap, 0);
    if (ptr == MAP_FAILED) {
        godot::UtilityFunctions::print("shm_open failed! errno: ", errno, " name: ", name);
        close(hMap);
        return false;
    }
    close(hMap);
#endif
	*out = (uint8_t *)ptr;
	return true;
}

#ifdef _WIN32
void read_pipe_to_godot(HANDLE hRead) {
#else
void read_pipe_to_godot(int fdRead) {
#endif
    char buffer[4096];
    std::string line_accumulator;

#ifdef _WIN32
    DWORD bytesRead;
    while (ReadFile(hRead, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
#else
    ssize_t bytesRead;
    while ((bytesRead = read(fdRead, buffer, sizeof(buffer) - 1)) > 0) {
#endif
        buffer[bytesRead] = '\0';
        line_accumulator += buffer;

        size_t newline_pos;
        while ((newline_pos = line_accumulator.find('\n')) != std::string::npos) {
            std::string full_line = line_accumulator.substr(0, newline_pos);
            // 去掉可能存在的 \r
            if (!full_line.empty() && full_line.back() == '\r') full_line.pop_back();

            godot::UtilityFunctions::print("[QEMU] ", full_line.c_str());
            line_accumulator.erase(0, newline_pos + 1);
        }
    }

    if (!line_accumulator.empty()) {
        godot::UtilityFunctions::print("[QEMU] ", line_accumulator.c_str());
    }

#ifdef _WIN32
    CloseHandle(hRead);
#else
    close(fdRead);
#endif
}

void disconnect_shm(void *ptr, HANDLE hMap, size_t size) {
#ifdef _WIN32
    if (ptr) UnmapViewOfFile(ptr);
    if (hMap) CloseHandle(hMap);
#else
    if (ptr && size > 0) munmap(ptr, size);
#endif
}

// 等待控制块共享内存出现，最多尝试 timeout_ms 毫秒
BufferStruct* wait_for_control_shm(const char* ctrl_name, HANDLE& hMap, int timeout_ms) {
    auto start = std::chrono::steady_clock::now();
    BufferStruct* bs = nullptr;
    while (bs == nullptr) {
        bs = connect_to_control_shm(ctrl_name, hMap);
        if (bs != nullptr) break;
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count() > timeout_ms) {
            return nullptr;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
  return bs;
}

void shutdownQemuID(int id) {
    auto it = processes.find(id);
    if (it == processes.end()) return;
#ifdef _WIN32
    DWORD pid = it->second;

    // 重新打开进程句柄，请求终止权限
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE | SYNCHRONIZE, FALSE, pid);
    if (hProcess == NULL) {
        DWORD err = GetLastError();
        godot::UtilityFunctions::print("OpenProcess failed, error: ", std::to_string(err).c_str()," trying taskkill");
        // 即使打开失败，仍尝试用 taskkill
        std::string cmd = "taskkill /F /T /PID " + std::to_string(pid);
        system(cmd.c_str());
        processes.erase(id);
        return;
    }

    // 尝试终止进程
    if (TerminateProcess(hProcess, 0)) {
        godot::UtilityFunctions::print("TerminateProcess succeeded for PID ", std::to_string(pid).c_str());
    } else {
        DWORD err = GetLastError();
        godot::UtilityFunctions::print("TerminateProcess failed, error: ", std::to_string(err).c_str());
    }
    CloseHandle(hProcess);
#else
    pid_t pid = it->second;
    kill(pid, SIGKILL);
#endif

    processes.erase(id);
}

void shutdownAllQemu(){
	for(auto &it : processes) {
		shutdownQemuID(it.first);
    }
	processes.clear();
}
bool isQemuRunning(int process_index) {
    if (process_index < 0 || process_index >= processes.size()) return false;

#ifdef _WIN32
    DWORD pid = processes[process_index];
	HANDLE hProcess = OpenProcess(PROCESS_TERMINATE | SYNCHRONIZE, FALSE, pid);
    if (hProcess == NULL) {
        return false;
    }
    DWORD result = WaitForSingleObject(hProcess, 0);
    CloseHandle(hProcess);
    return (result == WAIT_TIMEOUT);
#else
    pid_t pid = processes[process_index];
   // 发送信号 0 可以检测进程是否存在且我们是否有权限发送信号
    if (kill(pid, 0) == 0) {
        return true;
    }
    // 如果 kill 失败，可能需要 waitpid 清理僵尸进程
    int status;
    waitpid(pid, &status, WNOHANG);
    return false;
#endif
}
SharedBufferConnection startQemuAndConnectToBuffer(std::string qemuPath, std::vector<std::string> args,int session_id = 0) {
    SharedBufferConnection conn;
    conn.h = 0;
    conn.ptr = nullptr;
    std::string id_str = "sharedbuf_" + std::to_string(session_id);
    std::string ctrl_name = "/QemuCtrl_" + id_str;

    // 2. 构造 QEMU 命令行参数
    std::vector<std::string> full_args;
    full_args.push_back(qemuPath);
    full_args.push_back("--display");
   	full_args.push_back("sharedbuffer,id=" + id_str);
    full_args.insert(full_args.end(), args.begin(), args.end());

    // 3. 转换为 C 风格参数数组
    std::vector<char*> argv;
    for (auto& arg : full_args) {
        argv.push_back(const_cast<char*>(arg.c_str()));
    }
    argv.push_back(nullptr);

#ifdef _WIN32
	std::string cmdline;
    for (size_t i = 0; i < full_args.size(); ++i) {
        if (i > 0) cmdline += " ";
        if (full_args[i].find(' ') != std::string::npos) {
            cmdline += "\"" + full_args[i] + "\"";
        } else {
            cmdline += full_args[i];
        }
    }
	HANDLE hReadPipe, hWritePipe;
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE; // 必须设置为 TRUE，子进程才能继承写端
    sa.lpSecurityDescriptor = NULL;

    if (!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0)) {
        UtilityFunctions::printerr("Failed to create pipe for QEMU output");
    }

    // 确保父进程不继承管道的读端
    SetHandleInformation(hReadPipe, HANDLE_FLAG_INHERIT, 0);

    STARTUPINFOA si = { sizeof(si) };
    si.dwFlags |= STARTF_USESTDHANDLES;
    si.hStdOutput = hWritePipe; // 重定向 stdout
    si.hStdError = hWritePipe;  // 重定向 stderr

    PROCESS_INFORMATION pi = { 0 };

    BOOL success = CreateProcessA(
        NULL,
        cmdline.data(),
        NULL,
        NULL,
        TRUE,               // 允许句柄继承
        CREATE_NO_WINDOW,
        NULL,
        NULL,
        &si,
        &pi
    );
		godot::UtilityFunctions::print("[QEMU cmdline] ", cmdline.data());

    if (!success) {
        UtilityFunctions::printerr((std::string("Failed to start QEMU process. Error: ") + std::to_string(GetLastError())).c_str());
        CloseHandle(hReadPipe);
        CloseHandle(hWritePipe);
        return conn;
    }
	HANDLE hJob = CreateJobObject(NULL, NULL);
	JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli = { 0 };
	jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
	SetInformationJobObject(hJob, JobObjectExtendedLimitInformation, &jeli, sizeof(jeli));

	// 启动 QEMU 后将进程句柄加入 Job
	AssignProcessToJobObject(hJob, pi.hProcess);
    DWORD pid = pi.dwProcessId;
    processes.insert_or_assign(session_id, pid);

    // 关闭不再需要的句柄
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    CloseHandle(hWritePipe);
    // 启动后台线程读取输出，避免阻塞 Godot 主线程
    std::thread outputThread(read_pipe_to_godot, hReadPipe);
    outputThread.detach(); // 分离线程，让它独立运行
#else
    pid_t pid;
    int ret = posix_spawnp(&pid, qemuPath.c_str(), NULL, NULL, argv.data(), environ);
    if (ret != 0) {
        UtilityFunctions::printerr("Failed to start QEMU process. Error: ", strerror(ret));
        return conn;
    }

    processes.insert_or_assign(id_num,pid);
#endif

    // 4. 等待控制块共享内存出现并连接
    HANDLE hMap = 0;
    BufferStruct* bs = wait_for_control_shm(ctrl_name.c_str(), hMap, 5000);
    if (bs == nullptr) {
        UtilityFunctions::printerr("Timeout waiting for QEMU control shared memory: ", ctrl_name.c_str());
        return conn;
    }

    // 5. 填充返回结构
    conn.h = hMap;
    conn.BS = bs;
	conn.type = CONN_BUFFER_STRUCT;
	conn.id = session_id;
	conn.haveId = true;
    return conn;
}
