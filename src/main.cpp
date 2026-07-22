#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include "User.h"
#include "TaskManager.h"
#include "Reminder.h"

// 打印命令行帮助说明
void printHelp() {
    std::cout << "========================================================================\n"
              << "              MySchedule 日程管理软件 - 使用说明                        \n"
              << "========================================================================\n"
              << "用法示例:\n"
              << "  1. 交互 Shell 运行模式（含后台提醒线程）:\n"
              << "     $ ./myschedule run\n\n"
              << "  2. 命令行单次指令模式:\n"
              << "     $ ./myschedule <username> <password> <command> [args...]\n\n"
              << "支持的指令说明:\n"
              << "  register <username> <password> \n"
              << "      注册新用户\n\n"
              << "  addtask <名称> <启动时间> <提醒时间> [优先级] [分类]\n"
              << "      添加任务。时间格式建议: \"YYYY-MM-DD HH:MM\"\n"
              << "      例如: addtask \"C++作业\" \"2026-08-01 10:00\" \"2026-08-01 09:50\" 高 学习\n\n"
              << "  showtask\n"
              << "      按启动时间对齐显示所有任务\n\n"
              << "  deltask <ID>\n"
              << "      根据指定的任务 ID 删除任务\n"
              << "========================================================================\n";
}

int main(int argc, char* argv[]) {
    // 数据文件路径配置
    const std::string userDbPath = "data/users.txt";
    const std::string taskDbPath = "data/tasks.txt";

    // 实例化用户管理与任务管理对象
    User userManager(userDbPath);
    TaskManager taskManager(taskDbPath);

    // 参数不足时显示帮助
    if (argc < 2) {
        printHelp();
        return 0;
    }

    std::string arg1 = argv[1];

    // =========================================================================
    // 模式 1: 交互式 Shell 循环模式 ($ ./myschedule run)
    // =========================================================================
    if (arg1 == "run") {
        std::cout << "====================================================\n";
        std::cout << "       欢迎使用 MySchedule 日程管理系统 (Shell 模式)\n";
        std::cout << "====================================================\n";

        // 用户登录逻辑
        std::string currentUser, currentPass;
        bool loggedIn = false;

        while (!loggedIn) {
            std::cout << "\n请选择操作 [1] 登录  [2] 注册  [0] 退出系统: ";
            std::string choice;
            if (!(std::cin >> choice)) return 0;

            if (choice == "1") {
                std::cout << "请输入用户名: ";
                std::cin >> currentUser;
                std::cout << "请输入口令: ";
                std::cin >> currentPass;

                if (userManager.authenticate(currentUser, currentPass)) {
                    std::cout << "[成功] 登录成功！欢迎回来，" << currentUser << "。\n";
                    loggedIn = true;
                } else {
                    std::cout << "[错误] 用户名或口令错误，请重试！\n";
                }
            } else if (choice == "2") {
                std::string newUser, newPass;
                std::cout << "请输入新用户名: ";
                std::cin >> newUser;
                std::cout << "请输入新口令: ";
                std::cin >> newPass;
                userManager.registerUser(newUser, newPass);
            } else if (choice == "0" || choice == "exit") {
                std::cout << "感谢使用，程序已退出。\n";
                return 0;
            } else {
                std::cout << "无效选项，请重新选择。\n";
            }
        }

        // 启动多线程后台提醒机制
        Reminder reminderThread(taskManager);
        reminderThread.start();
        std::cout << "\n[提示] 后台周期性任务提醒线程已成功启动...\n";
        std::cout << "输入 \"help\" 查看命令指南，输入 \"exit\" 退出系统。\n\n";

        // 循环等待用户输入命令
        std::string command;
        while (true) {
            std::cout << currentUser << "@myschedule> ";
            if (!(std::cin >> command) || command == "exit") {
                break;
            }

            if (command == "showtask") {
                taskManager.showTasks();
            } else if (command == "addtask") {
                std::string name, start, remind, priority = "中", category = "生活";
                std::cout << "请输入任务名称: ";
                std::cin >> name;
                std::cout << "请输入启动时间 (如 2026-08-01 10:00): ";
                std::cin.ignore();
                std::getline(std::cin, start);
                std::cout << "请输入提醒时间 (如 2026-08-01 09:50): ";
                std::getline(std::cin, remind);
                std::cout << "请输入优先级 (高/中/低，默认: 中): ";
                std::string priInput;
                std::getline(std::cin, priInput);
                if (!priInput.empty()) priority = priInput;

                std::cout << "请输入分类 (学习/娱乐/生活，默认: 生活): ";
                std::string catInput;
                std::getline(std::cin, catInput);
                if (!catInput.empty()) category = catInput;

                taskManager.addTask(name, start, remind, priority, category);
            } else if (command == "deltask") {
                int id;
                std::cout << "请输入要删除的任务 ID: ";
                if (std::cin >> id) {
                    taskManager.deleteTask(id);
                } else {
                    std::cin.clear();
                    std::string dummy;
                    std::cin >> dummy;
                    std::cout << "[错误] 请输入有效的数字 ID！\n";
                }
            } else if (command == "help") {
                printHelp();
            } else {
                std::cout << "未知命令: \"" << command << "\"。输入 \"help\" 查看帮助。\n";
            }
        }

        // 停止后台提醒线程
        reminderThread.stop();
        std::cout << "系统已退出。\n";
        return 0;
    }

    // =========================================================================
    // 模式 2: 命令行直接传参运行模式
    // 示例: ./myschedule admin 123456 addtask "开会" "2026-08-01 10:00" "2026-08-01 09:50"
    // =========================================================================
    if (arg1 == "register" && argc >= 4) {
        userManager.registerUser(argv[2], argv[3]);
        return 0;
    }

    if (argc < 4) {
        printHelp();
        return 0;
    }

    std::string user = argv[1];
    std::string pass = argv[2];
    std::string action = argv[3];

    // 进行身份验证
    if (!userManager.authenticate(user, pass)) {
        std::cerr << "[错误] 用户身份认证失败！用户名或密码不正确。\n";
        return 1;
    }

    if (action == "showtask") {
        taskManager.showTasks();
    } else if (action == "deltask" && argc >= 5) {
        int id = std::stoi(argv[4]);
        taskManager.deleteTask(id);
    } else if (action == "addtask" && argc >= 6) {
        std::string name = argv[4];
        std::string start = argv[5];
        std::string remind = (argc >= 7) ? argv[6] : start;
        std::string priority = (argc >= 8) ? argv[7] : "中";
        std::string category = (argc >= 9) ? argv[8] : "生活";

        taskManager.addTask(name, start, remind, priority, category);
    } else {
        printHelp();
    }

    return 0;
}
