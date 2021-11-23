#pragma once
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include "Renderer.h"
#include "Panel.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Timer.h"
#include "Core.h"

namespace FlameUI {
    class _FlameUI
    {
    public:
        static void AddPanel(const std::shared_ptr<Panel>& panel);
        static void OnUpdate();
        static void Init();
    private:
        static void CheckFocus();
        static void ShiftDepthValues(uint32_t current_panel_index, uint32_t last_panel_index);
    private:
        static std::vector<std::shared_ptr<Panel>> s_Panels;
        static std::vector<float> s_DepthValues;
    };
}