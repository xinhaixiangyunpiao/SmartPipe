#include <string.h>
#include <opencv2/opencv.hpp> 
#include "data.h"

namespace sp{

Data::Data(){
    this->id = 0;
    this->type = MAT;
    this->context.mat.channels = 3;
    this->context.mat.rows = 0;
    this->context.mat.cols = 0;
    this->context.mat.data = nullptr;
}

Data::Data(short id, uint8_t type, int channels, int rows, int cols, unsigned char* data){
    this->id = id;
    this->type = type;
    this->context.mat.channels = channels;
    this->context.mat.rows = rows;
    this->context.mat.cols = cols;
    this->context.mat.data = data;
}

Data::Data(short id, uint8_t type, int x, int y, int width, int height){
    this->id = id;
    this->type = type;
    this->context.rect.x = x;
    this->context.rect.y = y;
    this->context.rect.width = width;
    this->context.rect.height = height;
}

void Data::set(int channels, int rows, int cols, unsigned char* data){
    this->context.mat.channels = channels;
    this->context.mat.rows = rows;
    this->context.mat.cols = cols;
    this->context.mat.data = data;
}

void Data::set(int x, int y, int width, int height){
    this->context.rect.x = x;
    this->context.rect.y = y;
    this->context.rect.width = width;
    this->context.rect.height = height;
}

Data::~Data(){

}

};