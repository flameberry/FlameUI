#pragma once
#include <vector>
#include "Button.h"

namespace FlameUI {
    class Panel
    {
    public:
        Panel(
            const fuiVec2<int>& position_in_pixels,
            const fuiVec2<uint32_t>& dimensions_in_pixels,
            const fuiVec4<float>& color,
            const std::string& textureFilePath
        );
        ~Panel() = default;

        void AddButton(
            uint32_t* quadId,
            const fuiVec2<int>& position_in_pixels,
            const fuiVec2<uint32_t>& dimensions_in_pixels,
            const fuiVec4<float>& color,
            const std::string& textureFilePath
        );
        void OnDraw();

        static std::shared_ptr<Panel> Create(
            const fuiVec2<int>& position_in_pixels,
            const fuiVec2<uint32_t>& dimensions_in_pixels,
            const fuiVec4<float>& color,
            const std::string& textureFilePath
        );
        inline uint32_t GetPanelQuadId() const { return m_PanelQuadId; }
    private:
        uint32_t m_PanelQuadId;
        std::vector<std::shared_ptr<Button>> m_Buttons;
    };
}