#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION

#include <ctime>
#include <vector>
#include <string>
#include <memory>
#include <cassert>
#include <sys/time.h>
#include <opencv2/opencv.hpp>
#include <numpy/arrayobject.h> 
#include "data.h"
#include "common.h"
#include "functions.h"

namespace sp{

int* init_numpy(){//初始化 numpy 执行环境，主要是导入包，python2.7用void返回类型，python3.0以上用int返回类型
    import_array();
}

IMPLEMENT_DYNCRT_BASE(Function);
Function::Function(){
    name = "Undefined";
    batch_size = 1;
    profile = nullptr;
    executor = nullptr;
    Python_Function = false;
    Gpu_Function = false;
    is_end = false;
}

Function::~Function(){
    // 表示某个function运行结束，会向next_Qs发送结束信号。
    for(auto q : next_Qs){
        for(int i = 0; i < MAX_PARALLEL; i++)
            q->enqueue(DATA_END);
    }
    std::cout << name << " function ended." << std::endl;
}

std::string Function::getName(){
    return name;
}

void Function::setBatchSize(int batch_size){
    this->batch_size = batch_size;
}

void Function::setProfile(Profile* profile){
    this->profile = profile;
}

void Function::bindQueues(std::vector<BlockingQueue<Data*>*> pre_Qs, std::vector<BlockingQueue<Data*>*> next_Qs){
    this->pre_Qs = pre_Qs;
    this->next_Qs = next_Qs;
}

void Function::bindExecutor(Executor* executor){
    this->executor = executor;
}

void Function::initQueues(){
    for(auto q : pre_Qs){
        q->open_named_pipe('r');
    }
    for(auto q : next_Qs){
        q->open_named_pipe('w');
    }
}

void Function::recvFromQueues(std::vector<std::vector<Data*>>& data){
    if(pre_Qs.size() == 0)
        return;
    for(int i = 0; i < batch_size; i++){
        std::vector<Data*> data_ptr_vec;
        for(auto q : pre_Qs){
            Data* data_ptr = nullptr;
            q->wait_dequeue(data_ptr);
            if(data_ptr == DATA_END){
                is_end = true;
                break;
            }
            data_ptr_vec.push_back(data_ptr);
        }
        if(data_ptr_vec.size() == 0)
            break;
        data.push_back(data_ptr_vec);
    }
}

void Function::sendToQueues(std::vector<Data*>& data){
    if(next_Qs.size() == 0)
        return;
    assert(data.size() <= batch_size);
    for(int i = 0; i < data.size(); i++){
        for(auto q : next_Qs){
            q->enqueue(data[i]);
        }
    }
}

void Function::start(){
    // check env
    assert(executor != nullptr);
    if(PROFILE)
        assert(profile != nullptr);
    assert(pre_Qs.size() != 0 || next_Qs.size() != 0);
}

void Function::finish(){

}

void Function::handle(std::vector<std::vector<Data*>>& data_input, std::vector<Data*>& data_output){
    // if data_input is empty, gen data container.
    if(data_input.size() == 0){
        char* ptr = executor->malloc(Data_Memory);
        Data* data = new (ptr)Data();
        std::vector<Data*> v;
        v.push_back(data);
        data_input.push_back(v);
    }
    struct timeval t;
    // before process
    gettimeofday(&t, NULL);
    if(PROFILE && profile != nullptr){
        for(auto i : data_input){
            for(auto j : i){
                if(j != DATA_END){
                    profile->update_data_lifecycle_detail_map(j->id, executor->getId(), "before " + name, t);
                }
            }
        }
    }
    // process
    this->process(data_input, data_output);
    // after process
    gettimeofday(&t, NULL);
    if(PROFILE && profile != nullptr){
        for(auto i : data_output){
            if(i != DATA_END){
                profile->update_data_lifecycle_detail_map(i->id, executor->getId(), "after " + name, t);
            }
        }
    }
}

bool Function::isPythonFunction(){
    return Python_Function;
}

bool Function::isGpuFunction(){
    return Gpu_Function;
}

bool Function::isHeadFunction(){
    if(pre_Qs.empty())
        return true;
    else
        return false;
}

bool Function::isTailFunction(){
    if(next_Qs.empty())
        return true;
    else
        return false;
}

bool Function::isEnd(){
    return is_end;
}

PythonFunction::PythonFunction(std::vector<std::string> pModulePaths, std::string pModuleName, std::string pClassName){
    Python_Function = true;
    this->pModulePaths = pModulePaths;
    this->pModuleName = pModuleName;
    this->pClassName = pClassName;
}

PythonFunction::~PythonFunction(){

}

void PythonFunction::start(){
    init_numpy();
    PyRun_SimpleString("import sys");
    for(std::string pModulePath : pModulePaths){
        std::string command = "sys.path.append('" + pModulePath + "')";
        PyRun_SimpleString(command.c_str());
    }
    pModule = PyImport_ImportModule(pModuleName.c_str());
    pDict = PyModule_GetDict(pModule);
    pClass = PyDict_GetItemString(pDict, pClassName.c_str());
    pConstruct = PyInstanceMethod_New(pClass);
    // check
    assert(pModule != nullptr);
    assert(pDict != nullptr);
    assert(pClass != nullptr);
    assert(pConstruct != nullptr);
    pInstance = nullptr;
    pArgs = nullptr;
    pReturn = nullptr;
}

void PythonFunction::finish(){
    Py_DECREF(pModule);
    Py_DECREF(pDict);
    Py_DECREF(pClass);
    Py_DECREF(pConstruct);
    Py_DECREF(pInstance);
    Py_DECREF(pArgs);
    Py_DECREF(pReturn);
}

PyObject* PythonFunction::convertToPython(std::vector<std::vector<Data*>>& data_input){
    PyObject* pList = PyList_New(data_input.size());
    for(int i = 0; i < data_input.size(); i++){
        for(int j = 0; j < data_input[i].size(); j++){
            Data* d = data_input[i][j];
            cv::Mat* img = new cv::Mat(d->context.mat.rows, d->context.mat.cols, CV_8UC3);
            img->data = d->context.mat.data;
            img->datastart = img->data;
            img->dataend = img->datastart + d->context.mat.channels*d->context.mat.rows*d->context.mat.cols;
            cvtColor(*img, *img, cv::COLOR_BGR2RGB);
            int m = img->rows, n = img->cols, c = img->channels();
            npy_intp Dims[3] = {m,n,c};
            PyObject* pArray = PyArray_SimpleNewFromData(3, Dims, NPY_UBYTE, (void*)(img->data));
            PyList_SET_ITEM(pList, i, pArray); 
        }
    }
    return pList;
}

void PythonFunction::convertToCPlusPlus(PyObject* pReturn, std::vector<Data*>& data_output){
    // std::cout << "convertToCPlusPlus enter" << std::endl;
    // int size = PyList_Size(pReturn);
    // for(int i = 0; i < size; i++){
    //     PyObject* pRet = PyList_GetItem(pReturn, i);
    // }
}

GpuFunction::GpuFunction(){
    Gpu_Function = true;
}

GpuFunction::~GpuFunction(){
    
}

void GpuFunction::start(){
    loadModel();
}

void GpuFunction::finish(){

}

namespace Image{
    /*
        生成图片相关
    */
    namespace Gen{
        IMPLEMENT_DYNCRT_CLASS(genFromDisk);
        genFromDisk::genFromDisk(std::vector<std::string>& v){ // std::string path, int fps, int width, int height
            assert(v.size() == 4);
            name = "genFromDisk";
            this->path = v[0];
            this->fps = std::stoi(v[1]);
            this->width = std::stoi(v[2]);
            this->height = std::stoi(v[3]);
            capture.open(this->path);
            if(!capture.isOpened()){
                std::cout << "video open failed!" << std::endl;
            }
            gettimeofday(&time_start, NULL);
            cnt = 0;
        }

