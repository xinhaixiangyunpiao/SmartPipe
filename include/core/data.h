#ifndef _DATA_H_
#define _DATA_H_

#include "common.h"

namespace sp{

class Data{
private:
    // 禁止类的拷贝和复制
    Data(const Data&) = delete;
    Data& operator= (const Data&) = delete;
public:
    Data();
    Data(short id, uint8_t type, int channels, int rows, int cols, unsigned char* data);
    Data(short id, uint8_t type, int x, int y, int width, int height);
    ~Data();
    void set(int dims, int rows, int cols, unsigned char* data);
    void set(int x, int y, int width, int height);
    short id;
    uint8_t type;
    union{
        struct{
            int channels;
            int rows;
            int cols;
            unsigned char* data;
        } mat;
        struct{
            int x;
            int y;
            int width;
            int height;
        } rect;
    } context;
};

};

#endif