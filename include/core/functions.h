#ifndef _FUNCTION_H_
#define _FUNCTION_H_

#include <ctime>
#include <vector>
#include <string>
#include <memory>
#include <sys/time.h>
#include <opencv2/opencv.hpp> 
#include "profile.h"
#include "define.h"
#include "data.h"
#include "executor.h"
#include "queue.hpp" 

namespace sp{

class Profile;
class Executor;

/*
    所有方法的基类
*/
class Function{
DECLEAR_DYNCRT_BASE(Function);
protected:
    std::string name;      // Function的名字 - Function的标识
    int batch_size;        // batch大小
    Profile* profile;      // Profile指针
    Executor* executor;    // Executor指针
    bool Python_Function;  // 是不是Python Function
    bool Gpu_Function;     // 是不是GPU Function
    std::vector<BlockingQueue<Data*>*> pre_Qs;  // Function对应的输入队列集合
    std::vector<BlockingQueue<Data*>*> next_Qs; // Function对应的输出队列集合
    bool is_end;           // 判断function是否结束
public:
    // 主进程方法
    Function();
    ~Function();
    std::string getName();
    void setBatchSize(int batch_size);
    void setProfile(Profile* profile);
    void bindQueues(std::vector<BlockingQueue<Data*>*> pre_Qs, std::vector<BlockingQueue<Data*>*> next_Qs);
    void bindExecutor(Executor* executor);
    // 运行时方法，在子进程中运行。
    void initQueues();
    void recvFromQueues(std::vector<std::vector<Data*>>& data);
    void sendToQueues(std::vector<Data*>& data);
    void handle(std::vector<std::vector<Data*>>& data_input, std::vector<Data*>& data_output);
    virtual void start();
    virtual void process(std::vector<std::vector<Data*>>& data_input, std::vector<Data*>& data_output) = 0;
    virtual void finish();
    bool isPythonFunction();
    bool isGpuFunction();
    bool isHeadFunction();
    bool isTailFunction();
    bool isEnd();
};

class PythonFunction: virtual public Function{
protected:
    /** Python 环境相关 **/
    std::vector<std::string> pModulePaths; // 对应的python文件路径
    std::string pModuleName; // 对应的python文件模块名
    std::string pClassName;  // 对应的python类名
    PyObject* pModule;       // Module对象
    PyObject* pDict;         // pModule的属性字典
    PyObject* pClass;        // 目标类
    PyObject* pConstruct;    // 目标类的构造函数 
    PyObject* pInstance;     // 目标类的一个实例
    PyObject* pArgs;         // 函数参数
    PyObject* pReturn;       // 函数返回值
public:
    PythonFunction(std::vector<std::string> pModulePaths, std::string pModuleName, std::string pClassName);
    ~PythonFunction();
    virtual void start();
    virtual void finish();
    PyObject* convertToPython(std::vector<std::vector<Data*>>& data_input);
    void convertToCPlusPlus(PyObject* data, std::vector<Data*>& data_output);
};

class GpuFunction: virtual public Function{
protected:

public:
    GpuFunction();
    ~GpuFunction();
    virtual void start();
    virtual void finish();
    virtual void loadModel() = 0; // 将模型加载到进程中，每个继承自GpuFunction的Fucntion需要重写这个方法。
};

/* 
    所有处理图片的Function
*/
namespace Image{

namespace Gen{
    /*
        从磁盘中读取视频流
    */
    class genFromDisk: public Function{
    DECLEAR_DYNCRT_CLASS("Image-Gen-genFromDisk", genFromDisk, Function);
    private:
        std::string path;
        int fps;
        int width;
        int height;
        cv::VideoCapture capture;
        struct timeval time_start;
        long cnt = 0;
    public:
        genFromDisk(std::vector<std::string>& v); // std::string path, int fps, int width, int height
        ~genFromDisk();
        virtual void process(std::vector<std::vector<Data*>>& data_input, std::vector<Data*>& data_output);
    };

    /* 
        从内存中读取视频流
    */
    class genFromMemory: public Function{
    DECLEAR_DYNCRT_CLASS("Image-Gen-genFromMemory", genFromMemory, Function);
    private:
        cv::Mat* data_source;
        long index;
        int fps;
        long size;
        struct timeval time_start;
        long cnt;
        bool firstFlag;
        
    public:
        genFromMemory(std::vector<std::string>& v); // long fps, cv::Mat* data_source, long size
        ~genFromMemory();
        virtual void process(std::vector<std::vector<Data*>>& data_input, std::vector<Data*>& data_output);
    };
};

namespace Resize{
    class resize: public Function{
    DECLEAR_DYNCRT_CLASS("Image-Resize-resize", resize, Function);
    private:
        int width;
        int height;
    public:
        resize(std::vector<std::string>& v); // int width, int height
        ~resize();
        virtual void process(std::vector<std::vector<Data*>>& data_input, std::vector<Data*>& data_output);
    };
};

namespace Crop{
    /* 
        crop大小在参数中
    */
    class cropWithParas: public Function{
    DECLEAR_DYNCRT_CLASS("Image-Crop-cropWithParas", cropWithParas, Function);
    private:
        float w_min;
        float w_max;
        float h_min;
        float h_max;
    public:
        cropWithParas(std::vector<std::string>& v); // float w_min, float w_max, float h_min, float h_max
        ~cropWithParas();
        virtual void process(std::vector<std::vector<Data*>>& data_input, std::vector<Data*>& data_output);
    };

    /*
        crop大小在在输入
    */
    class cropWithInput: public Function{
    DECLEAR_DYNCRT_CLASS("Image-Crop-cropWithInput", cropWithInput, Function);
    public:
        cropWithInput(std::vector<std::string>& v);
        ~cropWithInput();
        virtual void process(std::vector<std::vector<Data*>>& data_input, std::vector<Data*>& data_output);
    };
};

namespace Save{
    class save: public Function{
    DECLEAR_DYNCRT_CLASS("Image-Save-save", save, Function);
        std::string path;
        int fps;
        int width;
        int height;
        cv::VideoWriter writer;
    public:
        save(std::vector<std::string>& v); // std::string path, int fps, int width, int height
        ~save();
        virtual void process(std::vector<std::vector<Data*>>& data_input, std::vector<Data*>& data_output);
    };
};

namespace Trans{
    class trans: public Function{
    DECLEAR_DYNCRT_CLASS("Image-Trans-trans", trans, Function);
    public:
        trans(std::vector<std::string>& v);
        ~trans();
        virtual void process(std::vector<std::vector<Data*>>& data_input, std::vector<Data*>& data_output);
    };
};

};

/* 
    所有处理表格的Function
*/
namespace Table{

namespace Build{

};

namespace Filter{

};

namespace Select{

};

};

};
#endif