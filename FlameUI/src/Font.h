#pragma once
#include <unordered_map>
#include <glm/glm.hpp>
#include <string>

namespace FlameUI {
    struct Font
    {
        struct Character
        {
            uint32_t TextureId;  // ID handle of the glyph texture
            glm::ivec2 Size;       // Size of glyph
            glm::ivec2 Bearing;    // Offset from baseline to left/top of glyph
            double Advance;    // Offset to advance to next glyph
        };
        std::unordered_map<char, Character> Characters;
        float PixelRange = 8.0f;
        float Strength = 0.5f; // Ranges from 0.0f to 1.0f
        std::string FilePath = "";
    };
}