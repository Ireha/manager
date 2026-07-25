#ifndef REMINDER_H
#define REMINDER_H

#include "TaskManager.h"
#include <atomic>
#include <thread>

class Reminder {
private:
    TaskManager& taskManager;  // 引用任务管理器
    std::atomic<bool> running; // 标记后台线程运行状态
    std::thread workerThread;  // 后台工作线程

    // 后台轮询检查的核心逻辑
    void run();

    // 获取当前时间格式化字符串 (YYYY-MM-DD HH:MM)
    std::string getCurrentTimeString() const;

public:
    explicit Reminder(TaskManager& tm);
    ~Reminder();

    // 启动后台线程
    void start();

    // 停止后台线程
    void stop();
};

#endif // REMINDER_H
