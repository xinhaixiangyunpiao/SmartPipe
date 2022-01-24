#ifndef _MODEL_H_
#define _MODEL_H_

#include <Python.h>
#include "functions.h"

/*
    模型相关的函数封装在这里
*/

namespace sp{

namespace Model{
    namespace Yolo{
        class yolo_inference: public PythonFunction, public GpuFunction{
        DECLEAR_DYNCRT_CLASS("Model-Yolo-yolo_inference", yolo_inference, Function);
        private:
            
        public:
            yolo_inference(std::vector<std::string>& v);
            ~yolo_inference();
            virtual void loadModel();
            virtual void start();
            virtual void process(std::vector<std::vector<Data*>>& data_input, std::vector<Data*>& data_output);
            virtual void finish();
        };
    };

    namespace Openpose{
        class openpose_preprocess: public PythonFunction{
        DECLEAR_DYNCRT_CLASS("Model-Openpose-openpose_preprocess", openpose_preprocess, Function);
        private:

        public:
            openpose_preprocess(std::vector<std::string>& v);
            ~openpose_preprocess();
            virtual void process(std::vector<std::vector<Data*>>& data_input, std::vector<Data*>& data_output);
        };

        class openpose_inference: public PythonFunction, public GpuFunction{
        DECLEAR_DYNCRT_CLASS("Model-Openpose-openpose_inference", openpose_inference, Function);
        private:
            
        public:
            openpose_inference(std::vector<std::string>& v);
            ~openpose_inference();
            virtual void loadModel();
            virtual void start();
            virtual void process(std::vector<std::vector<Data*>>& data_input, std::vector<Data*>& data_output);
            virtual void finish();
        };

        class openpose_postprocess: public PythonFunction{
        DECLEAR_DYNCRT_CLASS("Model-Openpose-openpose_postprocess", openpose_inference, Function);
        private:

        public:
            openpose_postprocess(std::vector<std::string>& v);
            ~openpose_postprocess();
            virtual void process(std::vector<std::vector<Data*>>& data_input, std::vector<Data*>& data_output);
        };
    };
};

};

#endif