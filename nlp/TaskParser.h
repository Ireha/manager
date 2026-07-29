#ifndef TASKPARSER_H
#define TASKPARSER_H

#include <string>


struct ParsedTask
{
    std::string name;

    std::string startTime;

    std::string remindTime;

    std::string priority;

    std::string category;
};


class TaskParser
{
public:

    ParsedTask parse(
        const std::string& text
    );


private:

    std::string parseTime(
        const std::string& text
    );


    std::string parseTaskName(
        const std::string& text
    );

};


#endif
