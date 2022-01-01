#pragma once
#include <string>
#include <glm/glm.hpp>
#include <unordered_map>

namespace flame {
    struct character
    {
        uint32_t   textureId;  // ID handle of the glyph texture
        glm::vec2  size;       // Size of glyph
        glm::vec2  bearing;    // Offset from baseline to left/top of glyph
        double     advance;    // Offset to advance to next glyph
    };
}

namespace FlameUI {
    class TextHelper
    {
    public:
        TextHelper(const std::string& text, std::unordered_map<char, flame::character>& characters, float fontScale);
        ~TextHelper() = default;

        float          GetTextWidth()                   const { return m_Dimensions.x; }
        float          GetTextHeight()                  const { return m_Dimensions.y; }
    private:
        std::string m_Text;
        glm::vec2 m_Dimensions;
    };
}