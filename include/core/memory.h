#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <iostream>
#include <string.h>
#include <memory>
#include <data.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <list>
#include <atomic>
#include <opencv2/opencv.hpp>

namespace sp{

// 要申请空间的数据类型
#define Data_Memory 0x00
#define Cv_Mat_Data_3840_2160_Memory 0x01
#define Cv_Mat_Data_1920_1080_Memory 0x02
#define Cv_Mat_Data_1280_720_Memory 0x03
#define Cv_Mat_Data_960_540_Memory 0x04

class SharedMemory{
private:
    /**
     * @brief 静态变量
     * 
     */
    // shm标识符
    static key_t occ_shm_id;
    static key_t data_shm_id;
    static key_t cv_mat_data_3840_2160_shm_id;
    static key_t cv_mat_data_1920_1080_shm_id;
    static key_t cv_mat_data_1280_720_shm_id;
    static key_t cv_mat_data_960_540_shm_id;
    // 单位长度
    static size_t data_unit_length;
    static size_t cv_mat_data_3840_2160_unit_length;
    static size_t cv_mat_data_1920_1080_unit_length;
    static size_t cv_mat_data_1280_720_unit_length;
    static size_t cv_mat_data_960_540_unit_length;
    // 容量
    static int data_capacity;
    static int cv_mat_data_3840_2160_capacity;
    static int cv_mat_data_1920_1080_capacity;
    static int cv_mat_data_1280_720_capacity;
    static int cv_mat_data_960_540_capacity;
    // 为了进程安全，使用位图表征某个位置是否可用，不使用链表，时间复杂度会从O(1)上升到O(n),但是可以线程安全。
    /**
     * @brief 每个进程的私有变量
     * 
     */
    char* occ_ptr;
    char* data_shm_ptr;
    char* cv_mat_data_3840_2160_shm_ptr;
    char* cv_mat_data_1920_1080_shm_ptr;
    char* cv_mat_data_1280_720_shm_ptr;
    char* cv_mat_data_960_540_shm_ptr;
    // 遍历时使用CAS操作保证线程安全
    std::atomic<bool>* data_occ_ptr;
    std::atomic<bool>* cv_mat_3840_2160_occ_ptr;
    std::atomic<bool>* cv_mat_1920_1080_occ_ptr;
    std::atomic<bool>* cv_mat_1280_720_occ_ptr;
    std::atomic<bool>* cv_mat_960_540_occ_ptr;

public:
    SharedMemory();
    ~SharedMemory();
    static void Initialized();
    static void Finalize();
    char* malloc(uint8_t type);
    void free(char* ptr);
    int pos(char* ptr); // 根据指针得到在内存池中的位置
    char* get(uint8_t type, int pos); // 根据位置得到首地址
};

};

#endif
