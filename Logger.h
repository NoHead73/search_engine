#pragma once
#include <string>
#include <fstream>
#include <mutex>

class Logger {
private:
    std::ofstream log_file;
    std::mutex log_mutex;
    
public:
    Logger(const std::string& filename = "search_engine.log") {
        log_file.open(filename, std::ios::app);
    }
    
    ~Logger() {
        if (log_file.is_open()) {
            log_file.close();
        }
    }
    
    void log(const std::string& message) {
        std::lock_guard<std::mutex> lock(log_mutex);
        if (log_file.is_open()) {
            log_file << "[" << getCurrentTime() << "] " << message << std::endl;
        }
    }
    
private:
    std::string getCurrentTime() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};