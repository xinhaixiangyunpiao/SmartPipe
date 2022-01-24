#ifndef _APP_H_
#define _APP_H_

#include <unordered_map>
#include <string>
#include <vector>
#include "executor.h"
#include "profile.h"
#include "functions.h"

namespace sp{

class App{
private:
    short id;
    Profile* profile;
    std::vector<Executor*> executors;
    std::map<int, std::vector<Function*>> functions_map; // executor_id 到 vector<Function*>的映射。
    std::map<std::string, BlockingQueue<Data*>*> Qs;

public:
    App(short id, std::vector<std::tuple<std::string, std::vector<int>, std::vector<int>, std::vector<std::string>>>& Fs_table, int max_cores);
    App(short id, std::vector<std::tuple<std::string, std::vector<int>, std::vector<int>, std::vector<std::string>>>& Fs_table, std::map<int, std::vector<int>>& D_table);
    ~App();
    void start();  // 开始App的执行
    void waitForComplete(); // 阻塞当前进程，直到App执行完，并自动进行profile
};

};

#endif