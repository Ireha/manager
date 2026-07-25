#include "User.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <functional>

User::User(const std::string& filePath) : userFilePath(filePath) {
    loadUsers();
}

// 使用 std::hash 计算密码 HASH 值，确保密码不以明文形式保存
std::string User::hashPassword(const std::string& rawPassword) const {
    std::size_t hashValue = std::hash<std::string>{}(rawPassword);
    return std::to_string(hashValue);
}

// 从本地文件加载用户数据
bool User::loadUsers() {
    std::ifstream file(userFilePath);
    if (!file.is_open()) {
        // 如果文件不存在，属于正常初始化情况，后续注册时会自动创建
        return false;
    }

    userDb.clear();
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string username, hashedPassword;
        
        // 假设数据存为 CSV 格式: username,hashedPassword
        if (std::getline(ss, username, ',') && std::getline(ss, hashedPassword, ',')) {
            userDb[username] = hashedPassword;
        }
    }
    file.close();
    return true;
}

// 检查用户是否存在
bool User::userExists(const std::string& username) const {
    return userDb.find(username) != userDb.end();
}

// 注册新用户（自动保存 HASH 密码到文件）
bool User::registerUser(const std::string& username, const std::string& password) {
    if (username.empty() || password.empty()) {
        std::cerr << "[错误] 用户名或密码不能为空！\n";
        return false;
    }

    if (userExists(username)) {
        std::cerr << "[错误] 用户 \"" << username << "\" 已存在！\n";
        return false;
    }

    std::string hashedPassword = hashPassword(password);
    userDb[username] = hashedPassword;

    // 追加写回文件 (用 HASH 值存盘)
    std::ofstream file(userFilePath, std::ios::app);
    if (!file.is_open()) {
        std::cerr << "[错误] 无法保存数据到文件: " << userFilePath << "\n";
        return false;
    }

    file << username << "," << hashedPassword << "\n";
    file.close();

    std::cout << "[成功] 用户 \"" << username << "\" 注册成功！\n";
    return true;
}

// 用户认证
bool User::authenticate(const std::string& username, const std::string& password) const {
    auto it = userDb.find(username);
    if (it == userDb.end()) {
        return false; // 用户不存在
    }

    // 对比输入的密码 HASH 与文件里存的 HASH
    return it->second == hashPassword(password);
}
