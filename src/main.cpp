#include <iostream>
#include <string>
#include "TaskManager.h"
#include "Reminder.h"
#include "User.h"

void printHelp() {
    std::cout << "Usage: myschedule [user password] <command> [args...]\n\n"
              << "Commands:\n"
              << "  run                                    进入交互 Shell 模式并开启后台提醒\n"
              << "  addtask <name> <start> <remind> [pri] [cat]  添加任务\n"
              << "  showtask                               显示当前任务列表\n"
              << "  deltask <id>                           删除指定任务\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printHelp();
        return 0;
    }

    TaskManager taskManager("data/tasks.txt");
    std::string command = argv[1];

    if (command == "run") {
        Reminder reminder(taskManager);
        reminder.start();

        std::cout << "=== 日程管理 Shell 模式启动 (输入 exit 退出) ===\n";
        std::string input;
        while (true) {
            std::cout << "> ";
            if (!(std::cin >> input) || input == "exit") break;
            
            if (input == "showtask") {
                taskManager.showTasks();
            } else if (input == "help") {
                printHelp();
            }
            // 可补充交互式的其他指令...
        }

        reminder.stop();
    } else {
        // 解析: myschedule user pass addtask ... 模式
        if (argc >= 4) {
            std::string user = argv[1];
            std::string pass = argv[2];
            std::string action = argv[3];

            if (!User::authenticate(user, pass)) {
                std::cerr << "用户认证失败！\n";
                return 1;
            }

            if (action == "showtask") {
                taskManager.showTasks();
            } else if (action == "deltask" && argc >= 5) {
                taskManager.deleteTask(std::stoi(argv[4]));
            }
        } else {
            printHelp();
        }
    }

    return 0;
}
