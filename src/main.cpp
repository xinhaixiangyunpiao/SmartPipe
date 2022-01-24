#include <iostream>
#include <unistd.h>
#include <opencv2/opencv.hpp>
#include "app.h"
#include "data.h"
#include "executor.h"
#include "profile.h"
#include "common.h"
#include "mdReader.h"
#include "functions.h"
#include "memory.h"

cv::Mat* DATA_SOURCE;

/*
    初始化
    将视频流读入内存
*/
void init(cv::Mat** data_source){
    cv::VideoCapture capture("/data/lx/SmartPipe/data_source/videos/road.webm");
    long size = 50;
    *data_source = new cv::Mat[size];
    for(long i = 0; i < size; i++){
        std::cout << i << std::endl;
        capture >> (*data_source)[i];
    }
    capture.release();
}

/*
    结束释放资源
*/
void finish(cv::Mat* data_source){
    delete[] data_source;
}

void test(){
    sp::MDReader mdReader("/home/lx/SmartPipe/config/app3.md");
    auto Fs_table = mdReader.getFsTable();
    auto D_table = mdReader.getDeployTable();
    sp::App* app = new sp::App(0, Fs_table, D_table);
    app->start();
    app->waitForComplete();
}

int main()
{
    std::cout << "pid: " << getpid() << std::endl;
    sp::SharedMemory::Initialized();
    init(&DATA_SOURCE);
    test();
    finish(DATA_SOURCE);
    sp::SharedMemory::Finalize();
    return 0;
}