        genFromDisk::~genFromDisk(){
            capture.release();
        }

        void genFromDisk::process(std::vector<std::vector<Data*>>& data_input, std::vector<Data*>& data_output){
            // 验证输入个数
            assert(data_input.size() != 0);
            assert(data_input[0].size() == 1);
            // 生成图片
            for(int i = 0; i < batch_size; i++){
                char* mat_data_ptr = executor->malloc(height, width);
                cv::Mat* frame = new cv::Mat(height, width, CV_8UC3);
                frame->data = (uchar*)mat_data_ptr;
                frame->datastart = frame->data;
                frame->dataend = frame->datastart + height*width*3;
                frame->datalimit = frame->dataend;
                capture >> *frame;
                if(frame->empty()){
                    data_output.push_back(DATA_END);
                    break;
                }else{
                    data_input[i][0]->set(frame->channels(), frame->rows, frame->cols, (uchar*)mat_data_ptr);
                    data_output.push_back(data_input[i][0]);
                }
            }
            struct timeval t;
            gettimeofday(&t, NULL);
            double time_cost = (t.tv_sec + ((double)t.tv_usec/1000000)) - (time_start.tv_sec + ((double)time_start.tv_usec/1000000));
            while(time_cost < ((double)1.0/fps)*cnt){
                gettimeofday(&t, NULL);
                time_cost = (t.tv_sec + ((double)t.tv_usec/1000000)) - (time_start.tv_sec + ((double)time_start.tv_usec/1000000));
            }
            cnt++;
        }

