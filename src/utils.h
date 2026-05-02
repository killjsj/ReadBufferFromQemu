#pragma once
#include <stdio.h>
#include <map>
#include <string>
#include "godot_cpp/variant/utility_functions.hpp"
#ifdef _WIN32
    #include <windows.h>
#else
    #include <sys/mman.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <unistd.h>
	#include <spawn.h>
	#include <sys/wait.h>
	extern char **environ; // posix_spawn 需要用到环境变量指针
#endif
#include "structDefine.h"
#include <vector>
#include <thread>
enum ConnectionType {
    CONN_NONE,
    CONN_SINGLE_SCREEN,
    CONN_BUFFER_STRUCT
};

struct SharedBufferConnection {
    HANDLE h;
    ConnectionType type;

    SingleScreen *SS;
    BufferStruct *BS;
    void* ptr;

    int id = -1;
    bool haveId = false; // haveId 也不要和 id 放进 union
};
#ifdef _WIN32
static std::map<int,DWORD> processes;
#else
static std::map<int,pid_t> processes;
#endif
void shutdownQemuID(int id);
bool connect_to_data_shm(const char *name, size_t size, HANDLE &hMap,uint8_t **out);

BufferStruct *connect_to_control_shm(const char *name, HANDLE &hMap);
BufferStruct* wait_for_control_shm(const char* ctrl_name, HANDLE& hMap, int timeout_ms = 5000);
void disconnect_shm(void *ptr, HANDLE hMap, size_t size = 0);
void shutdownAllQemu();
bool isQemuRunning(int process_index);
SharedBufferConnection startQemuAndConnectToBuffer(std::string qemuPath, std::vector<std::string> args,int session_id);
