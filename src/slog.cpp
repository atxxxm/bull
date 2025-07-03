#include "../include/slog.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <stdio.h>
#include <stdarg.h>
#include <ctime>

void slog::LOG::print(COLOR color, const char* text, const char* str, va_list args) noexcept 
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

void slog::LOG::print(COLOR color, const char* text, std::string str) 
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

void slog::LOG::print_NO_STAPLES(COLOR color, const char* text)
{
    printf("\033[1;%dm%s\033[0m\n", static_cast<int>(color), text);
}

void slog::LOG::print_NO_STAPLES_WITHOUT_N(COLOR color, const char* text)
{
    printf("\033[1;%dm%s\033[0m", static_cast<int>(color), text);
}

void slog::LOG::DEBUG_NE(const char* str, ...) noexcept 
{
    va_list args;
    va_start(args, str);
    print(COLOR::BLUE, "DEBUG", str, args);
    va_end(args);
}

void slog::LOG::DEBUG(std::string str) 
{
    print(COLOR::BLUE, "DEBUG", str);
}

void slog::LOG::INFO_NE(const char* str, ...) noexcept 
{
    va_list args;
    va_start(args, str);
    print(COLOR::LIGHT_BLUE, "INFO", str, args);
    va_end(args);
}

void slog::LOG::INFO(std::string str) 
{
    print(COLOR::LIGHT_BLUE, "INFO", str);
}

void slog::LOG::WARNING_NE(const char* str, ...) noexcept 
{
    va_list args;
    va_start(args, str);
    print(COLOR::ORANGE, "WARNING", str, args);
    va_end(args);
}

void slog::LOG::WARNING(std::string str) 
{
    print(COLOR::ORANGE, "WARNING", str);
}

void slog::LOG::ERROR_NE(const char* str, ...) noexcept 
{
    va_list args;
    va_start(args, str);
    print(COLOR::RED, "ERROR", str, args);
    va_end(args);
}

void slog::LOG::ERROR(std::string str) 
{
    print(COLOR::RED, "ERROR", str);
}

void slog::LOG::FATAL_NE(const char* str, ...) noexcept 
{
    va_list args;
    va_start(args, str);
    print(COLOR::DARK_RED, "FATAL", str, args);
    va_end(args);
}

void slog::LOG::FATAL(std::string str) 
{
    print(COLOR::DARK_RED, "FATAL", str);
}

void slog::LOG::CUSTOM(std::string color, const char* text, std::string str)
{
    if (color == "red")
    {
        print(COLOR::RED, text, str);
    }
    else if (color == "dark_red")
    {
        print(COLOR::DARK_RED, text, str);
    }
    else if (color == "green")
    {
        print(COLOR::GREEN, text, str);
    }
    else if (color == "orange")
    {
        print(COLOR::ORANGE, text, str);
    }
    else if (color == "blue")
    {
        print(COLOR::BLUE, text, str);
    }
    else if (color == "purple")
    {
        print(COLOR::DARK_RED, text, str);
    }
    else if (color == "light_blue")
    {
        print(COLOR::LIGHT_BLUE, text, str);
    }
   
}

void slog::LOG::CUSTOM_NSL(std::string color, std::string text)
{
    if (color == "red")
    {
        print_NO_STAPLES(COLOR::RED, text.c_str());
    }
    else if (color == "dark_red")
    {
        print_NO_STAPLES(COLOR::DARK_RED, text.c_str());
    }
    else if (color == "green")
    {
        print_NO_STAPLES(COLOR::GREEN, text.c_str());
    }
    else if (color == "orange")
    {
        print_NO_STAPLES(COLOR::ORANGE, text.c_str());
    }
    else if (color == "blue")
    {
        print_NO_STAPLES(COLOR::BLUE, text.c_str());
    }
    else if (color == "purple")
    {
        print_NO_STAPLES(COLOR::PURPLE, text.c_str());
    }
    else if (color == "light_blue")
    {
        print_NO_STAPLES(COLOR::LIGHT_BLUE, text.c_str());
    }
}

void slog::LOG::CUSTOM_NSL_WITHOUT_N(std::string color, std::string text)
{
    if (color == "red")
    {
        print_NO_STAPLES_WITHOUT_N(COLOR::RED, text.c_str());
    }
    else if (color == "dark_red")
    {
        print_NO_STAPLES_WITHOUT_N(COLOR::DARK_RED, text.c_str());
    }
    else if (color == "green")
    {
        print_NO_STAPLES_WITHOUT_N(COLOR::GREEN, text.c_str());
    }
    else if (color == "orange")
    {
        print_NO_STAPLES_WITHOUT_N(COLOR::ORANGE, text.c_str());
    }
    else if (color == "blue")
    {
        print_NO_STAPLES_WITHOUT_N(COLOR::BLUE, text.c_str());
    }
    else if (color == "purple")
    {
        print_NO_STAPLES_WITHOUT_N(COLOR::PURPLE, text.c_str());
    }
    else if (color == "light_blue")
    {
        print_NO_STAPLES_WITHOUT_N(COLOR::LIGHT_BLUE, text.c_str());
    }
}

bool slog::LOG::new_open(std::string filename) 
{
    file.open(filename);
    return file.is_open();
}

bool slog::LOG::open(std::string filename) 
{
    file.open(filename, std::ios::app);
    return file.is_open();
}

bool slog::LOG::auto_open() 
{
    std::time_t now = std::time(nullptr);
    std::tm* local_time = std::localtime(&now);

    int year = local_time->tm_year + 1900;
    int month = local_time->tm_mon + 1;
    int day = local_time->tm_mday;

    std::string filename = "slog::LOGging_" + std::to_string(day) + "_" + std::to_string(month) + "_" + std::to_string(year) + ".slog::LOG";
    file.open(filename, std::ios::app);
    return file.is_open();
}

void slog::LOG::close() 
{
    file.close();
}
