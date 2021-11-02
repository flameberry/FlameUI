#pragma once
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include "core_macros.h"
#include "Renderer.h"
#include "Panel.h"
#include "opengl_helper_funcs.h"
#include "opengl_debug.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Timer.h"

namespace FlameUI {
    class _FlameUI
    {
    public:
        static void Init();
        static void _Panel(const QuadProps& quadProps, uint32_t* panelQuadId = nullptr);
        static void _Button(const QuadProps& quadProps, uint32_t* buttonQuadId = nullptr);
        static void OnDraw();
        static void End();
    private:
        static std::vector<std::shared_ptr<Panel>> s_Panels;
    };
}