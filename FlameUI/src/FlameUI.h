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
        static void InvalidatePanelPositions(int current_panel_index);
        static void CheckFocus();
    private:
        /// First `uint16_t` stores the index of the panel in the `s_Panels` vector,
        /// Second `uint16_t` stores the position of the panel, in terms of the z value
        static std::vector<uint16_t>               s_PanelPositions;
        static std::vector<std::shared_ptr<Panel>> s_Panels;
        static std::vector<float>                  s_DepthValues;
    };
}