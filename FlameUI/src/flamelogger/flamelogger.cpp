#include "flamelogger.h"

namespace flamelogger {
    static std::string project_name = "";
    static bool is_proj_name_as_prefix = false;

    void Init(const std::string& proj_name)
    {
        if (proj_name != "")
        {
            project_name = proj_name;
            is_proj_name_as_prefix = true;
        }
    }

    std::string get_prefix()
    {
        std::stringstream prefix("");
        std::time_t now = std::time(0);
        std::tm* currentTime = std::localtime(&now);
        prefix << "[" << std::setfill('0') << std::setw(2) << std::to_string(currentTime->tm_hour) << ":" << std::setfill('0') << std::setw(2) << std::to_string(currentTime->tm_min) << ":" << std::setfill('0') << std::setw(2) << std::to_string(currentTime->tm_sec) << "] ";

        if (is_proj_name_as_prefix)
            prefix << project_name << ": ";
        return prefix.str();
    }
}