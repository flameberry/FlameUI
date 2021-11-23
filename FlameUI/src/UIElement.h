#pragma once

/// Just experimental file
namespace FlameUI {
    class UIElement
    {
    public:
        virtual ~UIElement() = default;
    public:
        struct Bounds
        {
            float Left, Right, Bottom, Top;
        };
    };

    class Button : public UIElement
    {
    public:
        Button() = default;
        virtual ~Button() = default;
    private:
        Bounds m_Bounds;
    };
}