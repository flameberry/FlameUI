#include "Button.h"
#include "Renderer.h"

namespace FlameUI {
    Button::Button(
        uint32_t* quadId,
        const fuiVec2<int>& position_in_pixels,
        const fuiVec2<uint32_t>& dimensions_in_pixels,
        const fuiVec4<float>& color,
        const std::string& textureFilePath
    )
    {
        Renderer::AddQuad(&m_QuadId, position_in_pixels, dimensions_in_pixels, color, textureFilePath);
        *quadId = m_QuadId;
    }

    std::shared_ptr<Button> Button::Create(
        uint32_t* quadId,
        const fuiVec2<int>& position_in_pixels,
        const fuiVec2<uint32_t>& dimensions_in_pixels,
        const fuiVec4<float>& color,
        const std::string& textureFilePath
    )
    {
        return std::make_shared<Button>(quadId, position_in_pixels, dimensions_in_pixels, color, textureFilePath);
    }
}