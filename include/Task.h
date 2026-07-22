#ifndef TASK_H
#define TASK_H

#include <string>

struct Task {
    int id;                 // 唯一ID
    std::string name;       // 任务名称
    std::string startTime;  // 启动时间 "YYYY-MM-DD HH:MM"
    std::string priority;   // 优先级：高/中/低（缺省：中）
    std::string category;   // 分类：学习/娱乐/生活（缺省：生活）
    std::string remindTime; // 提醒时间 "YYYY-MM-DD HH:MM"
    bool reminded = false;  // 是否已提醒
};

#endif // TASK_H
