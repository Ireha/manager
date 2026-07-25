cmake_minimum_required(VERSION 3.10)
project(MySchedule CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 包含头文件目录
include_directories(include)

# 查找线程库（用于多线程后台提醒）
find_package(Threads REQUIRED)

# 收集源文件
file(GLOB SOURCES "src/*.cpp")

# 生成可执行文件到 bin 目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
add_executable(myschedule ${SOURCES})

# 链接线程库
target_link_libraries(myschedule PRIVATE Threads::Threads)
