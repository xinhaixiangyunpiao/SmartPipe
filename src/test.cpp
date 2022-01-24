#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
extern "C" {
#include "ncx_slab.h"
}
using namespace std;

int main(){
    // 申请共享内存并接入,首地址shm_ptr
    size_t pool_size = 32*1024UL*1024UL*1024UL;
    key_t shm_id = shmget(IPC_PRIVATE, pool_size, IPC_CREAT | 0666);
    u_char* shm_ptr = (u_char*)shmat(shm_id, NULL, 0);

    // 内存池初始化相关
    ncx_slab_pool_t *sp = (ncx_slab_pool_t*)shm_ptr;
    sp->addr = shm_ptr;                                                                                                      
    sp->min_shift = 3;                                                                                                     
    sp->end = shm_ptr + pool_size;

    // 初始化
    ncx_slab_init(sp);

    // alloc
    char* p = (char*)ncx_slab_alloc(sp, 1*1024*1024*1024);

    // 查看内存使用
    ncx_slab_stat_t stat;
    ncx_slab_stat(sp, &stat);

    // free 
    ncx_slab_free(sp, p);

    // 共享内存断开连接并free
    shmdt(shm_ptr);
    shmid_ds sds;
    shmctl(shm_id, IPC_RMID, &sds);

    return 0;
}