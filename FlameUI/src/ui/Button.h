#pragma once
#include <string>
#include <glm/glm.hpp>
#include "../core/Core.h"

namespace FlameUI {
    enum class PressState { NotPressed = 0, Hovered, Pressed };

    struct ButtonInfo
    {
        std::string text;
        glm::vec3 position;
        glm::vec2 dimensions;
        Rect2D buttonRect2D;
        PressState pressState = PressState::NotPressed;

        ButtonInfo(const std::string& text, const glm::vec3& position, const glm::vec2& dimensions)
            : text(text), position(position), dimensions(dimensions), buttonRect2D(position.x - dimensions.x / 2.0f, position.x + dimensions.x / 2.0f, position.y - dimensions.y / 2.0f, position.y + dimensions.y / 2.0f)
        {
        }
        ButtonInfo() = default;
        ~ButtonInfo() = default;

        void UpdateMetrics(const glm::vec3& pos, const glm::vec2& dim)
        {
            position = pos;
            dimensions = dim;
            buttonRect2D = { position.x - dimensions.x / 2.0f, position.x + dimensions.x / 2.0f, position.y - dimensions.y / 2.0f, position.y + dimensions.y / 2.0f };
        }
    };

    class Button
    {
    public:
        Button(const ButtonInfo& buttonInfo = ButtonInfo());
        ~Button();

        void OnDraw();
        const ButtonInfo& GetButtonInfo() const { return m_ButtonInfo; }
        void UpdateMetrics(const glm::vec3& pos, const glm::vec2& dim);
        inline void SetPressState(const PressState& pressState) { m_ButtonInfo.pressState = pressState; }
        inline PressState GetPressState() const { return m_ButtonInfo.pressState; }
    private:
        ButtonInfo m_ButtonInfo;
    };
}