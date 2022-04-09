#pragma once
#include "Window.h"
#include "FlameUILayer.h"

namespace Flameberry {
    class SandboxApp
    {
    public:
        SandboxApp();
        ~SandboxApp();

        Window& GetMainWindow() { return m_Window; }
        static SandboxApp* GetApp() { return s_App; }

        void Run();
    private:
        Window m_Window;
        Flameberry::FlameUILayer m_FlameUILayer;
    private:
        static SandboxApp* s_App;
    };
}