        IMPLEMENT_DYNCRT_CLASS(genFromMemory);
        genFromMemory::genFromMemory(std::vector<std::string>& v){ // long fps, cv::Mat* data_source, long size
            assert(v.size() == 3);
            name = "genFromMemory";
            // this->data_source = reinterpeter_cast<cv::Mat*>(stol(v[1]));
            this->data_source = DATA_SOURCE;
            this->fps = std::stol(v[0]);
            this->size = std::stol(v[2]);
            index = 0;
            cnt = 1;
            this->firstFlag = true;
        }

        genFromMemory::~genFromMemory(){
            
        }

        void genFromMemory::process(std::vector<std::vector<Data*>>& data_input, std::vector<Data*>& data_output){
            // first
            if(firstFlag){
                gettimeofday(&time_start, NULL);
                firstFlag = false;
            }
            // 验证输入个数
            assert(data_input.size() != 0);
            assert(data_input[0].size() == 1);
            // 得到图片
            for(int i = 0; i < batch_size; i++){
                if(index >= size){
                    data_output.push_back(DATA_END);
                    break;
                }else{
                    char* mat_data_ptr = executor->malloc(3840, 2160);
                    struct timeval t0, t1;
                    memcpy(mat_data_ptr, data_source[index].data, data_source[index].rows*data_source[index].cols*data_source[index].channels());
                    data_input[i][0]->set(data_source[index].channels(), data_source[index].rows, data_source[index].cols, (uchar*)mat_data_ptr);
                    data_output.push_back(data_input[i][0]);
                }
                index++;
            }
            // 等待结束
            struct timeval t;
            gettimeofday(&t, NULL);
            double time_cost = (t.tv_sec + ((double)t.tv_usec/1000000)) - (time_start.tv_sec + ((double)time_start.tv_usec/1000000));
            while(time_cost < ((double)1.0/fps)*cnt){
                gettimeofday(&t, NULL);
                time_cost = (t.tv_sec + ((double)t.tv_usec/1000000)) - (time_start.tv_sec + ((double)time_start.tv_usec/1000000));
            }
            gettimeofday(&t, NULL);
            cnt++;
        }
    };

