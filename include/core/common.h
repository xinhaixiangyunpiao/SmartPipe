#ifndef _COMMON_H_
#define _COMMON_H_

#include <mutex>
#include <memory>
#include <string>
#include <opencv2/opencv.hpp> 
#include "data.h"

// 全局变量，调试用
extern cv::Mat* DATA_SOURCE;

namespace sp{
    // declear
    class Data;

    // version
    static const std::string VERSION = "0.0.1";

    // parameters
    static const int MAX_PARALLEL = 10;

    // debug
    static const bool RECORD = true;
    static const bool PROFILE = true;

    // data
    static uint8_t UNKNOW = 0x00;
    static uint8_t MAT = 0x01;
    static uint8_t RECT = 0x02;

    // smartpipe
    static Data* DATA_END = nullptr;

    // print
    static std::mutex GLOBAL_LOCK;
};

#endif