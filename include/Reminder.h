#ifndef REMINDER_H
#define REMINDER_H

#include "TaskManager.h"
#include <atomic>
#include <thread>

class Reminder {
private:
    TaskManager& manager;
    std::atomic<bool> isRunning;
    std::thread workerThread;

    void run();

public:
    Reminder(TaskManager& tm);
    ~Reminder();

    void start();
    void stop();
};

#endif // REMINDER_H
