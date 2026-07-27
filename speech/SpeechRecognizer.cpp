#include "SpeechRecognizer.h"
#include "TaskManager.h"
#include "TaskParser.h"

#include <iostream>
#include <cstdio>
#include <sstream>
#include <algorithm>

// 对外接口1：录音识别，适配Qt调用
std::string SpeechRecognizer::recordAndRecognize()
{
    return voiceRecognition();
}

// 对外接口2：解析文本生成任务
bool SpeechRecognizer::voiceToTask(
    TaskManager& taskMgr,
    const std::string& text
)
{
    if(text.empty())
    {
        std::cerr 
        << "[语音任务解析失败] 无识别文本"
        << std::endl;

        return false;
    }


    // 调用自然语言解析器
    TaskParser parser;

    ParsedTask task = parser.parse(text);



    if(task.name.empty())
    {
        std::cerr
        << "[语音任务解析失败] 无法解析任务名称"
        << std::endl;

        return false;
    }



    std::cout
    << "===== NLP解析结果 ====="
    << std::endl;

    std::cout
    << "任务:"
    << task.name
    << std::endl;


    std::cout
    << "开始:"
    << task.startTime
    << std::endl;


    std::cout
    << "提醒:"
    << task.remindTime
    << std::endl;


    std::cout
    << "优先级:"
    << task.priority
    << std::endl;


    std::cout
    << "分类:"
    << task.category
    << std::endl;



    bool ret = taskMgr.addTask(
        task.name,
        task.startTime,
        task.remindTime,
        task.priority,
        task.category
    );



    if(ret)
    {
        std::cout
        << "[语音任务创建成功]"
        << std::endl;
    }
    else
    {
        std::cerr
        << "[语音任务创建失败]"
        << std::endl;
    }



    return ret;
}
// 底层录音+识别核心逻辑（方案1英文识别，无中文模型依赖）
std::string SpeechRecognizer::voiceRecognition()
{
    // 标准16kHz 16bit单声道录音命令，消除8bit低音质警告
    std::string recordCmd = "arecord -f S16_LE -r 16000 -c 1 -d 10 " + tempAudioPath;
    std::cout << "[语音录入] 3秒后开始录音，时长10秒..." << std::endl;

    // 执行录音
    int ret = std::system(recordCmd.c_str());
    if(ret != 0)
    {
        std::cerr << "[语音错误] 麦克风录音失败，请检查麦克风设备" << std::endl;
        return "";
    }

    // 移除中文模型-hmm参数，使用系统默认英文识别模型
    std::string recogCmd =
    	"~/manager/whisper.cpp/build/bin/whisper-cli "
    	"-m ~/manager/whisper.cpp/models/ggml-small.bin "
    	"-f " + tempAudioPath +
    	" -l zh";
    FILE* pipe = popen(recogCmd.c_str(), "r");
    if (!pipe)
    {
        std::cerr << "[语音错误] 语音识别工具启动失败" << std::endl;
        return "";
    }

    // 读取识别输出文本
    std::string resultText;
char buffer[1024];

while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
{
    std::string line(buffer);

    // 只保留识别结果行
    if(line.find("-->") != std::string::npos)
    {
        size_t pos = line.find("]");
        if(pos != std::string::npos)
        {
            resultText += line.substr(pos + 1);
        }
    }
}

    pclose(pipe);

    if(resultText.empty())
    {
        std::cout << "[识别完成] 文本：" << std::endl;
        std::cout << "[语音解析失败] 无识别内容，请朗读清晰英文短句" << std::endl;
    }
    else
    {
        std::cout << "[识别完成] 文本：" << resultText << std::endl;
    }

    return resultText;
}
