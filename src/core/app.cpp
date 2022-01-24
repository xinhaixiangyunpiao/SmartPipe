#include <string>
#include <cassert>
#include <sys/types.h>
#include <sys/wait.h>
#include "app.h"
#include "executor.h"
#include "common.h"
#include "functions.h"

namespace sp{

// 根据Fs_table和最大核数自动建立部署表从而建立App
App::App(short id, std::vector<std::tuple<std::string, std::vector<int>, std::vector<int>, std::vector<std::string>>>& Fs_table, int max_cores){
    this->id = id;
    if(PROFILE)
        profile = new Profile();
    else
        profile = nullptr;
    /** 建立部署表 **/
    std::map<int, std::vector<int>> D_table; // 部署表，每个stage负责哪几号task
    /**
        算法
    **/

/** 建立App **/
    // 建立Qs
    for(int task_id = 0; task_id < Fs_table.size(); task_id++){
        for(auto j : std::get<2>(Fs_table[task_id])){
            int next_task_id = j;
            std::string name = "q_" + std::to_string(task_id) + "_" + std::to_string(next_task_id);
            BlockingQueue<Data*>* q = new BlockingQueue<Data*>(name);
            Qs[name] = q;
        }
    }
    // 根据D_table建立function和functions_map，并建立executor，然后绑定executor，Queues，profile，设置batchsize
    for(auto i : D_table){
        // 建立Fucntion
        for(auto j : i.second){
            std::string function_name = std::get<0>(Fs_table[j]);
            std::vector<std::string> params = std::get<3>(Fs_table[j]);
            std::cout << function_name << " " << params.size() << std::endl;
            Function* function = Function::create(function_name, params);
            // bind Queues
            std::vector<BlockingQueue<Data*>*> pre_Qs, next_Qs;
            for(auto j : std::get<1>(Fs_table[j])){
                int pre_task_id = j;
                std::string name = "q_" + std::to_string(pre_task_id) + "_" + std::to_string(j);
                assert(Qs.find(name) != Qs.end());
                pre_Qs.push_back(Qs.find(name)->second);
            }
            for(auto j : std::get<2>(Fs_table[j])){
                int next_task_id = j;
                std::string name = "q_" + std::to_string(j) + "_" + std::to_string(next_task_id);
                assert(Qs.find(name) != Qs.end());
                next_Qs.push_back(Qs.find(name)->second);
            }
            function->bindQueues(pre_Qs, next_Qs);
            // bind Profile
            function->setProfile(profile);
            // 加入function_map
            functions_map[i.first].push_back(function);
        }
        // 建立Executor
        Executor* executor = new Executor(i.first, functions_map[i.first]);
        executors.push_back(executor);
        // bind Executor
        for(auto function : functions_map[i.first]){
            function->bindExecutor(executor);
        }
    }
}

App::App(short id, std::vector<std::tuple<std::string, std::vector<int>, std::vector<int>, std::vector<std::string>>>& Fs_table, std::map<int, std::vector<int>>& D_table){
    this->id = id;
    if(PROFILE)
        profile = new Profile();
    else
        profile = nullptr;
    /** 建立App **/
    // 建立Qs
    for(int task_id = 0; task_id < Fs_table.size(); task_id++){
        for(auto j : std::get<2>(Fs_table[task_id])){
            int next_task_id = j;
            std::string name = "q_" + std::to_string(task_id) + "_" + std::to_string(next_task_id);
            BlockingQueue<Data*>* q = new BlockingQueue<Data*>(name);
            Qs[name] = q;
        }
    }
    // 根据D_table建立function和functions_map，并建立executor，然后绑定executor，Queues，profile，设置batchsize
    for(auto i : D_table){
        // 建立Fucntion
        for(auto j : i.second){
            std::string function_name = std::get<0>(Fs_table[j]);
            std::vector<std::string> params = std::get<3>(Fs_table[j]);
            Function* function = Function::create(function_name, params);
            // bind Queues
            std::vector<BlockingQueue<Data*>*> pre_Qs, next_Qs;
            for(auto k : std::get<1>(Fs_table[j])){
                int pre_task_id = k;
                std::string name = "q_" + std::to_string(pre_task_id) + "_" + std::to_string(j);
                assert(Qs.find(name) != Qs.end());
                pre_Qs.push_back(Qs.find(name)->second);
            }
            for(auto k : std::get<2>(Fs_table[j])){
                int next_task_id = k;
                std::string name = "q_" + std::to_string(j) + "_" + std::to_string(next_task_id);
                assert(Qs.find(name) != Qs.end());
                next_Qs.push_back(Qs.find(name)->second);
            }
            function->bindQueues(pre_Qs, next_Qs);
            // bind Profile
            function->setProfile(profile);
            // 加入function_map
            functions_map[i.first].push_back(function);
        }
        // 建立Executor
        Executor* executor = new Executor(i.first, functions_map[i.first]);
        executors.push_back(executor);
        // bind Executor
        for(auto function : functions_map[i.first]){
            function->bindExecutor(executor);
        }
    }
}

void App::start(){
    for(auto executor : executors){
        executor->start();
    }
}

App::~App(){
    // 释放Qs,Functions,Executors的内存空间
}

void App::waitForComplete(){
    for(auto executor : executors){
        int exit_status = -1;
        waitpid(executor->getPid(), &exit_status, 0); // 阻塞等待子进程结束。子进程执行结束会进入僵尸状态Z状态，直到此函数执行完毕才会彻底杀死。
        assert(WIFEXITED(exit_status) != 0); // 子进程正常退出
    }
    if(PROFILE && profile != nullptr){
        profile->profile();
    }
}

};