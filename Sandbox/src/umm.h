#pragma once

namespace flame {
    class umm
    {
    public:
        umm();
        umm(const char* title);
        ~umm();
        void Run(const char* title);
        void Show(float y);
    private:
        static float s_Number;
    };
}