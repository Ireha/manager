#include "TaskParser.h"

#include <ctime>
#include <iomanip>
#include <sstream>
#include <regex>
#include <iostream>

int chineseNumberToInt(const std::string& s)
{
    if(s=="一") return 1;
    if(s=="二") return 2;
    if(s=="三") return 3;
    if(s=="四") return 4;
    if(s=="五") return 5;
    if(s=="六") return 6;
    if(s=="七") return 7;
    if(s=="八") return 8;
    if(s=="九") return 9;
    if(s=="十") return 10;

    return std::stoi(s);
}
// 主解析函数
ParsedTask TaskParser::parse(const std::string& text)
{
    ParsedTask result;


    // 默认值
    result.priority = "中";
    result.category = "生活";


    /*
        解析任务名称
    */
    result.name = parseTaskName(text);


    /*
        解析时间
    */
    result.startTime = parseTime(text);


    /*
        默认提醒提前10分钟
    */
    result.remindTime = result.startTime;


    if(!result.startTime.empty())
    {
        std::tm tmTime = {};

        std::stringstream ss(result.startTime);

        ss >> std::get_time(
            &tmTime,
            "%Y-%m-%d %H:%M"
        );


        tmTime.tm_min -= 10;

        mktime(&tmTime);


        char buffer[64];

        strftime(
            buffer,
            sizeof(buffer),
            "%Y-%m-%d %H:%M",
            &tmTime
        );


        result.remindTime = buffer;
    }



    /*
        优先级解析
    */

    if(text.find("高") != std::string::npos)
    {
        result.priority = "高";
    }
    else if(text.find("低") != std::string::npos)
    {
        result.priority = "低";
    }



    /*
        分类解析
    */

    if(text.find("学习") != std::string::npos ||
       text.find("作业") != std::string::npos)
    {
        result.category = "学习";
    }
    else if(text.find("会议") != std::string::npos ||
            text.find("工作") != std::string::npos)
    {
        result.category = "工作";
    }
    else if(text.find("运动") != std::string::npos)
    {
        result.category = "健康";
    }


    return result;
}





// -------------------------
// 时间解析
// -------------------------

std::string TaskParser::parseTime(
    const std::string& text
)
{

    time_t now = time(nullptr);

    tm target = *localtime(&now);



    /*
        日期解析
    */


    if(text.find("明天") != std::string::npos)
    {
        target.tm_mday += 1;
    }


    else if(text.find("后天") != std::string::npos)
    {
        target.tm_mday += 2;
    }



    mktime(&target);



    /*
        时间解析
    */

    int hour = 9;


    // 下午X点
    std::regex afternoon(
        "下午([0-9]+)点"
    );


    std::smatch match;


   if(std::regex_search(text, match, afternoon))
{
    hour = chineseNumberToInt(match[1]);

    if(hour < 12)
        hour += 12;
}


    else
    {

        // 上午X点
        std::regex morning(
            "上午([0-9]+)点"
        );


        if(std::regex_search(text, match, morning))
        {
            hour = chineseNumberToInt(match[1]);
        }


        else
        {

            // 普通数字点
            std::regex normal(
                "([0-9]+)点"
            );


            if(std::regex_search(text, match, normal))
            {
                hour = chineseNumberToInt(match[1]);
            }

        }
    }



    target.tm_hour = hour;

    target.tm_min = 0;

    target.tm_sec = 0;



    char buffer[64];


    strftime(
        buffer,
        sizeof(buffer),
        "%Y-%m-%d %H:%M",
        &target
    );


    return buffer;
}





// -------------------------
// 任务名称解析
// -------------------------

std::string TaskParser::parseTaskName(
    const std::string& text
)
{

    std::string name = text;


    /*
        删除时间描述
    */


    std::regex timePattern(
    "(今天|明天|后天)?(上午|下午|晚上)?([0-9]+|一|二|三|四|五|六|七|八|九|十)点"
);


    name = std::regex_replace(
        name,
        timePattern,
        ""
    );



    /*
        删除提醒关键词
    */


    std::string keywords[] =
    {
        "提醒我",
        "提醒",
        "帮我",
        "安排",
        "设置一个"
    };


    for(auto &k : keywords)
    {
        size_t pos;

        while(
            (pos=name.find(k))
            != std::string::npos
        )
        {
            name.erase(
                pos,
                k.length()
            );
        }
    }



    /*
        删除标点
    */

    while(!name.empty())
    {

        char c=name.back();

        if(c=='.'||
           c==','||
           c==' '||
           c=='\n')
        {
            name.pop_back();
        }

        else
        {
            break;
        }

    }



    return name;
}
