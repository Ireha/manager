# 任务管理系统 课程大作业
## 一、项目介绍
基于C++开发的任务日程管理程序，支持任务增删改查、时间提醒、优先级分类、本地数据存储，配套Git版本管理。

## 二、开发环境
- 操作系统：Ubuntu 64位 虚拟机
- 编程语言：C++
- 版本控制：Git + GitHub远程仓库
- 编译工具：CMake

## 三、项目目录结构
work/
├── CMakeLists.txt        # CMake 构建配置文件
├── README.md             # 项目说明文档
├── test.sh               # 功能测试 Shell 脚本
├── bin/                  # 编译生成的可执行文件目录 (可通过 .gitignore 忽略)
├── include/              # 头文件目录 (.h / .hpp)
│   ├── Task.h            # 任务结构与属性定义
│   ├── TaskManager.h     # 任务增删改查及文件 IO 管理
│   ├── User.h            # 用户认证与密码 HASH 加密
│   └── Reminder.h        # 后台多线程提醒逻辑
├── src/                  # 源文件目录 (.cpp)
│   ├── main.cpp          # 程序入口与 CLI 参数解析
│   ├── TaskManager.cpp   # TaskManager 实现
│   ├── User.cpp          # User 实现
│   └── Reminder.cpp      # Reminder 实现
└── data/                 # 存放数据文件的目录
    ├── users.txt         # 用户与密码 HASH 文件
    └── tasks.txt         # 任务持久化存储文件


项目开发进度 (Project Progress)

-  **架构设计**：完成项目目录结构划分与 CMake 构建配置。
-  **账户管理模块 (User Module)**：
  -  用户注册与登录逻辑实现。
  -  密码采用 `std::hash` 进行 HASH 加密保存（禁止明文存盘）。
  -  用户数据从 `data/users.txt` 本地文件进行自动加载与追加持久化。
-  **任务管理模块 (TaskManager Module)**：（开发中）
	**任务属性**：支持任务 ID、名称、启动时间、优先级（高/中/低）、分类（学习/娱乐/生活）、提醒时间。
  -     **属性缺省值**：优先级默认为“中”，分类默认为“生活”。
  -     **唯一性约束**：保证每个任务分配唯一 ID，且任务启动时间互不相同、任务名称+启动时间组合唯一。
  -     **对齐显示与排序**：支持按启动时间升序排序并按列格式化对齐打印。
-  **后台多线程提醒模块 (Reminder Module)**
   **QT图形界面（待开发）
