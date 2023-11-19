#pragma once
#include<string>
#include <chrono>

namespace utils{

enum LOG_LEVEL
{
    ERROR = 0,
    WARNING,
    DEBUG,
    INFO,
    ALL
};

void Log(LOG_LEVEL level, const char* format, ...);
void setLogLevel(LOG_LEVEL level);

struct Event {
    enum EventType {
        DEVICEINFO,
        SENSORINFO,
    };
    Event(const std::string& name, EventType type);
    Event(Event& rhs);
    Event(Event&& other);
    virtual ~Event(){};
    std::string name;
    EventType evtType;
    std::chrono::system_clock::time_point timeStamp;
};

}// end of namespace utils