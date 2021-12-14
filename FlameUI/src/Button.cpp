#include "Button.h"
#include "Renderer.h"

namespace FlameUI {
    Button::Button(
        const std::string& title, const QuadPosType& quadPosType, const glm::vec2& position_in_pixels,
        const glm::vec2& dimensions_in_pixels, const glm::vec4& color, const std::string& textureFilePath
    )
        : m_InnerPadding(5.0f)
    {
        switch (quadPosType)
        {
        case FL_QUAD_POS_CENTER:
            m_PositionCenter = position_in_pixels;
            m_PositionBLVertex = position_in_pixels - (dimensions_in_pixels / 2.0f);
            break;
        case FL_QUAD_POS_BOTTOM_LEFT_VERTEX:
            m_PositionCenter = position_in_pixels + (dimensions_in_pixels / 2.0f);
            m_PositionBLVertex = position_in_pixels;
            break;
        default:
            FL_ERROR("Invalid Quad Position Type!");
            break;
        }

        if (textureFilePath != "")
            Renderer::AddQuad(&m_ButtonQuadId, quadPosType, position_in_pixels, dimensions_in_pixels, color, textureFilePath);
        else
            Renderer::AddQuad(&m_ButtonQuadId, quadPosType, position_in_pixels, dimensions_in_pixels, color);

        // Renderer::AddText(title, { position_in_pixels.x + ((int)dimensions_in_pixels.x / 2), position_in_pixels.y + ((int)dimensions_in_pixels.y / 2) }, 0.5f, { 1.0f, 1.0f, 1.0f, 1.0f });
    }

    Button::Button(const std::string& title, const glm::vec2& panel_position, const QuadPosType& quadPosType,
        const glm::vec2& button_pos_in_pixels, const glm::vec2& dimensions_in_pixels, const glm::vec4& color,
        const std::string& textureFilePath
    ) : m_InnerPadding(5.0f)
    {
        switch (quadPosType)
        {
        case FL_QUAD_POS_CENTER:
            m_PositionCenter = button_pos_in_pixels;
            m_PositionBLVertex = button_pos_in_pixels - (dimensions_in_pixels / 2.0f);
            m_PositionCenterRelativeToPanel = button_pos_in_pixels - panel_position;
            break;
        case FL_QUAD_POS_BOTTOM_LEFT_VERTEX:
            m_PositionCenter = button_pos_in_pixels + (dimensions_in_pixels / 2.0f);
            m_PositionBLVertex = button_pos_in_pixels;
            m_PositionCenterRelativeToPanel = button_pos_in_pixels + (dimensions_in_pixels / 2.0f) - panel_position;
            break;
        default:
            FL_ERROR("Invalid Quad Position Type!");
            break;
        }

        if (textureFilePath != "")
            Renderer::AddQuad(&m_ButtonQuadId, quadPosType, button_pos_in_pixels, dimensions_in_pixels, color, textureFilePath);
        else
            Renderer::AddQuad(&m_ButtonQuadId, quadPosType, button_pos_in_pixels, dimensions_in_pixels, color);

        // Renderer::AddText(title, { button_pos_in_pixels.x + ((int)dimensions_in_pixels.x / 2), button_pos_in_pixels.y + ((int)dimensions_in_pixels.y / 2) }, 0.5f, glm::vec4(1.0f));
    }

    glm::vec2 Button::GetPosition(const QuadPosType& quadPosType) const
    {
        switch (quadPosType)
        {
        case FL_QUAD_POS_CENTER:
            return m_PositionCenter;
        case FL_QUAD_POS_BOTTOM_LEFT_VERTEX:
            return m_PositionBLVertex;
        default:
            FL_ERROR("Invalid Quad Position Type!");
            return {};
        }
    }

    void Button::OnUpdate(const glm::vec2& panel_position_in_pixels, float z_index)
    {
        Renderer::SetQuadPosition(&m_ButtonQuadId, panel_position_in_pixels + m_PositionCenterRelativeToPanel);
        Renderer::SetQuadZIndex(&m_ButtonQuadId, z_index + 0.001f);
    }

    std::shared_ptr<Button> Button::Create(
        const std::string& title, const QuadPosType& quadPosType, const glm::vec2& position_in_pixels,
        const glm::vec2& dimensions_in_pixels, const glm::vec4& color, const std::string& textureFilePath
    )
    {
        return std::make_shared<Button>(title, quadPosType, position_in_pixels, dimensions_in_pixels, color, textureFilePath);
    }

    std::shared_ptr<Button> Button::Create(
        const std::string& title, const glm::vec2& panel_position, const QuadPosType& quadPosType,
        const glm::vec2& button_pos_in_pixels, const glm::vec2& dimensions_in_pixels, const glm::vec4& color,
        const std::string& textureFilePath
    )
    {
        return std::make_shared<Button>(title, panel_position, quadPosType, button_pos_in_pixels, dimensions_in_pixels, color, textureFilePath);
    }
}
