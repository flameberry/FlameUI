#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

namespace fby_logger {
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

    template<typename T, typename... Args>
    static void log(const T& message, const Args&... args)
    {
        std::string output_message = format_string(message, args...);
        std::cout << output_message << std::endl;
    }
}
