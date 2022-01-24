#include <Python.h>
#include "models.h"

namespace sp{

namespace Model{
    namespace Yolo{
        IMPLEMENT_DYNCRT_CLASS(yolo_inference);
        std::vector<std::string> pModeulePaths {"/home/lx/SmartPipe/models/yolo/", "/home/lx/SmartPipe/models/yolo/repo/yolov5/"};
        yolo_inference::yolo_inference(std::vector<std::string>& v): PythonFunction(pModeulePaths, "yolo", "yolo"), GpuFunction(){
            assert(v.size() == 0);
            name = "yolo_inference";
        }

        yolo_inference::~yolo_inference(){

        }

        void yolo_inference::loadModel(){
            PyObject_CallMethod(pInstance, "load_model", "");
        }

        void yolo_inference::start(){
            // 构建实例
            PythonFunction::start();
            pInstance = PyObject_CallObject(pConstruct, nullptr);
            GpuFunction::start();
        }

        void yolo_inference::finish(){
            PythonFunction::finish();
            GpuFunction::finish();
        }

        void yolo_inference::process(std::vector<std::vector<Data*>>& data_input, std::vector<Data*>& data_output){
            assert(data_input.size() != 0);
            assert(data_input[0].size() == 1);
            // 将data_input中的数据提取出来并转成numpy矩阵
            PyObject* data = convertToPython(data_input);
            // 调用python函数
            pArgs = PyTuple_New(1);
            PyTuple_SetItem(pArgs, 0, data);
            pReturn = PyObject_CallMethod(pInstance, "inference", "O", pArgs);
            // 将结果解析成Data并赋值给data_output
            convertToCPlusPlus(pReturn, data_output);
        }


    };

    namespace Openpose{

    };
};

};