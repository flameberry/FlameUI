#include "Button.h"
#include "../renderer/Renderer.h"

namespace FlameUI {
    Button::Button(const ButtonInfo& buttonInfo)
        : m_ButtonInfo(buttonInfo)
    {
    }

    void Button::OnDraw()
    {
        if (m_ButtonInfo.pressState == PressState::Hovered)
            Renderer::AddQuad(m_ButtonInfo.position, m_ButtonInfo.dimensions, Renderer::GetThemeInfo().buttonHoveredColor, FL_ELEMENT_TYPE_BUTTON_INDEX);
        else
            Renderer::AddQuad(m_ButtonInfo.position, m_ButtonInfo.dimensions, Renderer::GetThemeInfo().buttonColor, FL_ELEMENT_TYPE_BUTTON_INDEX);
    }

    void Button::UpdateMetrics(const glm::vec3& pos, const glm::vec2& dim)
    {
        m_ButtonInfo.UpdateMetrics(pos, dim);
    }

    Button::~Button()
    {
    }
}