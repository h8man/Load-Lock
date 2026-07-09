#pragma once

#include <string>

namespace game
{
    class GameLogic;
}

namespace renderer
{
    class ConsoleRenderer
    {
    public:
        bool IsOpen() const;
        void Render(const game::GameLogic& gameState) const;

    private:
        void ClearConsole() const;
    };
}
