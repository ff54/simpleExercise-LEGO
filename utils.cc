#include <iostream>
#include <stdarg.h>
#include "utils.hh"

namespace {
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_GRAY "\x1b[37m"
#define ANSI_COLOR_HIGRAY "\x1b[90m"
#define ANSI_COLOR_HIRED "\x1b[91m"
#define ANSI_COLOR_HIGREEN "\x1b[92m"
#define ANSI_COLOR_HIYELLOW "\x1b[93m"
#define ANSI_COLOR_HIBLUE "\x1b[94m"
#define ANSI_COLOR_HIPINK "\x1b[95m"
#define ANSI_COLOR_HICYAN "\x1b[96m"
#define ANSI_COLOR_WHITE "\x1b[97m"
#define ANSI_COLOR_RESET "\x1b[0m"
const unsigned LOG_LEVEL_COUNT = 5;
const char* prefix[LOG_LEVEL_COUNT] = {"ERROR ", "WARNING ", "DEBUG ", "INFO ", "ALL"};
const char* colorList[LOG_LEVEL_COUNT] = {ANSI_COLOR_RED,  ANSI_COLOR_YELLOW, ANSI_COLOR_GREEN, ANSI_COLOR_CYAN, ANSI_COLOR_HIBLUE};

static enum utils::LOG_LEVEL logLevel_g = utils::INFO;
}
namespace utils
{
using namespace std;
Event::Event(const std::string& name, EventType type): name(name),evtType(type), timeStamp(chrono::system_clock::now()) {}

Event::Event(Event& rhs):name(rhs.name), evtType(rhs.evtType), timeStamp(rhs.timeStamp){ }

Event::Event(Event&& other): name(move(other.name)), evtType(other.evtType), timeStamp(move(other.timeStamp)) { }


void Log(LOG_LEVEL level, const char* format, ...)
{
    va_list arg;

    if ((level > logLevel_g) || (level < ERROR) || (level > INFO)) {
        return;
    }

    va_start(arg, format);
    printf(":%s%s%s", colorList[level], prefix[level], ANSI_COLOR_RESET);
    vprintf(format, arg);
    printf("\n");
    va_end(arg);
}

void setLogLevel(LOG_LEVEL level) {
    logLevel_g = level;
}
    
} // namespace utils
