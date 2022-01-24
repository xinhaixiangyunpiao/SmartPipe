#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <cassert>
#include <string>

namespace sp{

// 结合共享内存用于进程间通信
template<class T>
class BlockingQueue{
private:
    std::string fifo_name;
    int pipe_fd;
    // 禁用拷贝
    BlockingQueue(const BlockingQueue&) = delete;
    BlockingQueue& operator= (const BlockingQueue&) = delete;
public:
    BlockingQueue(std::string fifo_name);
    ~BlockingQueue();
    std::string getName();      // 得到命名管道名称
    void open_named_pipe(char type);       // 打开命名管道
    void wait_dequeue(T& data); // 阻塞得到队列中的数据
    void enqueue(T data);       // 将数据发送到队列
};

template<class T>
BlockingQueue<T>::BlockingQueue(std::string fifo_name){
    this->fifo_name = fifo_name;
    // 创建命名管道
    int ret = mkfifo(fifo_name.c_str(), 0777);
    if(ret < 0){
        if(errno != EEXIST){
            std::cout << "named fifo created failed." << std::endl;
            std::cout << strerror(errno) << std::endl;
        }
    }
    // 初始化文件描述符
    pipe_fd = -1;
}

template<class T>
BlockingQueue<T>::~BlockingQueue(){
    if(pipe_fd != -1){
        close(pipe_fd);
    }
}

template<class T>
std::string BlockingQueue<T>::getName(){
    return fifo_name;
}

template<class T>
void BlockingQueue<T>::open_named_pipe(char type){
    if(type == 'w'){
        pipe_fd = open(fifo_name.c_str(), O_WRONLY);
    }else if(type == 'r'){
        pipe_fd = open(fifo_name.c_str(), O_RDONLY);
    }else{
        assert(false);
    }
    if(pipe_fd < 0){
        std::cout << "pipe open failed." << std::endl;
        assert(false);
    }
}

template<class T>
void BlockingQueue<T>::wait_dequeue(T& data){
    assert(pipe_fd != -1);
    if(read(pipe_fd, &data, sizeof(T)) < 0){
        std::cout << "read failed." << std::endl;
        std::cout << strerror(errno) << std::endl;
    }
}

template<class T>
void BlockingQueue<T>::enqueue(T data){
    assert(pipe_fd != -1);
    if(write(pipe_fd, &data, sizeof(T)) < 0){
        std::cout << "write failed." << std::endl;
        std::cout << strerror(errno) << std::endl;
    }
}

};

#endif