#ifndef _Pipe_H_
#define _Pipe_H_

#include <vector>
#include <thread>
#include <string>
#include <memory>
#include <map>
#include <sys/types.h>
#include <Python.h>
#include "functions.h"
#include "profile.h"
#include "data.h"
#include "memory.h"

namespace sp{

// class declear
class Function;
class Profile;

class Executor{
protected:
    short id;                                      // Executor的id
    short lcore_id;                                // 绑定的核号, 默认亲和的核是id%(NUM_CORES)
    pid_t pid;                                     // 进程pid
    std::string name;                              // Excutor的functions字符串拼接
    std::vector<Function*> functions;              // 包含的所有function
    SharedMemory* sharedMemory;                    // 每个executor有一个共享内存的管理类
    bool hasPythonFunction;                        // 有Python的Function
    bool hasGpuFunction;                           // 有GPU的Function
public:
    Executor(short id, std::vector<Function*> functions);
    ~Executor();
    short getId();
    pid_t getPid();
    void start(); // 开始进程的执行
    void addFunction(Function* function); // 添加function
    void delFunction(std::string name);   // 移除function
    void setAffinity(short lcore_id);     // 设置核的亲和性 
    char* malloc(uint8_t type);
    char* malloc(int width, int height);
    void free(char* ptr);
    int pos(char* ptr); // 根据指针得到在内存池中的位置
    char* get(uint8_t type, int pos); // 根据位置得到元素首地址
    char* get(int width, int height, int pos); // 根据位置得到元素首地址
};

};

#endif