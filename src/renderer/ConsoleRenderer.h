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
        void Render(const game::GameLogic& gameState, const std::string& levelName, int levelNumber, int levelCount) const;

    private:
        void ClearConsole() const;
    };
}
