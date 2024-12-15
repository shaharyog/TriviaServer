#pragma once

#include <string>
#include <ctime>
#include <chrono>
#include <fmt/std.h>
#include <fmt/core.h>
#include <fmt/color.h>
#include <cxxabi.h> // For __cxa_demangle
#include <typeinfo> // For typeid
#include <mutex>
#include "utils/communicator/endpoint.h"
#include "constants.h"
#include <iostream>

constexpr auto timeFormat = "%A %e %B %I:%M:%S %p";
static std::mutex logMutex;

std::string getClassName(const char *mangledName);

tm getLocalTime();

template<typename T>
void log(const std::string &functionName, const std::string &message, bool success, const Endpoint &endpoint) {
    const auto localTime = getLocalTime();
    const std::string className = getClassName(typeid(T).name());
    std::lock_guard lock(logMutex);
    auto outputFile = std::ofstream(LOG_FILE_PATH, std::ios_base::app);
    if (success) {
        std::string infoSuccessHeader = fmt::format("[{:" + std::string(timeFormat) + "}] [{:^15.15}] [{:^55.55}]:",
                                                    localTime, "INFO_SUCCESS",
                                                    className + "::" + functionName);
        std::string infoSuccessMsg = fmt::format(" {:<70.70}\t\t{:<28}\n", message, endpoint.toString());

        fmt::print(fmt::emphasis::bold | fg(fmt::color::green_yellow),
                   infoSuccessHeader);
        fmt::print(fg(fmt::color::green_yellow), infoSuccessMsg);

        outputFile << (infoSuccessHeader + infoSuccessMsg);

    } else {
        std::string infoErrorHeader = fmt::format("[{:" + std::string(timeFormat) + "}] [{:^15.15}] [{:^55.55}]:",
                                                  localTime, "INFO_ERROR", className + "::" + functionName);

        std::string infoErrorMsg = fmt::format(" {:<70.70}\t\t{:<28}\n", message, endpoint.toString());

        fmt::print(fmt::emphasis::bold | fg(fmt::color::magenta),
                   infoErrorHeader);
        fmt::print(fg(fmt::color::magenta), infoErrorMsg);

        outputFile << (infoErrorHeader + infoErrorMsg);
    }
}


template<typename T>
inline void logError(const std::string &functionName, const std::string &message, const Endpoint &endpoint) {
    const auto localTime = getLocalTime();
    const std::string className = getClassName(typeid(T).name());
    std::lock_guard lock(logMutex);
    auto outputFile = std::ofstream(LOG_FILE_PATH, std::ios_base::app);
    std::string infoErrorHeader = fmt::format("[{:" + std::string(timeFormat) + "}] [{:^15.15}] [{:^55.55}]:",
                                              localTime, "INFO_ERROR", className + "::" + functionName);

    std::string infoErrorMsg = fmt::format(" {:<70.70}\t\t{:<28}\n", message, endpoint.toString());

    fmt::print(fmt::emphasis::bold | fg(fmt::color::magenta),
               infoErrorHeader);
    fmt::print(fg(fmt::color::magenta), infoErrorMsg);

    outputFile << (infoErrorHeader + infoErrorMsg);
}


inline void
logServerProgress(const std::string &className, const std::string &functionName, const std::string &message) {
    const auto localTime = getLocalTime();
    std::lock_guard lock(logMutex);
    auto outputFile = std::ofstream(LOG_FILE_PATH, std::ios_base::app);
    std::string infoSuccessHeader = fmt::format("[{:" + std::string(timeFormat) + "}] [{:^15.15}] [{:^55.55}]:",
                                                localTime, "SERVER_PROGRESS",
                                                className + "::" + functionName);

    std::string infoSuccessMsg = fmt::format(" {:<70.70}", message);

    fmt::print(fmt::emphasis::bold | fg(fmt::color::aqua),
               infoSuccessHeader);
    fmt::print(fg(fmt::color::aqua), infoSuccessMsg);

    outputFile << (infoSuccessHeader + infoSuccessMsg);
}


template<typename T>
inline void logServerProgress(const std::string &functionName, const std::string &message) {
    const std::string className = getClassName(typeid(T).name());
    logServerProgress(className, functionName, message);
}

inline void logServerResult(bool success, bool isExit = false, bool isSevere = true) {
    std::lock_guard lock(logMutex);
    auto outputFile = std::ofstream(LOG_FILE_PATH, std::ios_base::app);

    if (success) {
        std::string successHeader = fmt::format("\t\t{:<28}\n", "Success");

        fmt::print(fg(fmt::color::lime_green), successHeader);

        outputFile << successHeader;
    } else if (!isExit) {
        std::string failedHeader = fmt::format("\t\t{:<28}\n", "Failed");
        fmt::print(fg(isSevere ? fmt::color::red : fmt::color::orange), failedHeader);
        outputFile << failedHeader;
    } else {
        std::string failedHeader = fmt::format("\t\t{:<28}\n", "Failed, exiting...");
        fmt::print(fg(fmt::color::red), failedHeader);
        outputFile << failedHeader;
        std::abort();
    }
}