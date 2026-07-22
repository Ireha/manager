#ifndef USER_H
#define USER_H

#include <string>

class User {
public:
    // 计算密码的 HASH 值（例如 SHA-256 或 std::hash）
    static std::string hashPassword(const std::string& rawPassword);
    
    // 验证用户密码
    static bool authenticate(const std::string& username, const std::string& password);
};

#endif // USER_H
