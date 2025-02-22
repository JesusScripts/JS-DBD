#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <ctime>

namespace logger {

    static void log(const std::string& message) {
        std::ofstream logFile("log.txt", std::ios_base::app);
        if (logFile.is_open()) {
            auto now = std::chrono::system_clock::now();
            std::time_t now_time = std::chrono::system_clock::to_time_t(now);
            logFile << std::ctime(&now_time) << ": " << message << std::endl;
            logFile.close();
        }
    }

}

#endif // LOGGER_H