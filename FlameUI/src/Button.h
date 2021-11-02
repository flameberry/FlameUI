#pragma once
#include <string>
#include "opengl_math_types.h"

namespace FlameUI {
    struct QuadProps
    {
        fuiVec2<int> position_in_pixels;
        fuiVec2<uint32_t> dimensions_in_pixels;
        fuiVec4<float> color;
        std::string texture_filepath;

        QuadProps() = default;
        QuadProps(
            const fuiVec2<int>& position_in_pixels,
            const fuiVec2<uint32_t>& dimensions_in_pixels,
            const fuiVec4<float>& color,
            const std::string& texture_filepath
        )
            : position_in_pixels(position_in_pixels), dimensions_in_pixels(dimensions_in_pixels), color(color), texture_filepath(texture_filepath)
        {
        }
    };

    class Button
    {
    public:
        Button(
            uint32_t* quadId,
            const fuiVec2<int>& position_in_pixels,
            const fuiVec2<uint32_t>& dimensions_in_pixels,
            const fuiVec4<float>& color,
            const std::string& textureFilePath
        );
        ~Button() = default;

        static std::shared_ptr<Button> Create(
            uint32_t* quadId,
            const fuiVec2<int>& position_in_pixels,
            const fuiVec2<uint32_t>& dimensions_in_pixels,
            const fuiVec4<float>& color,
            const std::string& textureFilePath
        );

        inline uint32_t GetQuadId() const { return m_QuadId; }
    private:
        uint32_t m_QuadId;
    };
}