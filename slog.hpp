#ifndef SLOG_HPP
#define SLOG_HPP

#include <string>
#include <fstream>

class LOG 
{
protected:
    std::ofstream file;

    enum class COLOR 
    {
        RED = 91,
        DARK_RED = 31,
        GREEN = 92,
        ORANGE = 93,
        BLUE = 94,
        PURPLE = 95,
        LIGHT_BLUE = 96
    };

private:
    void print(COLOR color, const char* text, const char* str, va_list args) noexcept;

    void print(COLOR color, const char* text, std::string str);

public:
    void DEBUG_NE(const char* str, ...) noexcept;

    void DEBUG(std::string str);

    void INFO_NE(const char* str, ...) noexcept;

    void INFO(std::string str);

    void WARNING_NE(const char* str, ...) noexcept;

    void WARNING(std::string str);

    void ERROR_NE(const char* str, ...) noexcept;

    void ERROR(std::string str);

    void FATAL_NE(const char* str, ...) noexcept;

    void FATAL(std::string str);

    bool new_open(std::string filename);

    bool open(std::string filename);

    bool auto_open();

    void close();
};

#endif