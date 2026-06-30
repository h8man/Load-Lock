#include "ConsoleRenderer.h"

#include "../game/GameLogic.h"

#include <cstdlib>
#include <iostream>
#include <string>

namespace
{
    struct TileArt
    {
        const char* rows[3];
    };

    TileArt GetTileArt(char tile)
    {
        switch (tile)
        {
        case '#':
            return { { "###", "###", "###" } };
        case '.':
            return { { "   ", " . ", "   " } };
        case '@':
            return { { " O ", "/|\\", "/ \\" } };
        case '+':
            return { { " O ", "/.\\", "/ \\" } };
        case '$':
            return { { "+-+", "|B|", "+-+" } };
        case '*':
            return { { "+-+", "|*|", "+-+" } };
        default:
            return { { "   ", "   ", "   " } };
        }
    }
}

namespace renderer
{
    void ConsoleRenderer::Render(const game::GameLogic& gameState, const std::string& levelName, int levelNumber, int levelCount) const
    {
        ClearConsole();

        std::cout << "Load & Lock - Console Sokoban\n\n";
        std::cout << "Level " << levelNumber << " / " << levelCount << ": " << levelName << "\n\n";

        for (int y = 0; y < gameState.GetHeight(); ++y)
        {
            for (int artRow = 0; artRow < 3; ++artRow)
            {
                for (int x = 0; x < gameState.GetWidth(); ++x)
                {
                    const TileArt art = GetTileArt(gameState.GetRenderTile(x, y));
                    std::cout << art.rows[artRow];
                }

                std::cout << '\n';
            }
        }

        std::cout << "\nMoves: " << gameState.GetMoveCount() << '\n';
        std::cout << "Controls: WASD or Arrow Keys to move, N/P to change level, R to reset, Q to quit.\n";

        if (gameState.IsComplete())
        {
            std::cout << "\nLevel complete. Press N for next level, P for previous level, R to restart, or Q to quit.\n";
        }
    }

    void ConsoleRenderer::ClearConsole() const
    {
        std::system("cls");
    }
}
