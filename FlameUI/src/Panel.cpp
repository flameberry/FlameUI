#include "Panel.h"
#include "Renderer.h"

namespace FlameUI {
    Panel::Panel(
        const glm::ivec2& position_in_pixels,
        const glm::ivec2& dimensions_in_pixels,
        const glm::vec4& color,
        const std::string& textureFilePath
    )
        : m_Position(position_in_pixels), m_Dimensions(dimensions_in_pixels), m_InnerPadding(15, 10)
    {
        Renderer::AddQuad(&m_PanelQuadId, FL_QUAD_POS_CENTER, position_in_pixels, dimensions_in_pixels, color, textureFilePath);
    }

    void Panel::AddButton(
        uint32_t* quadId,
        const std::string& title,
        const glm::ivec2& dimensions_in_pixels,
        const glm::vec4& color,
        const std::string& textureFilePath
    )
    {
        static int top_element_position = m_Position.y + ((int)m_Dimensions.y / 2);
        static int bottom_element_position = m_Position.y - ((int)m_Dimensions.y / 2);
        if (top_element_position - bottom_element_position - 2 * m_InnerPadding.y > 0)
        {
            glm::ivec2 dimensions = dimensions_in_pixels;

            uint32_t quad_id;
            bool is_too_big_X = dimensions_in_pixels.x + (2 * m_InnerPadding.x) > m_Dimensions.x;
            bool is_too_big_Y = dimensions_in_pixels.y > top_element_position - bottom_element_position - 2 * m_InnerPadding.y;

            if ((is_too_big_X) && (!is_too_big_Y))
                dimensions.x = m_Dimensions.x - 2 * m_InnerPadding.x;
            else if ((!is_too_big_X) && (is_too_big_Y))
                dimensions.y = top_element_position - bottom_element_position - 2 * m_InnerPadding.y;
            else if (is_too_big_X && is_too_big_Y)
            {
                dimensions.x = m_Dimensions.x - 2 * m_InnerPadding.x;
                dimensions.y = top_element_position - bottom_element_position - 2 * m_InnerPadding.y;
            }

            glm::ivec2 position = {
                m_Position.x - ((int)m_Dimensions.x / 2) + m_InnerPadding.x,
                top_element_position - m_InnerPadding.y - (int)dimensions.y
            };

            m_Buttons.push_back(Button::Create(&quad_id, title, FL_QUAD_POS_BOTTOM_LEFT_VERTEX, position, dimensions, color, textureFilePath));

            if (quadId)
                *quadId = quad_id;

            top_element_position = top_element_position - m_InnerPadding.y - dimensions.y;
        }
        else
        {
            FL_LOG("Panel is full!");
        }
    }

    std::shared_ptr<Panel> Panel::Create(
        const glm::ivec2& position_in_pixels,
        const glm::ivec2& dimensions_in_pixels,
        const glm::vec4& color,
        const std::string& textureFilePath
    )
    {
        return std::make_shared<Panel>(position_in_pixels, dimensions_in_pixels, color, textureFilePath);
    }
}
