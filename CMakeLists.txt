cmake_minimum_required(VERSION 3.5)

project(main)

find_package(OpenCV REQUIRED PATHS /home/lx/ NO_DEFAULT_PATH)

find_package(unofficial-concurrentqueue CONFIG REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3 -pthread")

set(CMAKE_EXE_LINKER_FLAGS "-lpthread")

include_directories(./include/
                    ./include/core/
                    ./include/tools/
                    /home/lx/.conda/envs/smartpipe/include/python3.8/
                    /data/lx/.local/lib/python3.8/site-packages/numpy/core/include/
                    ./third_party/shmqueue/
                    /home/lx/vcpkg/packages/concurrentqueue_x64-linux/include/concurrentqueue/
                    ./third_party/ncx_mempool/
                    ${OpenCV_INCLUDE_DIRS})

add_executable(main 
              ./src/main.cpp 
              ./src/core/functions.cpp 
              ./src/core/executor.cpp 
              ./src/core/profile.cpp 
              ./src/core/util.cpp 
              ./src/core/data.cpp 
              ./src/core/models.cpp 
              ./src/core/memory.cpp 
              ./src/core/app.cpp 
              ./src/tools/mdReader.cpp)

link_directories(${OpenCV_LIBRARY_DIRS} /home/lx/SmartPipe/third_party/ncx_mempool/)

target_link_libraries(main 
                      PRIVATE 
                      ${OpenCV_LIBS} 
                      /home/lx/.conda/envs/smartpipe/lib/libpython3.8.so
                      /home/lx/SmartPipe/third_party/ncx_mempool/libncx_slab.a
                      unofficial::concurrentqueue::concurrentqueue) 


