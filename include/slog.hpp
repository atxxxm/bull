#pragma once

#include <string>
#include <fstream>

namespace slog
{
    class LOG 
    {
    protected:
        std::ofstream file;

    private:
        enum COLOR 
        {
            RED = 91,
            DARK_RED = 31,
            GREEN = 92,
            ORANGE = 93,
            BLUE = 94,
            PURPLE = 95,
            LIGHT_BLUE = 96
        };

        void print(COLOR color, const char* text, const char* str, va_list args) noexcept;

        void print(COLOR color, const char* text, std::string str);

        void print_NO_STAPLES(COLOR color, const char* text);

        void print_NO_STAPLES_WITHOUT_N(COLOR color, const char* text);

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

        void CUSTOM(std::string color, const char* text, std::string str);

        void CUSTOM_NSL(std::string color, std::string text);

        void CUSTOM_NSL_WITHOUT_N(std::string color, std::string text);

        bool new_open(std::string filename);

        bool open(std::string filename);

        bool auto_open();

        void close();
    };

}