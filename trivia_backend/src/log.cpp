#include "log.h"

std::string getClassName(const char *mangledName) {

    int status;
    char *className = abi::__cxa_demangle(mangledName, nullptr, nullptr, &status);
    if (status == 0) {
        std::string result(className);
        free(className);
        return result;
    }
    return mangledName;
}

tm getLocalTime()
{
    const auto now = std::chrono::system_clock::now();
    const std::time_t localTime = std::chrono::system_clock::to_time_t(now);

    return  *std::localtime(&localTime);
}