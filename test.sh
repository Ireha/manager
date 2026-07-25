#!/bin/bash

# ==============================================================================
# MySchedule 自动化功能测试脚本
# ==============================================================================

# 设置颜色输出
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo -e "${GREEN}====================================================${NC}"
echo -e "${GREEN}          开始 MySchedule 项目功能测试脚本          ${NC}"
echo -e "${GREEN}====================================================${NC}"

# 1. 自动编译项目
echo -e "\n[测试 1] 正在编译项目..."
mkdir -p build && cd build
cmake .. && make
cd ..

# 确定可执行文件路径
EXE=""
if [ -f "./bin/myschedule" ]; then
    EXE="./bin/myschedule"
elif [ -f "./build/bin/myschedule" ]; then
    EXE="./build/bin/myschedule"
else
    echo -e "${RED}[失败] 找不到可执行文件 myschedule，编译可能失败！${NC}"
    exit 1
fi

echo -e "${GREEN}[成功] 找到可执行程序: ${EXE}${NC}"

# 2. 初始化环境
mkdir -p data
rm -f data/users.txt data/tasks.txt

# 3. 测试用户注册与认证
echo -e "\n[测试 2] 测试用户注册与登录..."
$EXE register testuser 123456

# 4. 测试添加任务 (普通添加)
echo -e "\n[测试 3] 测试添加任务..."
$EXE testuser 123456 addtask "C++课程设计" "2026-08-01 10:00" "2026-08-01 09:50" "高" "学习"
$EXE testuser 123456 addtask "健身锻炼" "2026-08-01 18:00" "2026-08-01 17:30" "中" "生活"

# 5. 测试属性缺省值 (不传优先级和分类)
echo -e "\n[测试 4] 测试任务属性缺省值..."
$EXE testuser 123456 addtask "看电影" "2026-08-02 20:00" "2026-08-02 19:30"

# 6. 测试唯一性约束 (插入相同开始时间的任务，预期失败)
echo -e "\n[测试 5] 测试启动时间冲突校验 (预期应当报错提示冲突)..."
$EXE testuser 123456 addtask "冲突任务" "2026-08-01 10:00" "2026-08-01 09:00"

# 7. 测试显示所有任务 (检查排序与按列对齐)
echo -e "\n[测试 6] 显示所有任务 (验证排序与格式化)..."
$EXE testuser 123456 showtask

# 8. 测试删除任务
echo -e "\n[测试 7] 删除 ID 为 1 的任务..."
$EXE testuser 123456 deltask 1

# 9. 验证删除后的任务列表及持久化文件
echo -e "\n[测试 8] 删除后的任务列表:"
$EXE testuser 123456 showtask

echo -e "\n[测试 9] 检查本地落盘文件内容 (data/tasks.txt):"
cat data/tasks.txt

echo -e "\n${GREEN}====================================================${NC}"
echo -e "${GREEN}              自动化测试流程执行完毕！              ${NC}"
echo -e "${GREEN}====================================================${NC}"
