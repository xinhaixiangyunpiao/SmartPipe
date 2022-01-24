#include <thread>
#include <chrono>
#include <memory>
#include <iomanip>
#include <unistd.h>
#include <iostream>
#include <pthread.h>
#include <sys/time.h>
#include <sched.h>
#include "data.h"
#include "executor.h"
#include "util.h"
#include "common.h"

namespace sp{

Executor::Executor(short id, std::vector<Function*> functions){
    this->id = id;
    this->lcore_id = id%80;
    this->functions = functions;
    name = "";
    hasPythonFunction = false;
    hasGpuFunction = false;
    for(auto function : functions){
        name += function->getName() + " ";
        if(function->isPythonFunction())
            hasPythonFunction = true;
        if(function->isGpuFunction())
            hasGpuFunction = true;
    }
    // 根据function的情况确定是否初始化python环境和初始化cuda环境。
    if(hasPythonFunction){
        // 初始化Python执行环境
        Py_Initialize();
        // 初始化numpy环境
        
    }
    if(hasGpuFunction){
        // 初始化GPU环境

    }
}

Executor::~Executor(){
    if(hasPythonFunction){
        Py_Finalize();
    }
    if(hasGpuFunction){
        // 结束Gpu环境

    }
}

short Executor::getId(){
    return id;
}

pid_t Executor::getPid(){
    return pid;
}

void Executor::start(){
    /**
        开启进程，在子进程中执行这些
    **/
    pid_t pid = fork();
    if(pid){
        // 父进程
        this->pid = pid;
        // 将子进程绑定核
        cpu_set_t mask;
        CPU_ZERO(&mask);    //置空
        CPU_SET(lcore_id, &mask);   //设置亲和力值
        if(sched_setaffinity(pid, sizeof(mask), &mask) == -1){
            std::cout << "warning: could not set CPU affinity, continuing..." << std::endl;
        }
    }else{
        // 子进程
        // 初始化sharedMemory对象
        sharedMemory = new SharedMemory();
        // 初始化队列
        for(std::vector<Function*>::iterator it = functions.begin(); it != functions.end(); it++){
            Function* function = *it;
            function->initQueues();
        }
        // 进行每个function的start
        for(std::vector<Function*>::iterator it = functions.begin(); it != functions.end(); it++){
            Function* function = *it;
            function->start();
        }
        sleep(14 - id*7); // 先初始化后面的Executor
        // 初始化变量
        std::vector<std::vector<Data*>> data;
        std::vector<Data*> data_out;
        // 循环处理
        long cnt = 0;
        while(true){
            // 所有function结束后跳出
            if(functions.size() == 0)
                break;
            // 每次都会循环一次functions中的function
            for(std::vector<Function*>::iterator it = functions.begin(); it != functions.end();){
                Function* function = *it;
                // 清理
                data.clear();
                data_out.clear();

                // 接收
                std::cout << function->getName() << ": " << cnt << " before recv" << std::endl;
                function->recvFromQueues(data);
                std::cout << function->getName() << ": " << cnt << " after recv" << std::endl;

                // 判断退出
                if(function->isHeadFunction() == false && data.empty()){
                    function->finish();
                    delete function;
                    it = functions.erase(it);
                    continue;
                }

                // 处理
                std::cout << function->getName() << ": " << cnt << " before handle" << std::endl;
                function->handle(data, data_out);
                std::cout << function->getName() << ": " << cnt << " after handle" << std::endl;

                // 判断退出
                if(function->isHeadFunction() && data_out.size() != 0 && data_out[0] == DATA_END){
                    function->finish();
                    delete function;
                    it = functions.erase(it);
                    continue;
                }

                // 发送
                std::cout << function->getName() << ": " << cnt << " before send" << std::endl;
                function->sendToQueues(data_out);
                std::cout << function->getName() << ": " << cnt << " after send" << std::endl;

                // 退出
                if(function->isEnd()){
                    function->finish();
                    delete function;
                    it = functions.erase(it);
                    continue;
                }
                it++;
            }
            cnt++;
        }
        delete sharedMemory;
        std::cout << name << " exit." << std::endl;
        exit(1); // 正常退出
    }
}

void Executor::addFunction(Function* function){
    functions.push_back(function);
}

void Executor::delFunction(std::string function_name){
    for(std::vector<Function*>::iterator it = functions.begin(); it != functions.end(); it++){
        if((*it)->getName() == function_name){
            delete (*it);
            functions.erase(it);
            break;
        }
    }
}

void Executor::setAffinity(short lcore_id){
    this->lcore_id = lcore_id%80;
}

char* Executor::malloc(uint8_t type){
    if(sharedMemory == nullptr){
        assert(false);
    }else{
        return sharedMemory->malloc(type);
    }
}

char* Executor::malloc(int width, int height){
    if(sharedMemory == nullptr){
        assert(false);
    }else if(width <= 960 && height <= 540){
        return sharedMemory->malloc(Cv_Mat_Data_960_540_Memory);
    }else if(width <= 1280 && height <= 720){
        return sharedMemory->malloc(Cv_Mat_Data_1280_720_Memory);
    }else if(width <= 1920 && height <= 1080){
        return sharedMemory->malloc(Cv_Mat_Data_1920_1080_Memory);
    }else if(width <= 3840 && height <= 2160){
        return sharedMemory->malloc(Cv_Mat_Data_3840_2160_Memory);
    }else{
        assert(false);
    }
}

void Executor::free(char* ptr){
    if(sharedMemory != nullptr){
        sharedMemory->free(ptr);
    }else{
        assert(false);
    }
}

int Executor::pos(char* ptr){
    if(ptr == nullptr)
        return -1;
    if(sharedMemory != nullptr){
        sharedMemory->pos(ptr);
    }else{
        assert(false);
    }
}

char* Executor::get(uint8_t type, int pos){
    if(sharedMemory != nullptr){
        if(pos == -1)
            return nullptr;
        sharedMemory->get(type, pos);
    }else{
        assert(false);
    }
}

char* Executor::get(int width, int height, int pos){
    if(pos == -1)
        return nullptr;
    if(sharedMemory == nullptr){
        assert(false);
    }else if(width <= 960 && height <= 540){
        return sharedMemory->get(Cv_Mat_Data_960_540_Memory, pos);
    }else if(width <= 1280 && height <= 720){
        return sharedMemory->get(Cv_Mat_Data_1280_720_Memory, pos);
    }else if(width <= 1920 && height <= 1080){
        return sharedMemory->get(Cv_Mat_Data_1920_1080_Memory, pos);
    }else if(width <= 3840 && height <= 2160){
        return sharedMemory->get(Cv_Mat_Data_3840_2160_Memory, pos);
    }else{
        assert(false);
    }
}

}