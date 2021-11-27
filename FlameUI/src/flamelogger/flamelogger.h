#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <ctime>
#include <iomanip>

#ifdef FL_XCODE_PROJ

#define FL_COLOR_DEFAULT ""
#define FL_COLOR_RED ""
#define FL_COLOR_GREEN ""
#define FL_COLOR_YELLOW ""
#define FL_COLOR_PURPLE ""
#define FL_COLOR_CYAN ""
#define FL_COLOR_WHITE ""

#define FL_COLOR_PURPLE_BOLD ""

#else

#define FL_COLOR_DEFAULT "\e[0m"
#define FL_COLOR_RED "\e[0;31m"
#define FL_COLOR_GREEN "\e[0;32m"
#define FL_COLOR_YELLOW "\e[0;33m"
#define FL_COLOR_PURPLE "\e[0;35m"
#define FL_COLOR_CYAN "\e[0;36m"
#define FL_COLOR_WHITE "\e[0;37m"

#define FL_COLOR_PURPLE_BOLD "\e[1;35m"

#endif

namespace flamelogger {
    /// Gets the prefix of the log message
    std::string get_prefix();
    /// Project Name should be set at the beginning of the program,
    /// which will be used as a prefix to all the log messages during runtime
    void Init(const std::string& proj_name);

    static std::vector<std::string> convert_params_to_string()
    {
        return {};
    }

    template<typename T, typename... Args>
    static std::vector<std::string> convert_params_to_string(const T& message, const Args&... args)
    {
        std::vector<std::string> string_set;

        std::stringstream ss;
        ss << message;

        string_set.push_back(ss.str());

        auto arguments = convert_params_to_string(args...);
        for (auto i : arguments)
            string_set.push_back(i);

        return string_set;
    }

    template<typename T, typename... Args>
    static std::string format_string(const T& message, const Args&... args)
    {
        std::vector<std::string> param_list = convert_params_to_string(message, args...);

        std::string temp_string = "";
        bool is_in_brackets = false;

        std::vector<std::string> indexes_in_string_format;

        bool is_index_valid = false;

        for (auto character : param_list[0])
        {
            if (character == '}' && is_in_brackets)
            {
                is_in_brackets = false;
                if (temp_string != "")
                {
                    indexes_in_string_format.push_back(temp_string);
                    temp_string = "";
                    is_index_valid = false;
                }
            }

            if (is_in_brackets)
            {
                if (((int)character >= 48) && ((int)character <= 57) && is_index_valid)
                {
                    temp_string.push_back(character);
                }
                else
                {
                    temp_string = "";
                    is_index_valid = false;
                }
            }

            if (character == '{')
            {
                is_in_brackets = true;
                is_index_valid = true;
            }
        }

        std::string msg = param_list[0];

        for (uint32_t i = 0; i < indexes_in_string_format.size(); i++)
        {
            msg.replace(
                msg.find("{" + indexes_in_string_format[i] + "}"),
                2 + indexes_in_string_format[i].length(),
                param_list[std::stoi(indexes_in_string_format[i]) + 1]
            );
        }
        return msg;
    }

    /// Logs the message in CYAN color in the terminal
    template<typename T, typename... Args>
    static void log(const T& message, const Args&... args)
    {
        std::string output_message = format_string(message, args...);
        std::cout << FL_COLOR_CYAN << get_prefix() << output_message << FL_COLOR_DEFAULT << std::endl;
    }

    /// Logs the message in GREEN color in the terminal
    template<typename T, typename... Args>
    static void info(const T& message, const Args&... args)
    {
        std::string output_message = format_string(message, args...);
        std::cout << FL_COLOR_GREEN << get_prefix() << output_message << FL_COLOR_DEFAULT << std::endl;
    }

    /// Logs the message in YELLOW color in the terminal
    template<typename T, typename... Args>
    static void warn(const T& message, const Args&... args)
    {
        std::string output_message = format_string(message, args...);
        std::cout << FL_COLOR_YELLOW << get_prefix() << output_message << FL_COLOR_DEFAULT << std::endl;
    }

    /// Logs the message in RED color in the terminal
    template<typename T, typename... Args>
    static void error(const T& message, const Args&... args)
    {
        std::string output_message = format_string(message, args...);
        std::cout << FL_COLOR_RED << get_prefix() << output_message << FL_COLOR_DEFAULT << std::endl;
    }
}
