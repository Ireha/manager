#include "TaskManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>

TaskManager::TaskManager(const std::string& filename) : filePath(filename), nextId(1) {
    loadFromFile();
}

// 检查启动时间是否唯一（每个任务的开始时间不能相同）
bool TaskManager::isStartTimeUnique(const std::string& startTime) const {
    for (const auto& task : tasks) {
        if (task.startTime == startTime) {
            return false;
        }
    }
    return true;
}

// 检查任务名称 + 启动时间的组合是否唯一
bool TaskManager::isNameAndStartTimeUnique(const std::string& name, const std::string& startTime) const {
    for (const auto& task : tasks) {
        if (task.name == name && task.startTime == startTime) {
            return false;
        }
    }
    return true;
}

// 添加任务
bool TaskManager::addTask(const std::string& name, 
                        const std::string& startTime, 
                        const std::string& remindTime, 
                        std::string priority, 
                        std::string category) {
    std::lock_guard<std::mutex> lock(taskMutex);

    // 校验属性缺省值
    if (priority.empty()) priority = "中";
    if (category.empty()) category = "生活";

    // 1. 校验开始时间唯一性
    if (!isStartTimeUnique(startTime)) {
        std::cerr << "[错误] 添加失败：已存在启动时间为 \"" << startTime << "\" 的任务！启动时间不能相同。\n";
        return false;
    }

    // 2. 校验 任务名称+开始时间 组合唯一性
    if (!isNameAndStartTimeUnique(name, startTime)) {
        std::cerr << "[错误] 添加失败：已存在相同名称与启动时间的任务！\n";
        return false;
    }

    // 创建并加入内存
    Task newTask{nextId++, name, startTime, priority, category, remindTime, false};
    tasks.push_back(newTask);

    std::cout << "[成功] 成功添加任务！分配 ID: " << newTask.id << "\n";

    // 输入完成后自动保存到文件
    saveToFile();
    return true;
}

// 删除任务
bool TaskManager::deleteTask(int id) {
    std::lock_guard<std::mutex> lock(taskMutex);

    auto it = std::remove_if(tasks.begin(), tasks.end(), [id](const Task& t) {
        return t.id == id;
    });

    if (it != tasks.end()) {
        tasks.erase(it, tasks.end());
        std::cout << "[成功] 已成功删除 ID 为 " << id << " 的任务。\n";
        saveToFile(); // 自动更新文件
        return true;
    }

    std::cerr << "[错误] 未找到 ID 为 " << id << " 的任务！\n";
    return false;
}

// 显示任务（按启动时间排序，并按列对齐）
void TaskManager::showTasks() const {
    std::lock_guard<std::mutex> lock(taskMutex);

    if (tasks.empty()) {
        std::cout << "\n当前暂无任何日程任务。\n\n";
        return;
    }

    // 复制一份数据按启动时间排序
    std::vector<Task> sortedTasks = tasks;
    std::sort(sortedTasks.begin(), sortedTasks.end(), [](const Task& a, const Task& b) {
        return a.startTime < b.startTime;
    });

    // 格式化表头输出
    std::cout << "\n========================================================================================\n";
    std::cout << std::left 
              << std::setw(6)  << "ID"
              << std::setw(20) << "任务名称"
              << std::setw(20) << "启动时间"
              << std::setw(10) << "优先级"
              << std::setw(10) << "分类"
              << std::setw(20) << "提醒时间"
              << std::setw(10) << "状态" << "\n";
    std::cout << "----------------------------------------------------------------------------------------\n";

    for (const auto& t : sortedTasks) {
        std::cout << std::left 
                  << std::setw(6)  << t.id
                  << std::setw(20) << t.name
                  << std::setw(20) << t.startTime
                  << std::setw(10) << t.priority
                  << std::setw(10) << t.category
                  << std::setw(20) << t.remindTime
                  << std::setw(10) << (t.reminded ? "已提醒" : "待提醒") << "\n";
    }
    std::cout << "========================================================================================\n\n";
}

// 保存任务到文件
bool TaskManager::saveToFile() const {
    std::ofstream file(filePath, std::ios::trunc); // 覆盖写入
    if (!file.is_open()) {
        std::cerr << "[错误] 无法打开文件进行写入: " << filePath << "\n";
        return false;
    }

    for (const auto& t : tasks) {
        file << t.id << ","
             << t.name << ","
             << t.startTime << ","
             << t.priority << ","
             << t.category << ","
             << t.remindTime << ","
             << (t.reminded ? "1" : "0") << "\n";
    }

    file.close();
    return true;
}

// 从文件加载任务
bool TaskManager::loadFromFile() {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return false;
    }

    std::lock_guard<std::mutex> lock(taskMutex);
    tasks.clear();

    std::string line;
    int maxId = 0;

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string idStr, name, startTime, priority, category, remindTime, remindedStr;

        if (std::getline(ss, idStr, ',') &&
            std::getline(ss, name, ',') &&
            std::getline(ss, startTime, ',') &&
            std::getline(ss, priority, ',') &&
            std::getline(ss, category, ',') &&
            std::getline(ss, remindTime, ',') &&
            std::getline(ss, remindedStr, ',')) {

            Task t;
            t.id = std::stoi(idStr);
            t.name = name;
            t.startTime = startTime;
            t.priority = priority;
            t.category = category;
            t.remindTime = remindTime;
            t.reminded = (remindedStr == "1");

            tasks.push_back(t);
            if (t.id > maxId) {
                maxId = t.id;
            }
        }
    }

    nextId = maxId + 1; // 确保新 ID 不重复
    file.close();
    return true;
}

// 获取任务列表副本
std::vector<Task> TaskManager::getTasks() const {
    std::lock_guard<std::mutex> lock(taskMutex);
    return tasks;
}

// 标记任务已提醒
void TaskManager::markAsReminded(int id) {
    std::lock_guard<std::mutex> lock(taskMutex);
    for (auto& t : tasks) {
        if (t.id == id) {
            t.reminded = true;
            break;
        }
    }
    saveToFile();
}
