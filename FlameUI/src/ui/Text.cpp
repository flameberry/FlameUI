#include "Text.h"

namespace FlameUI {
    TextHelper::TextHelper(const std::string& text, std::unordered_map<char, flame::character>& characters, float fontScale)
        : m_Text(text), m_Dimensions(0.0f)
    {
        for (std::string::const_iterator it = text.begin(); it != text.end(); it++)
        {
            if (it != text.end() - 1)
                m_Dimensions.x += characters[*it].advance * fontScale + 1.0f;
            m_Dimensions.x += characters[*it].size.x * fontScale;
        }
    }
}