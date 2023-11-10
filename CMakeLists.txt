cmake_minimum_required(VERSION 3.12)
project(OpencvSample)

find_package(OpenCV REQUIRED)

add_executable(test_exe main.cpp)

target_link_libraries(test_exe PRIVATE ${OpenCV_LIBS})
