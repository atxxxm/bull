#ifndef SLOG_HPP
#define SLOG_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <stdio.h>
#include <stdarg.h>
#include <ctime>

class LOG 
{
protected:
    std::ofstream file;

    enum class COLOR {
        RED = 91,
        DARK_RED = 31,
        GREEN = 92,
        ORANGE = 93,
        BLUE = 94,
        PURPLE = 95,
        LIGHT_BLUE = 96
    };

private:
    void print(COLOR color, const char* text, const char* str, va_list args) noexcept 
    {
        char buff[1024] = {0};
        char file_buff[1024] = {0};
        snprintf(buff, sizeof(buff), "[\033[1;%dm%s\033[0m]: %s\n", static_cast<int>(color), text, str);
        vprintf(buff, args);

        snprintf(file_buff, sizeof(file_buff), "[%s]: %s\n", text, str);

        if (file.is_open()) {
            char formatted_buff[1024] = {0};
            vsnprintf(formatted_buff, sizeof(formatted_buff), file_buff, args);

            std::time_t now = std::time(nullptr);
            std::tm* local_time = std::localtime(&now);

            int hours = local_time->tm_hour;
            int minutes = local_time->tm_min;
            int seconds = local_time->tm_sec;
            file << "(" << hours << ":" << minutes << ":" << seconds << ")" << formatted_buff;
        }
    }
    void print(COLOR color, const char* text, std::string str) 
    {
        printf("[\033[1;%dm%s\033[0m]: ", static_cast<int>(color), text);
        std::cout << str << "\n";
    
        if (file.is_open()) {
            std::time_t now = std::time(nullptr);
            std::tm* local_time = std::localtime(&now);
    
            int hours = local_time->tm_hour;
            int minutes = local_time->tm_min;
            int seconds = local_time->tm_sec;
    
            file << "(" << hours << ":" << minutes << ":" << seconds << ")[" << text << "]: " << str << "\n";
        }
    }

public:
    void DEBUG_NE(const char* str, ...) noexcept 
    {
        va_list args;
        va_start(args, str);
        print(COLOR::BLUE, "DEBUG", str, args);
        va_end(args);
    }

    void DEBUG(std::string str) 
    {
        print(COLOR::BLUE, "DEBUG", str);
    }

    void INFO_NE(const char* str, ...) noexcept 
    {
        va_list args;
        va_start(args, str);
        print(COLOR::LIGHT_BLUE, "INFO", str, args);
        va_end(args);
    }

    void INFO(std::string str) 
    {
        print(COLOR::LIGHT_BLUE, "INFO", str);
    }

    void WARNING_NE(const char* str, ...) noexcept 
    {
        va_list args;
        va_start(args, str);
        print(COLOR::ORANGE, "WARNING", str, args);
        va_end(args);
    }

    void WARNING(std::string str) 
    {
        print(COLOR::ORANGE, "WARNING", str);
    }

    void ERROR_NE(const char* str, ...) noexcept 
    {
        va_list args;
        va_start(args, str);
        print(COLOR::RED, "ERROR", str, args);
        va_end(args);
    }

    void ERROR(std::string str) 
    {
        print(COLOR::RED, "ERROR", str);
    }

    void FATAL_NE(const char* str, ...) noexcept 
    {
        va_list args;
        va_start(args, str);
        print(COLOR::DARK_RED, "FATAL", str, args);
        va_end(args);
    }

    void FATAL(std::string str) 
    {
        print(COLOR::DARK_RED, "FATAL", str);
    }

    bool new_open(std::string filename) 
    {
        file.open(filename);
        return file.is_open();
    }

    bool open(std::string filename) 
    {
        file.open(filename, std::ios::app);
        return file.is_open();
    }

    bool auto_open() 
    {
        std::time_t now = std::time(nullptr);
        std::tm* local_time = std::localtime(&now);

        int year = local_time->tm_year + 1900;
        int month = local_time->tm_mon + 1;
        int day = local_time->tm_mday;

        std::string filename = "logging_" + std::to_string(day) + "_" + std::to_string(month) + "_" + std::to_string(year) + ".log";
        file.open(filename, std::ios::app);
        return file.is_open();
    }

    void close() {
        file.close();
    }
};

#endif