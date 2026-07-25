#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include "Task.h"
#include <vector>
#include <string>
#include <mutex>

class TaskManager {
private:
    std::vector<Task> tasks;      // 内存中的任务列表
    std::string filePath;         // 数据文件保存路径
    int nextId;                   // 下一个自动分配的任务 ID
    mutable std::mutex taskMutex; // 多线程读写安全锁

    // 内部私有辅助方法
    bool isStartTimeUnique(const std::string& startTime) const;
    bool isNameAndStartTimeUnique(const std::string& name, const std::string& startTime) const;

public:
    explicit TaskManager(const std::string& filename = "data/tasks.txt");

    // 从本地文件加载任务
    bool loadFromFile();

    // 保存任务到本地文件（自动保存）
    bool saveToFile() const;

    // 添加任务（具备默认值、自动保存与唯一性校验）
    bool addTask(const std::string& name, 
                 const std::string& startTime, 
                 const std::string& remindTime, 
                 std::string priority = "中", 
                 std::string category = "生活");

    // 根据 ID 删除任务
    bool deleteTask(int id);

    // 显示所有任务（按启动时间排序，按列对齐输出）
    void showTasks() const;

    // 获取任务列表副本（供 Reminder 模块后台检查使用）
    std::vector<Task> getTasks() const;

    // 标记任务已提醒并保存状态
    void markAsReminded(int id);
};

#endif // TASKMANAGER_H
