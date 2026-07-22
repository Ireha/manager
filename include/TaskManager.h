#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include "Task.h"
#include <vector>
#include <string>
#include <mutex>

class TaskManager {
private:
    std::vector<Task> tasks;
    std::string filePath;
    int nextId;
    std::mutex taskMutex; // 多线程安全锁

public:
    TaskManager(const std::string& filename);

    bool loadFromFile();
    bool saveToFile(); // 每次变更自动保存

    bool addTask(const std::string& name, const std::string& startTime, 
                 const std::string& remindTime, std::string priority = "中", 
                 std::string category = "生活");

    bool deleteTask(int id);
    void showTasks() const; // 按开始时间排序并按列对齐输出

    // 供多线程提醒调用的接口
    void checkAndTriggerReminders();
};

#endif // TASKMANAGER_H
