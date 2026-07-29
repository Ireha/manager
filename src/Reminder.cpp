#include "Reminder.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
// ========== 改动1：新增cstdlib用于system调用 ==========
#include <cstdlib>

// ========== 新增AudioPlayer实现，完全独立，不干扰原有逻辑 ==========
AudioPlayer::AudioPlayer()
{
    audioPath = "./audio/reminder.mp3";
}

void AudioPlayer::playRemindAudio()
{
    // Linux调用mpg123后台播放，屏蔽输出日志
    std::string playCmd = "mpg123 " + audioPath + " > /dev/null 2>&1";
    int ret = std::system(playCmd.c_str());
    if (ret != 0)
    {
        std::cout << "\n[音频警告] 播放失败，请执行：sudo apt install mpg123" << std::endl;
    }
}

// ========== 以下全部是你原生代码，无任何删除、修改、重写 ==========
Reminder::Reminder(TaskManager& tm) : taskManager(tm), running(false) {}

Reminder::~Reminder() {
    stop();
}

// 获取系统当前时间格式化字符串
std::string Reminder::getCurrentTimeString() const {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm tm_buf;

#ifdef _WIN32
    localtime_s(&tm_buf, &now_time);
#else
    localtime_r(&now_time, &tm_buf);
#endif

    std::stringstream ss;
    ss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M");
    return ss.str();
}

// 后台线程的主循环：每隔 5 秒轮询检查一次任务列表
void Reminder::run() {
    while (running) {

        // 每次检查前重新读取任务文件
        taskManager.loadFromFile();

        std::string currentTime = getCurrentTimeString();
        auto tasks = taskManager.getTasks();

        for (const auto& task : tasks) {
            // 如果任务未提醒且提醒时间小于等于当前时间，触发屏幕打印
            if (!task.reminded && !task.remindTime.empty() && task.remindTime <= currentTime) {
                std::cout << "\n\n========================================\n"
                          << "⏰ [任务到期提醒] 蜂鸣响铃!\n"
                          << "   任务 ID   : " << task.id << "\n"
                          << "   任务名称  : " << task.name << "\n"
                          << "   启动时间  : " << task.startTime << "\n"
                          << "   提醒时间  : " << task.remindTime << "\n"
                          << "========================================\n> " 
                          << std::flush;

                // ========== 改动2：仅新增这一行，播放提醒音频 ==========
                audioPlayer.playRemindAudio();

                // 标记该任务为已提醒，防止重复打印
                taskManager.markAsReminded(task.id);
            }
        }

        // 线程休眠 5 秒
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

// 启动后台独立线程
void Reminder::start() {
    if (!running) {
        running = true;
        workerThread = std::thread(&Reminder::run, this);
    }
}

// 安全停止后台线程
void Reminder::stop() {
    if (running) {
        running = false;
        if (workerThread.joinable()) {
            workerThread.join(); // 等待后台线程安全结束
        }
    }
}
