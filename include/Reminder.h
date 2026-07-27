#ifndef REMINDER_H
#define REMINDER_H

#include "TaskManager.h"
#include <atomic>
#include <thread>
#include <string>

// 全新独立音频播放器类，和Reminder解耦
class AudioPlayer
{
public:
    AudioPlayer();
    void playRemindAudio();
private:
    std::string audioPath;
};

// ========== 以下是你原来完整的Reminder类声明，一字未改 ==========
class Reminder {
private:
    TaskManager& taskManager;
    std::atomic<bool> running;
    std::thread workerThread;

    // 新增音频成员，仅此一处新增
    AudioPlayer audioPlayer;

    void run();
    std::string getCurrentTimeString() const;
public:
    explicit Reminder(TaskManager& tm);
    ~Reminder();

    void start();
    void stop();
};

#endif // REMINDER_H