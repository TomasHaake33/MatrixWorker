project(MatrixWorker)
set_target_properties(MatrixWorker PROPERTIES CXX_STANDARD 14)

add_library(MatrixWorker
    src/MatrixWorkerImpl.cpp
    src/MatrixWorkerImpl.h
	include/MatrixWorker/MatrixWorkerLib.h)

target_include_directories(MatrixWorker PUBLIC include)

add_executable(MatrixWorkerBin 
    devbin/main.cpp)    
target_include_directories(MatrixWorkerBin PRIVATE include)
target_link_libraries(MatrixWorkerBin PRIVATE MatrixWorker)

#for my local build
#set(BOOST_ROOT "D:/dev/sdk/boost_1_68_0")
#set(BOOST_INCLUDE_DIRS "${BOOST_ROOT}/include")
#set(BOOST_LIBRARY_DIRS "${BOOST_ROOT}/lib")

find_package(Boost COMPONENTS unit_test_framework)
add_executable(MatrixWorkerTests tests/tests.cpp)
    
target_include_directories(MatrixWorkerTests PRIVATE include)
target_link_libraries(MatrixWorkerTests PRIVATE MatrixWorker Boost::unit_test_framework)