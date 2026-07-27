cmake_minimum_required(VERSION 3.10)
project(MySchedule CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 头文件检索目录（基础模块+语音模块，解决SpeechRecognizer.h找不到问题）
include_directories(
    include
    speech
    nlp
)

# ---------------------- 依赖库查找 ----------------------
# 多线程
find_package(Threads REQUIRED)
# OpenSSL 密码哈希加密
find_package(OpenSSL REQUIRED)
# Qt5 图形界面（加分项，可选检测）
find_package(Qt5 OPTIONAL_COMPONENTS Core Gui Widgets)
# ALSA音频库、离线语音识别库
find_library(ASOUND_LIB asound)
find_library(SPHINXBASE_LIB sphinxbase)
find_library(POCKETSPHINX_LIB pocketsphinx)

# ---------------------- 源码文件分类（核心：分离两套main入口，解决冲突） ----------------------
# 1. 基础业务逻辑（不含src/main.cpp，纯底层功能）
file(GLOB SRC_BASE_LOGIC
    "src/Reminder.cpp"
    "src/User.cpp"
    "src/TaskManager.cpp"
)
# 2. 命令行专属入口（唯一命令行main）
set(SRC_CLI_MAIN "src/main.cpp")
# 3. 语音识别加分模块源码
file(GLOB SRC_SPEECH "speech/*.cpp")
# 3.5 自然语言解析模块
file(GLOB SRC_NLP "nlp/*.cpp")
# 4. Qt界面加分模块源码
file(GLOB SRC_QT "qt_gui/*.cpp")
file(GLOB QT_UI_FILES "qt_gui/*.ui")
file(GLOB QT_HEADER_FILES "qt_gui/*.h")

# 输出二进制程序到 bin 文件夹
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# ---------------------- 可执行1：纯命令行程序 myschedule ----------------------
add_executable(myschedule
    ${SRC_BASE_LOGIC}
    ${SRC_CLI_MAIN}
    ${SRC_SPEECH}
    ${SRC_NLP}
)
# 链接全部依赖（线程、加密、音频、语音识别）
target_link_libraries(myschedule PRIVATE Threads::Threads)
target_link_libraries(myschedule PRIVATE OpenSSL::Crypto)
target_link_libraries(myschedule PRIVATE ${ASOUND_LIB} ${SPHINXBASE_LIB} ${POCKETSPHINX_LIB})

# ---------------------- 可执行2：Qt图形界面加分程序 myschedule_gui ----------------------
# 仅当系统安装Qt5时才编译GUI程序，不破坏基础命令行编译
if(Qt5_FOUND)
    # Qt UI、MOC 自动编译
    qt5_wrap_cpp(QT_MOC_SRCS ${QT_HEADER_FILES})
    qt5_wrap_ui(QT_UI_SRCS ${QT_UI_FILES})

    # GUI程序核心：只加载业务逻辑+Qt代码，**不加载src/main.cpp**，彻底解决main多重定义冲突
    add_executable(myschedule_gui
    ${SRC_BASE_LOGIC}
    ${SRC_SPEECH}
    ${SRC_NLP}
    ${SRC_QT}
    ${QT_MOC_SRCS}
    ${QT_UI_SRCS}
)

    # GUI链接全部依赖 + Qt图形库
    target_link_libraries(myschedule_gui PRIVATE Threads::Threads)
    target_link_libraries(myschedule_gui PRIVATE OpenSSL::Crypto)
    target_link_libraries(myschedule_gui PRIVATE ${ASOUND_LIB} ${SPHINXBASE_LIB} ${POCKETSPHINX_LIB})
    target_link_libraries(myschedule_gui PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets)
endif()
