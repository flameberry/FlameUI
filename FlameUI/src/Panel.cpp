#include "Panel.h"
#include "Renderer.h"

namespace FlameUI {
    Panel::Panel(
        const fuiVec2<int>& position_in_pixels,
        const fuiVec2<uint32_t>& dimensions_in_pixels,
        const fuiVec4<float>& color,
        const std::string& textureFilePath
    )
    {
        Renderer::AddQuad(&m_PanelQuadId, position_in_pixels, dimensions_in_pixels, color, textureFilePath);
    }

    void Panel::AddButton(
        uint32_t* quadId,
        const fuiVec2<int>& position_in_pixels,
        const fuiVec2<uint32_t>& dimensions_in_pixels,
        const fuiVec4<float>& color,
        const std::string& textureFilePath
    )
    {
        uint32_t quad_id;
        m_Buttons.push_back(Button::Create(&quad_id, position_in_pixels, dimensions_in_pixels, color, textureFilePath));
        *quadId = quad_id;
    }

    std::shared_ptr<Panel> Panel::Create(
        const fuiVec2<int>& position_in_pixels,
        const fuiVec2<uint32_t>& dimensions_in_pixels,
        const fuiVec4<float>& color,
        const std::string& textureFilePath
    )
    {
        return std::make_shared<Panel>(position_in_pixels, dimensions_in_pixels, color, textureFilePath);
    }
}