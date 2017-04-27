cmake_minimum_required(VERSION 3.6)
project(MultimediaProject1)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp Image.cpp Image.h ImageProcess.cpp ImageProcess.h)
add_executable(MultimediaProject1 ${SOURCE_FILES})