#ifndef _CONTEXT_H_
#define _CONTEXT_H_

#include <string>
#include <vector>
#include "app.h"

namespace sp{

class Context{
private:
    // App
    std::vector<App*> apps;
    
public:
    Context();
    ~Context();
    void addApp(std::string config_path); // 向系统中添加一个app，app自动会开始运行，自动profile。
};

};

#endif