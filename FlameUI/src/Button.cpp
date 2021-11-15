#include "Button.h"
#include "Renderer.h"

namespace FlameUI {
    Button::Button(
        uint32_t* quadId,
        const std::string& title,
        const QuadPosType& quadPosType,
        const glm::ivec2& position_in_pixels,
        const glm::ivec2& dimensions_in_pixels,
        const glm::vec4& color,
        const std::string& textureFilePath
    )
        : m_Position(position_in_pixels), m_InnerPadding(5.0f)
    {
        Renderer::AddQuad(&m_QuadId, quadPosType, position_in_pixels, dimensions_in_pixels, color, textureFilePath);
        Renderer::AddText(title, { position_in_pixels.x + ((int)dimensions_in_pixels.x / 2), position_in_pixels.y + ((int)dimensions_in_pixels.y / 2) }, 0.5f, { 1.0f, 1.0f, 1.0f, 1.0f });
        if (quadId)
            *quadId = m_QuadId;
    }

    std::shared_ptr<Button> Button::Create(
        uint32_t* quadId,
        const std::string& title,
        const QuadPosType& quadPosType,
        const glm::ivec2& position_in_pixels,
        const glm::ivec2& dimensions_in_pixels,
        const glm::vec4& color,
        const std::string& textureFilePath
    )
    {
        return std::make_shared<Button>(quadId, title, quadPosType, position_in_pixels, dimensions_in_pixels, color, textureFilePath);
    }
}