    /*
        图片resize相关
    */
    namespace Resize{
        IMPLEMENT_DYNCRT_CLASS(resize);
        resize::resize(std::vector<std::string>& v){ // int width, int height
            assert(v.size() == 2);
            name = "resize";
            this->width = std::stoi(v[0]);
            this->height = std::stoi(v[1]);
        }

        resize::~resize(){

        }

        void resize::process(std::vector<std::vector<Data*>>& data_input, std::vector<Data*>& data_output){
            // 验证输入个数
            assert(batch_size != 0);
            assert(data_input.size() <= batch_size);
            assert(data_input[0].size() == 1);
            // 处理
            for(int i = 0; i < data_input.size(); i++){
                // resize
                if(data_input[i][0] == DATA_END)
                    break;
                // 从接收到的data构建cv::Mat src
                Data* d = data_input[i][0];
                cv::Mat* src = new cv::Mat(d->context.mat.rows, d->context.mat.cols, CV_8UC3); 
                src->data = d->context.mat.data;
                src->datastart = d->context.mat.data;
                src->dataend = d->context.mat.data + d->context.mat.rows*d->context.mat.cols*d->context.mat.channels;
                src->datalimit = src->dataend;
                // 从内存池申请data内存，并构建cv::Mat dst
                char* mat_data_ptr = executor->malloc(width, height);
                cv::Mat* dst = new cv::Mat(height, width, CV_8UC3);
                dst->data = (uchar*)mat_data_ptr;
                dst->datastart = dst->data;
                dst->dataend = dst->datastart + height*width*3;
                dst->datalimit = dst->dataend;
                // 进行resize
                cv::resize(*src, *dst, dst->size(), 0, 0, cv::INTER_LINEAR);
                // 释放内存池内存，和cv::Mat src内存
                executor->free((char*)(src->data));
                delete src;
                // 设置Data的值
                data_input[i][0]->set(dst->channels(), dst->rows, dst->cols, dst->data);
                // 释放cv::Mat dst内存
                delete dst;
                data_output.push_back(data_input[i][0]);
            }
        }
    };

    /*
        图片裁剪相关
    */
    namespace Crop{
        IMPLEMENT_DYNCRT_CLASS(cropWithParas);
        cropWithParas::cropWithParas(std::vector<std::string>& v){ // float w_min, float w_max, float h_min, float h_max
            assert(v.size() == 4);
            name = "cropWithParas";
            this->w_min = std::stof(v[0]);
            this->w_max = std::stof(v[1]);
            this->h_min = std::stof(v[2]);
            this->h_max = std::stof(v[3]);
        }

        cropWithParas::~cropWithParas(){

        }

        void cropWithParas::process(std::vector<std::vector<Data*>>& data_input, std::vector<Data*>& data_output){
            // 验证输入
            assert(batch_size != 0);
            assert(data_input.size() <= batch_size);
            assert(data_input[0].size() == 1);
            // 处理
            // for(int i = 0; i < data_input.size(); i++){
            //     if(data_input[i][0] == DATA_END)
            //         break;
            //     Data* d = data_input[i][0];
            //     char* mat_data_ptr = executor->malloc(w_max - w_min, h_max - h_min);
            //     cv::Mat* src = new cv::Mat(d->context.mat.rows, d->context.mat.cols, CV_8UC3);
            //     src->data = d->context.mat.data;
            //     src->datastart = src->data;
            //     src->dataend = src->datastart + d->context.mat.rows*d->context.mat.cols*d->context.mat.dims;
            //     src->datalimit = src->dataend;
            //     cv::Mat* dst = new cv::Mat(h_max - h_min, w_max - w_min, CV_8UC3);
            //     dst->data = mat_data_ptr;
            //     dst->datastart = dst->data;
            //     dst->dataend = dst->datastart + (h_max - h_min)*(w_max - w_min)*3;
            //     dst->datalimit = dst->dataend;
            //     cv::Rect rect(w_min*(src->rows), h_min*(src->cols), (w_max-w_min)*(src->rows), (h_max-h_min)*(src->cols));
            //     *dst = (*src)(rect);
            //     // 释放内存，加入输出

            //     executor->free((char*)(src->data));
            //     executor->free((char*)src);
            //     data_input[i][0]->wrap(MAT, (void*)dst);
            //     data_output.push_back(data_input[i][0]);
            // }
        }

