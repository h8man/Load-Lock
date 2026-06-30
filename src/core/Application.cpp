#include "Application.h"

#include "../assets/AssetManager.h"
#include "../game/GameLogic.h"
#include "../input/InputHandler.h"
#include "../renderer/ConsoleRenderer.h"

#include <iostream>
#include <string>
#include <vector>

namespace core
{
    int Application::Run() const
    {
        assets::AssetManager assetManager;
        const std::string levelPath = assetManager.FindLevelPath("level1.txt");

        std::vector<std::string> rows;
        if (!assetManager.LoadLevelRows(levelPath, rows))
        {
            std::cerr << "Failed to load level file: " << levelPath << '\n';
            return 1;
        }

        game::GameLogic gameState;
        std::string error;
        if (!gameState.Load(rows, error))
        {
            std::cerr << "Failed to initialize level: " << error << '\n';
            return 1;
        }

        input::InputHandler inputHandler;
        renderer::ConsoleRenderer renderer;

        while (true)
        {
            renderer.Render(gameState);

            switch (inputHandler.ReadCommand())
            {
            case input::Command::MoveUp:
                if (!gameState.IsComplete())
                {
                    gameState.Move(0, -1);
                }
                break;
            case input::Command::MoveDown:
                if (!gameState.IsComplete())
                {
                    gameState.Move(0, 1);
                }
                break;
            case input::Command::MoveLeft:
                if (!gameState.IsComplete())
                {
                    gameState.Move(-1, 0);
                }
                break;
            case input::Command::MoveRight:
                if (!gameState.IsComplete())
                {
                    gameState.Move(1, 0);
                }
                break;
            case input::Command::Reset:
                gameState.Reset();
                break;
            case input::Command::Quit:
                return 0;
            case input::Command::None:
            default:
                break;
            }
        }
    }
}
