#include "FlameUI.h"
#include <glad/glad.h>
#include "Renderer.h"

namespace FlameUI {
    std::vector<std::shared_ptr<Panel>> _FlameUI::s_Panels;

    void _FlameUI::Init()
    {
        Renderer::Init();
    }

    void _FlameUI::_Panel(const QuadProps& quadProps, uint32_t* panelQuadId)
    {
        s_Panels.push_back(Panel::Create(quadProps.position_in_pixels, quadProps.dimensions_in_pixels, quadProps.color, quadProps.texture_filepath));
        if (panelQuadId)
            *panelQuadId = s_Panels.back()->GetPanelQuadId();
    }

    void _FlameUI::_Button(const QuadProps& quadProps, uint32_t* buttonQuadId)
    {
        uint32_t quad_id;
        s_Panels.back()->AddButton(&quad_id, quadProps.position_in_pixels, quadProps.dimensions_in_pixels, quadProps.color, quadProps.texture_filepath);
        if (buttonQuadId)
            *buttonQuadId = quad_id;
    }

    void _FlameUI::OnDraw()
    {
        Renderer::OnDraw();
    }

    void _FlameUI::End()
    {
        Renderer::CleanUp();
    }
}
