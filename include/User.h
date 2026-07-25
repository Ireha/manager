#ifndef USER_H
#define USER_H

#include <string>
#include <unordered_map>

class User {
private:
    std::string userFilePath;
    // 内存中保存 用户名 -> 密码HASH 的映射
    std::unordered_map<std::string, std::string> userDb;

    // 简单 HASH 计算算法（也可以替换为 SHA-256）
    std::string hashPassword(const std::string& rawPassword) const;

public:
    explicit User(const std::string& filePath = "data/users.txt");

    // 从文件加载用户信息
    bool loadUsers();

    // 注册新用户（自动保存到文件）
    bool registerUser(const std::string& username, const std::string& password);

    // 验证用户身份
    bool authenticate(const std::string& username, const std::string& password) const;

    // 检查用户是否存在
    bool userExists(const std::string& username) const;
};

#endif // USER_H
