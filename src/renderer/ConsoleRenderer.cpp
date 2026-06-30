#include "ConsoleRenderer.h"

#include "../game/GameLogic.h"

#include <cstdlib>
#include <iostream>

namespace renderer
{
    void ConsoleRenderer::Render(const game::GameLogic& gameState) const
    {
        ClearConsole();

        std::cout << "Load & Lock - Console Sokoban\n\n";

        for (int y = 0; y < gameState.GetHeight(); ++y)
        {
            for (int x = 0; x < gameState.GetWidth(); ++x)
            {
                std::cout << gameState.GetRenderTile(x, y);
            }

            std::cout << '\n';
        }

        std::cout << "\nMoves: " << gameState.GetMoveCount() << '\n';
        std::cout << "Controls: WASD or Arrow Keys to move, R to reset, Q to quit.\n";

        if (gameState.IsComplete())
        {
            std::cout << "\nLevel complete. Press R to restart or Q to quit.\n";
        }
    }

    void ConsoleRenderer::ClearConsole() const
    {
        std::system("cls");
    }
}
