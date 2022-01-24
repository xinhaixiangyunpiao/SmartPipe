#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <iostream>
#include <Python.h>
#include <numpy/arrayobject.h> 

using namespace std;

int* init_numpy(){//初始化 numpy 执行环境，主要是导入包，python2.7用void返回类型，python3.0以上用int返回类型
    import_array();
}

int main(){
    Py_Initialize();
    init_numpy();
    unsigned char* ptr = new unsigned char[1920*1080*3];
    for(int i = 0; i < 1920*1080*3; i++){
        ptr[i] = 128;
    }
    npy_intp Dims[3] = {1080,1920,3};
    PyObject* pArray = PyArray_SimpleNewFromData(3, Dims, NPY_UBYTE, (void*)(ptr));
    Py_Finalize();
    return 0;
}