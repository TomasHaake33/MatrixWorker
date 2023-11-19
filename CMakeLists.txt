add_executable(MatrixWorker
    devbin/main.cpp
    src/MatrixWorkerImpl.cpp
    src/MatrixWorkerImpl.h
    include/MatrixWorker/MatrixWorkerLib.h)

target_include_directories(MatrixWorker PRIVATE include)