        IMPLEMENT_DYNCRT_CLASS(cropWithInput);
        cropWithInput::cropWithInput(std::vector<std::string>& v){
            assert(v.size() == 0);
            name = "cropWithInput";
        }

        cropWithInput::~cropWithInput(){

        }

        void cropWithInput::process(std::vector<std::vector<Data*>>& data_input, std::vector<Data*>& data_output){
            assert(batch_size != 0);
            assert(data_input.size() <= batch_size);
            assert(data_input[0].size() == 2);
            // for(int i = 0; i < data_input.size(); i++){
            //     if(data_input[i][0] == DATA_END || data_input[i][1] == DATA_END)
            //         break;
            //     cv::Mat* src_img = (cv::Mat*)data_input[i][0]->get();
            //     cv::Rect* src_rect = (cv::Rect*)data_input[i][1]->get();
            //     char* mat_ptr = executor->malloc(Cv_Mat_Memory);
            //     char* mat_data_ptr = executor->malloc(src_rect->width, src_rect->height);
            //     cv::Mat* dst = new (mat_ptr)cv::Mat(src_rect->height, src_rect->width, CV_8UC3, mat_data_ptr);
            //     *dst = (*src_img)(*src_rect);
            //     // 释放内存，加入输出
            //     executor->free((char*)(src_img->data));
            //     executor->free((char*)src_img);
            //     executor->free((char*)src_rect);
            //     data_input[i][0]->wrap(MAT, (void*)dst);
            //     data_output.push_back(data_input[i][0]);
            // }
        }

    };

    /* 
        图片存储相关
    */
    namespace Save{
        IMPLEMENT_DYNCRT_CLASS(save);
        save::save(std::vector<std::string>& v){ // std::string path, int fps, int width, int height
            assert(v.size() == 4);
            name = "save";
            this->path = v[0];
            this->fps = std::stoi(v[1]);
            this->width = std::stoi(v[2]);
            this->height = std::stoi(v[3]);
            writer.open(this->path, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), this->fps, cv::Size(this->width, this->height), true);
        }

        save::~save(){
            writer.release();
        }

        void save::process(std::vector<std::vector<Data*>>& data_input, std::vector<Data*>& data_output){
            // 验证输入个数
            assert(batch_size != 0);
            assert(data_input.size() <= batch_size);
            assert(data_input[0].size() == 1);
            // 处理
            // for(int i = 0; i < data_input.size(); i++){
            //     if(data_input[i][0] == DATA_END)
            //         break;
            //     cv::Mat* src = (cv::Mat*)data_input[i][0]->get();
            //     writer << *src;
            //     // 释放内存
            //     data_input[i][0]->release();
            // }
        }
    };

    /*
        图片转发
    */
    namespace Trans{
        IMPLEMENT_DYNCRT_CLASS(trans);
        trans::trans(std::vector<std::string>& v){
            assert(v.size() == 0);
            name = "trans";
        }

        trans::~trans(){

        }

        void trans::process(std::vector<std::vector<Data*>>& data_input, std::vector<Data*>& data_output){
            // 验证输入个数
            assert(batch_size != 0);
            assert(data_input.size() <= batch_size);
            assert(data_input[0].size() == 1);
            // 处理
            for(int i = 0; i < data_input.size(); i++){
                if(data_input[i][0] == DATA_END)
                    break;
                data_output.push_back(data_input[i][0]);
            }
        }
    }
};

namespace Table{

};

};