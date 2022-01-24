#include "memory.h"

namespace sp{

// 初始化静态成员变量
key_t SharedMemory::occ_shm_id(0);
key_t SharedMemory::data_shm_id(0);
key_t SharedMemory::cv_mat_data_3840_2160_shm_id(0);
key_t SharedMemory::cv_mat_data_1920_1080_shm_id(0);
key_t SharedMemory::cv_mat_data_1280_720_shm_id(0);
key_t SharedMemory::cv_mat_data_960_540_shm_id(0);
size_t SharedMemory::data_unit_length(0);
size_t SharedMemory::cv_mat_data_3840_2160_unit_length(0);
size_t SharedMemory::cv_mat_data_1920_1080_unit_length(0);
size_t SharedMemory::cv_mat_data_1280_720_unit_length(0);
size_t SharedMemory::cv_mat_data_960_540_unit_length(0);
int SharedMemory::data_capacity(1000);
int SharedMemory::cv_mat_data_3840_2160_capacity(500);
int SharedMemory::cv_mat_data_1920_1080_capacity(500);
int SharedMemory::cv_mat_data_1280_720_capacity(500);
int SharedMemory::cv_mat_data_960_540_capacity(500);

SharedMemory::SharedMemory(){
    // 接入sharedMemory
    occ_ptr = (char*)shmat(occ_shm_id, NULL, 0);
    data_shm_ptr = (char*)shmat(data_shm_id, NULL, 0);
    cv_mat_data_3840_2160_shm_ptr = (char*)shmat(cv_mat_data_3840_2160_shm_id, NULL, 0);
    cv_mat_data_1920_1080_shm_ptr = (char*)shmat(cv_mat_data_1920_1080_shm_id, NULL, 0);
    cv_mat_data_1280_720_shm_ptr = (char*)shmat(cv_mat_data_1280_720_shm_id, NULL, 0);
    cv_mat_data_960_540_shm_ptr = (char*)shmat(cv_mat_data_960_540_shm_id, NULL, 0);
    // 创建变量
    data_occ_ptr = new (occ_ptr)std::atomic<bool>[data_capacity]();
    cv_mat_3840_2160_occ_ptr = new (occ_ptr + 2*1024*1024)std::atomic<bool>[cv_mat_data_3840_2160_capacity]();
    cv_mat_1920_1080_occ_ptr = new (occ_ptr + 3*1024*1024)std::atomic<bool>[cv_mat_data_1920_1080_capacity]();
    cv_mat_1280_720_occ_ptr = new (occ_ptr + 4*1024*1024)std::atomic<bool>[cv_mat_data_1280_720_capacity]();
    cv_mat_960_540_occ_ptr = new (occ_ptr + 5*1024*1024)std::atomic<bool>[cv_mat_data_960_540_capacity]();
}

SharedMemory::~SharedMemory(){
    // 进行解绑
    shmdt(occ_ptr);
    shmdt(data_shm_ptr);
    shmdt(cv_mat_data_3840_2160_shm_ptr);
    shmdt(cv_mat_data_1920_1080_shm_ptr);
    shmdt(cv_mat_data_1280_720_shm_ptr);
    shmdt(cv_mat_data_960_540_shm_ptr);
    // 置空
    data_occ_ptr = nullptr;
    cv_mat_3840_2160_occ_ptr = nullptr;
    cv_mat_1920_1080_occ_ptr = nullptr;
    cv_mat_1280_720_occ_ptr = nullptr;
    cv_mat_960_540_occ_ptr = nullptr;
}

void SharedMemory::Initialized(){
    // 初始化共享内存长度
    data_unit_length = sizeof(sp::Data);
    cv_mat_data_3840_2160_unit_length = 3840*2160*3;
    cv_mat_data_1920_1080_unit_length = 1920*1080*3;
    cv_mat_data_1280_720_unit_length = 1280*720*3;
    cv_mat_data_960_540_unit_length = 960*540*3;
    // 分配sharedMemory
    occ_shm_id = shmget(IPC_PRIVATE, 16*1024*1024, IPC_CREAT | 0666);
    data_shm_id = shmget(IPC_PRIVATE, data_capacity*data_unit_length, IPC_CREAT | 0666);
    cv_mat_data_3840_2160_shm_id = shmget(IPC_PRIVATE, cv_mat_data_3840_2160_capacity*cv_mat_data_3840_2160_unit_length, IPC_CREAT | 0666);
    cv_mat_data_1920_1080_shm_id = shmget(IPC_PRIVATE, cv_mat_data_1920_1080_capacity*cv_mat_data_1920_1080_unit_length, IPC_CREAT | 0666);
    cv_mat_data_1280_720_shm_id = shmget(IPC_PRIVATE, cv_mat_data_1280_720_capacity*cv_mat_data_1280_720_unit_length, IPC_CREAT | 0666);
    cv_mat_data_960_540_shm_id = shmget(IPC_PRIVATE, cv_mat_data_960_540_capacity*cv_mat_data_960_540_unit_length, IPC_CREAT | 0666);
    assert(occ_shm_id >= 0);
    assert(data_shm_id >= 0);
    assert(cv_mat_data_3840_2160_shm_id >= 0);
    assert(cv_mat_data_1920_1080_shm_id >= 0);
    assert(cv_mat_data_1280_720_shm_id >= 0);
    assert(cv_mat_data_960_540_shm_id >= 0);
}

void SharedMemory::Finalize(){
    // 释放共享内存
    shmid_ds sds;
    shmctl(occ_shm_id, IPC_RMID, &sds);
    shmctl(data_shm_id, IPC_RMID, &sds);
    shmctl(cv_mat_data_3840_2160_shm_id, IPC_RMID, &sds);
    shmctl(cv_mat_data_1920_1080_shm_id, IPC_RMID, &sds);
    shmctl(cv_mat_data_1280_720_shm_id, IPC_RMID, &sds);
    shmctl(cv_mat_data_960_540_shm_id, IPC_RMID, &sds);
}

char* SharedMemory::malloc(uint8_t type){
    bool old_value = false;
    bool new_value = true;
    if(type == Data_Memory){
        for(int i = 0; i < data_capacity; i++){
            if(data_occ_ptr[i].compare_exchange_strong(old_value, new_value)){
                return data_shm_ptr + data_unit_length*i;
            }
        }
    }else if(type == Cv_Mat_Data_3840_2160_Memory){
        for(int i = 0; i < cv_mat_data_3840_2160_capacity; i++){
            if(cv_mat_3840_2160_occ_ptr[i].compare_exchange_strong(old_value, new_value)){
                return cv_mat_data_3840_2160_shm_ptr + cv_mat_data_3840_2160_unit_length*i;
            }
        }
    }else if(type == Cv_Mat_Data_1920_1080_Memory){
        for(int i = 0; i < cv_mat_data_1920_1080_capacity; i++){
            if(cv_mat_1920_1080_occ_ptr[i].compare_exchange_strong(old_value, new_value)){
                return cv_mat_data_1920_1080_shm_ptr + cv_mat_data_1920_1080_unit_length*i;
            }
        }
    }else if(type == Cv_Mat_Data_1280_720_Memory){
        for(int i = 0; i < cv_mat_data_1280_720_capacity; i++){
            if(cv_mat_1280_720_occ_ptr[i].compare_exchange_strong(old_value, new_value)){
                return cv_mat_data_1280_720_shm_ptr + cv_mat_data_1280_720_unit_length*i;
            }
        }
    }else if(type == Cv_Mat_Data_960_540_Memory){
        for(int i = 0; i < cv_mat_data_960_540_capacity; i++){
            if(cv_mat_960_540_occ_ptr[i].compare_exchange_strong(old_value, new_value)){
                return cv_mat_data_960_540_shm_ptr + cv_mat_data_960_540_unit_length*i;
            }
        }
    }else{
        assert(false);
    }
}

void SharedMemory::free(char* ptr){
    if(ptr >= data_shm_ptr && ptr < data_shm_ptr + data_capacity*data_unit_length){
        assert((ptr - data_shm_ptr)%data_unit_length == 0);
        data_occ_ptr[(ptr - data_shm_ptr)/data_unit_length] = false;
    }else if(ptr >= cv_mat_data_3840_2160_shm_ptr && ptr < cv_mat_data_3840_2160_shm_ptr + cv_mat_data_3840_2160_capacity*cv_mat_data_3840_2160_unit_length){
        assert((ptr - cv_mat_data_3840_2160_shm_ptr)%cv_mat_data_3840_2160_unit_length == 0);
        cv_mat_3840_2160_occ_ptr[(ptr - cv_mat_data_3840_2160_shm_ptr)/cv_mat_data_3840_2160_unit_length] = false;
    }else if(ptr >= cv_mat_data_1920_1080_shm_ptr && ptr < cv_mat_data_1920_1080_shm_ptr + cv_mat_data_1920_1080_capacity*cv_mat_data_1920_1080_unit_length){
        assert((ptr - cv_mat_data_1920_1080_shm_ptr)%cv_mat_data_1920_1080_unit_length == 0);
        cv_mat_1920_1080_occ_ptr[(ptr - cv_mat_data_1920_1080_shm_ptr)/cv_mat_data_1920_1080_unit_length] = false;
    }else if(ptr >= cv_mat_data_1280_720_shm_ptr && ptr < cv_mat_data_1280_720_shm_ptr + cv_mat_data_1280_720_capacity*cv_mat_data_1280_720_unit_length){
        assert((ptr - cv_mat_data_1280_720_shm_ptr)%cv_mat_data_1280_720_unit_length == 0);
        cv_mat_1280_720_occ_ptr[(ptr - cv_mat_data_1280_720_shm_ptr)/cv_mat_data_1280_720_unit_length] = false;
    }else if(ptr >= cv_mat_data_960_540_shm_ptr && ptr < cv_mat_data_960_540_shm_ptr + cv_mat_data_960_540_capacity*cv_mat_data_960_540_unit_length){
        assert((ptr - cv_mat_data_960_540_shm_ptr)%cv_mat_data_960_540_unit_length == 0);
        cv_mat_960_540_occ_ptr[(ptr - cv_mat_data_960_540_shm_ptr)/cv_mat_data_960_540_unit_length] = false;
    }else{
        assert(false);
    }
}

int SharedMemory::pos(char* ptr){
    if(ptr >= data_shm_ptr && ptr < data_shm_ptr + data_capacity*data_unit_length){
        assert((ptr - data_shm_ptr)%data_unit_length == 0);
        return (ptr - data_shm_ptr)/data_unit_length;
    }else if(ptr >= cv_mat_data_3840_2160_shm_ptr && ptr < cv_mat_data_3840_2160_shm_ptr + cv_mat_data_3840_2160_capacity*cv_mat_data_3840_2160_unit_length){
        assert((ptr - cv_mat_data_3840_2160_shm_ptr)%cv_mat_data_3840_2160_unit_length == 0);
        return (ptr - cv_mat_data_3840_2160_shm_ptr)/cv_mat_data_3840_2160_unit_length;
    }else if(ptr >= cv_mat_data_1920_1080_shm_ptr && ptr < cv_mat_data_1920_1080_shm_ptr + cv_mat_data_1920_1080_capacity*cv_mat_data_1920_1080_unit_length){
        assert((ptr - cv_mat_data_1920_1080_shm_ptr)%cv_mat_data_1920_1080_unit_length == 0);
        return (ptr - cv_mat_data_1920_1080_shm_ptr)/cv_mat_data_1920_1080_unit_length;
    }else if(ptr >= cv_mat_data_1280_720_shm_ptr && ptr < cv_mat_data_1280_720_shm_ptr + cv_mat_data_1280_720_capacity*cv_mat_data_1280_720_unit_length){
        assert((ptr - cv_mat_data_1280_720_shm_ptr)%cv_mat_data_1280_720_unit_length == 0);
        return (ptr - cv_mat_data_1280_720_shm_ptr)/cv_mat_data_1280_720_unit_length;
    }else if(ptr >= cv_mat_data_960_540_shm_ptr && ptr < cv_mat_data_960_540_shm_ptr + cv_mat_data_960_540_capacity*cv_mat_data_960_540_unit_length){
        assert((ptr - cv_mat_data_960_540_shm_ptr)%cv_mat_data_960_540_unit_length == 0);
        return (ptr - cv_mat_data_960_540_shm_ptr)/cv_mat_data_960_540_unit_length;
    }else{
        assert(false);
    }
}

char* SharedMemory::get(uint8_t type, int pos){
    if(type == Data_Memory){
        return data_shm_ptr + data_unit_length*pos;
    }else if(type == Cv_Mat_Data_3840_2160_Memory){
        return cv_mat_data_3840_2160_shm_ptr + cv_mat_data_3840_2160_unit_length*pos;
    }else if(type == Cv_Mat_Data_1920_1080_Memory){
        return cv_mat_data_1920_1080_shm_ptr + cv_mat_data_1920_1080_unit_length*pos;
    }else if(type == Cv_Mat_Data_1280_720_Memory){
        return cv_mat_data_1280_720_shm_ptr + cv_mat_data_1280_720_unit_length*pos;
    }else if(type == Cv_Mat_Data_960_540_Memory){
        return cv_mat_data_960_540_shm_ptr + cv_mat_data_960_540_unit_length*pos;
    }else{
        assert(false);
    }
}

};