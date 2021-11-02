#ifdef FL_EXAMPLES

#include <iostream>
#include "FlameUI.h"
#include "../Window.h"

int main()
{
    std::shared_ptr<FlameBerry::Window> window = FlameBerry::Window::Create();

    /* Only the `_FlameUI` class in the `FlameUI` namespace should be needed */

    /* Always call Init() function first */
    FlameUI::_FlameUI::Init();

    /** Make a Panel using the `_Panel()` function, providing it with `QuadProps` which takes 4 parameters
     * `QuadProps`
     * First parameter specifies the position in pixels where (0, 0) is the center of the screen
     * Second parameter specifies the dimensions in pixel where currently (1280, 720) are the max dimensions
     * Third parameter specifies the color of the panel in the RGBA format\
     * Fourth parameter specifies the file path of the texture image, to put on the panel, you can pass "" if you don't want a texture
     *
     * `_Panel()`
     * First parameter takes in a `QuadProps` object as mentioned above
     * [Optional] Second parameter takes in a uint32_t pointer, where you will store the quad id, which will be used in future to dynamically change quad properties
    */
    FlameUI::_FlameUI::_Panel(FlameUI::QuadProps(fuiVec2<int>(0, 0), fuiVec2<uint32_t>(300, 500), fuiVec4<float>(1.0f, 1.0f, 1.0f, 1.0f), ""));

    /**
     * `_Button()`
     * First parameter takes in a `QuadProps` object, same as `_Panel()` function as mentioned above
     * [Optional] Second parameter takes in a uint32_t pointer, where you will store the quad id, which will be used in future to dynamically change quad properties
    */
    FlameUI::_FlameUI::_Button(FlameUI::QuadProps(fuiVec2<int>(0, 0), fuiVec2<uint32_t>(200, 200), fuiVec4<float>(1.0f, 1.0f, 1.0f, 1.0f), "Sandbox/resources/textures/Checkerboard.png"));
    FlameUI::_FlameUI::_Button(FlameUI::QuadProps(fuiVec2<int>(0, 0), fuiVec2<uint32_t>(200, 200), fuiVec4<float>(1.0f, 1.0f, 1.0f, 1.0f), "Sandbox/resources/textures/ChernoLogo.png"));

    FlameUI::_FlameUI::_Panel(FlameUI::QuadProps(fuiVec2<int>(500, 0), fuiVec2<uint32_t>(200, 400), fuiVec4<float>(1.0f, 1.0f, 0.0f, 1.0f), ""));
    FlameUI::_FlameUI::_Button(FlameUI::QuadProps(fuiVec2<int>(500, 0), fuiVec2<uint32_t>(48, 48), fuiVec4<float>(1.0f, 1.0f, 1.0f, 1.0f), "Sandbox/resources/textures/pause_icon.png"));
    // ---------------------

    while (window->IsRunning())
    {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        /* `OnDraw()` function draws all the panels and buttons mentioned after `_FlameUI::Init()` function at the beginning */
        FlameUI::_FlameUI::OnDraw();

        window->OnUpdate();
    }

    /* `End()` function should be called at the end of the main application loop, to ensure no ram usage when application window is closed */
    FlameUI::_FlameUI::End();
    glfwTerminate();
}
#endif