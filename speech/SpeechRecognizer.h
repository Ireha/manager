#ifndef SPEECHRECOGNIZER_H
#define SPEECHRECOGNIZER_H

#include <string>
class TaskManager;

class SpeechRecognizer
{
public:
    // 兼容Qt界面调用的原函数名（录音+识别，对应原recordAndRecognize）
    std::string recordAndRecognize();
    // 语音文本解析成任务，对应原voiceToTask
    bool voiceToTask(TaskManager& taskMgr, const std::string& text);
private:
    // 底层识别逻辑（内部复用）
    std::string voiceRecognition();
    const std::string tempAudioPath = "/tmp/tmp_voice.wav";
};

#endif // SPEECHRECOGNIZER_H
