#ifdef LOAD_AND_LOCK_USE_RAYLIB

#include "RaylibRenderer.h"

#include "../game/GameLogic.h"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <raylib.h>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace
{
    constexpr int kWindowWidth = 1280;
    constexpr int kWindowHeight = 900;
    constexpr int kSpriteRows = 3;
    constexpr int kSpriteColumns = 6;
    constexpr float kHorizontalPadding = 32.0f;
    constexpr float kTopPadding = 120.0f;
    constexpr float kBottomPadding = 120.0f;

    std::string FindSpriteSheetPath()
    {
        const fs::path candidates[] =
        {
            "sprites.png",
            "./sprites.png",
            "../sprites.png",
            "../../sprites.png"
        };

        for (const fs::path& candidate : candidates)
        {
            if (fs::exists(candidate) && fs::is_regular_file(candidate))
            {
                return candidate.string();
            }
        }

        return candidates[0].string();
    }

    float Clamp01(float value)
    {
        return std::clamp(value, 0.0f, 1.0f);
    }

    float EaseOutCubic(float value)
    {
        const float t = 1.0f - Clamp01(value);
        return 1.0f - (t * t * t);
    }

    float EaseInOutSine(float value)
    {
        return -(std::cos(3.14159265f * Clamp01(value)) - 1.0f) * 0.5f;
    }

    int GetSpriteIndex(char tile)
    {
        switch (tile)
        {
        case '#': return 0;
        case '.': return 5;
        case '@': return 12;
        case '+': return 12;
        case '$': return 3;
        case '*': return 4;
        case ' ': return 1;
        case 'D': return 2;
        default: return 1;
        }
    }

    void DrawFallbackTile(char tile, const Rectangle& destination)
    {
        DrawRectangleRounded(destination, 0.1f, 8, Color{ 80, 86, 98, 255 });

        switch (tile)
        {
        case '#':
            DrawRectangleRounded(destination, 0.12f, 8, Color{ 92, 118, 148, 255 });
            break;
        case '.':
            DrawCircle(static_cast<int>(destination.x + destination.width * 0.5f), static_cast<int>(destination.y + destination.height * 0.5f), destination.width * 0.12f, Color{ 255, 215, 90, 255 });
            break;
        case '@':
            DrawCircle(static_cast<int>(destination.x + destination.width * 0.5f), static_cast<int>(destination.y + destination.height * 0.5f), destination.width * 0.28f, Color{ 110, 220, 255, 255 });
            break;
        case '+':
            DrawCircle(static_cast<int>(destination.x + destination.width * 0.5f), static_cast<int>(destination.y + destination.height * 0.5f), destination.width * 0.28f, Color{ 110, 220, 255, 255 });
            DrawCircleLines(static_cast<int>(destination.x + destination.width * 0.5f), static_cast<int>(destination.y + destination.height * 0.5f), destination.width * 0.18f, Color{ 255, 215, 90, 255 });
            break;
        case '$':
            DrawRectangleRounded(
                Rectangle{
                    destination.x + destination.width * 0.2f,
                    destination.y + destination.height * 0.2f,
                    destination.width * 0.6f,
                    destination.height * 0.6f
                },
                0.1f,
                8,
                Color{ 181, 120, 65, 255 });
            break;
        case '*':
            DrawRectangleRounded(
                Rectangle{
                    destination.x + destination.width * 0.2f,
                    destination.y + destination.height * 0.2f,
                    destination.width * 0.6f,
                    destination.height * 0.6f
                },
                0.1f,
                8,
                Color{ 181, 120, 65, 255 });
            DrawCircleLines(static_cast<int>(destination.x + destination.width * 0.5f), static_cast<int>(destination.y + destination.height * 0.5f), destination.width * 0.18f, Color{ 255, 215, 90, 255 });
            break;
        default:
            break;
        }
    }
}

namespace renderer
{
    struct RaylibRenderer::Impl
    {
        Texture2D sprites = { 0 };
        bool hasSprites = false;
        bool useFallbackGraphics = false;
        std::string spriteSheetPath;
    };

    RaylibRenderer::RaylibRenderer() : impl_(std::make_unique<Impl>())
    {
        SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
        InitWindow(kWindowWidth, kWindowHeight, "Load & Lock");
        SetTargetFPS(60);

        impl_->spriteSheetPath = FindSpriteSheetPath();
        if (fs::exists(impl_->spriteSheetPath))
        {
            impl_->sprites = LoadTexture(impl_->spriteSheetPath.c_str());
            impl_->hasSprites = impl_->sprites.id != 0;
        }
    }

    RaylibRenderer::~RaylibRenderer()
    {
        if (impl_->hasSprites)
        {
            UnloadTexture(impl_->sprites);
        }

        if (IsWindowReady())
        {
            CloseWindow();
        }
    }

    bool RaylibRenderer::IsOpen() const
    {
        return IsWindowReady() && !WindowShouldClose();
    }

    void RaylibRenderer::Render(const game::GameLogic& gameState) const
    {
        if (IsKeyPressed(KEY_G))
        {
            impl_->useFallbackGraphics = !impl_->useFallbackGraphics;
        }

        BeginDrawing();
        ClearBackground(Color{ 24, 28, 36, 255 });

        DrawText("Load & Lock", 24, 24, 36, RAYWHITE);
        DrawText(TextFormat("Level %d / %d: %s", gameState.GetLevelNumber(), gameState.GetLevelCount(), gameState.GetLevelName().c_str()), 24, 68, 24, LIGHTGRAY);

        const int fieldWidth = std::max(1, gameState.GetWidth());
        const int fieldHeight = std::max(1, gameState.GetHeight());
        const int displayWidth = std::max(8, fieldWidth);
        const int displayHeight = std::max(8, fieldHeight);
        const float usableWidth = static_cast<float>(GetScreenWidth()) - (kHorizontalPadding * 2.0f);
        const float usableHeight = static_cast<float>(GetScreenHeight()) - kTopPadding - kBottomPadding;
        const float tileSize = std::max(12.0f, std::floor(std::min(
            usableWidth / static_cast<float>(displayWidth),
            usableHeight / static_cast<float>(displayHeight))));

        const float boardWidth = tileSize * static_cast<float>(displayWidth);
        const float boardHeight = tileSize * static_cast<float>(displayHeight);
        const float startX = std::max(kHorizontalPadding, (static_cast<float>(GetScreenWidth()) - boardWidth) * 0.5f);
        const float startY = kTopPadding;
        const float fieldStartX = startX + (static_cast<float>(displayWidth - fieldWidth) * tileSize * 0.5f);
        const float fieldStartY = startY + (static_cast<float>(displayHeight - fieldHeight) * tileSize * 0.5f);

        DrawRectangleRounded(
            Rectangle{ startX - 8.0f, startY - 8.0f, boardWidth + 16.0f, boardHeight + 16.0f },
            0.02f,
            8,
            Color{ 36, 41, 50, 255 });

        const bool useSpriteGraphics = impl_->hasSprites && !impl_->useFallbackGraphics;
        const float spriteWidth = 225;
        const float spriteHeight = 225;
        const game::CutsceneState& cutsceneState = gameState.GetCutsceneState();
        const bool showFinalCutscene = cutsceneState.isActive;
        const game::Position playerPosition = gameState.GetPlayerPosition();
        struct AnimatedCrate
        {
            char tile;
            Rectangle destination;
        };
        std::vector<AnimatedCrate> animatedCrates;
        const auto drawTile = [&](char tile, const Rectangle& destination, Color tint = WHITE)
        {
            DrawRectangleRec(destination, tile == ' ' ? Color{ 30, 34, 42, 255 } : Color{ 62, 67, 77, 255 });

            const int spriteIndex = GetSpriteIndex(tile);
            if (useSpriteGraphics && spriteIndex >= 0)
            {
                const int spriteColumn = spriteIndex % kSpriteColumns;
                const int spriteRow = spriteIndex / kSpriteColumns;
                const int hPadding = 25 * spriteColumn + 25;
                const int vPadding = 75 * spriteRow + 25;
                DrawTexturePro(
                    impl_->sprites,
                    Rectangle{
                        spriteWidth * static_cast<float>(spriteColumn) + hPadding,
                        spriteHeight * static_cast<float>(spriteRow) + vPadding,
                        spriteWidth,
                        spriteHeight
                    },
                    destination,
                    Vector2{ 0.0f, 0.0f },
                    0.0f,
                    tint);
            }
            else if (spriteIndex >= 0)
            {
                DrawFallbackTile(tile, destination);
                if (tint.a < 255)
                {
                    DrawRectangleRec(destination, Fade(Color{ 255, 255, 255, 255 }, 1.0f - (static_cast<float>(tint.a) / 255.0f)));
                }
            }
        };

        for (int y = 0; y < gameState.GetHeight(); ++y)
        {
            for (int x = 0; x < gameState.GetWidth(); ++x)
            {
                char tile = gameState.GetRenderTile(x, y);
                const Rectangle destination =
                {
                    fieldStartX + static_cast<float>(x) * tileSize,
                    fieldStartY + static_cast<float>(y) * tileSize,
                    tileSize,
                    tileSize
                };

                if (showFinalCutscene)
                {
                    if (x == playerPosition.x && y == playerPosition.y)
                    {
                        tile = tile == '+' ? '.' : ' ';
                    }
                    else if (tile == '$' || tile == '*')
                    {
                        animatedCrates.push_back({ tile, destination });
                        tile = tile == '*' ? '.' : ' ';
                    }
                }

                drawTile(tile, destination);
            }
        }

        DrawText(TextFormat("Moves: %d", gameState.GetMoveCount()), 24, GetScreenHeight() - 84, 24, RAYWHITE);
        DrawText(TextFormat("Level Score: %d", gameState.GetLevelScore()), 220, GetScreenHeight() - 84, 24, RAYWHITE);
        DrawText(TextFormat("Total Score: %d", gameState.GetTotalScore()), 440, GetScreenHeight() - 84, 24, RAYWHITE);
        DrawText("Controls: WASD/Arrows move, N/P level, R reset, G toggle graphics, Q/Esc quit", 24, GetScreenHeight() - 50, 20, LIGHTGRAY);

        if (!impl_->hasSprites)
        {
            DrawText(TextFormat("Sprite sheet not found: %s", impl_->spriteSheetPath.c_str()), 24, GetScreenHeight() - 114, 18, ORANGE);
        }
        else if (impl_->useFallbackGraphics)
        {
            DrawText("Fallback graphics enabled.", 24, GetScreenHeight() - 114, 20, ORANGE);
        }
        else if (gameState.IsComplete() && !cutsceneState.isOverlayVisible)
        {
            DrawText("Level complete. Press N, P, R or Q.", 24, GetScreenHeight() - 114, 20, Color{ 120, 230, 140, 255 });
        }

        if (gameState.ShouldShowCompletionScore())
        {
            const char* title = gameState.GetLevelNumber() == gameState.GetLevelCount() ? "Final Stage Clear!" : "Stage Clear!";
            const char* stageScoreText = TextFormat("Stage Score: %d", gameState.GetCompletedLevelScore());
            const char* totalScoreText = TextFormat("Total Score: %d", gameState.GetTotalScore());
            const int panelWidth = 420;
            const int panelHeight = 130;
            const int panelX = (GetScreenWidth() - panelWidth) / 2;
            const int panelY = 28;

            DrawRectangleRounded(
                Rectangle{ static_cast<float>(panelX), static_cast<float>(panelY), static_cast<float>(panelWidth), static_cast<float>(panelHeight) },
                0.12f,
                10,
                Fade(BLACK, 0.8f));
            DrawText(title, panelX + 30, panelY + 18, 30, Color{ 255, 230, 120, 255 });
            DrawText(stageScoreText, panelX + 30, panelY + 62, 28, RAYWHITE);
            DrawText(totalScoreText, panelX + 30, panelY + 94, 24, Color{ 120, 230, 140, 255 });
        }

        if (cutsceneState.isActive)
        {
            const float progress = Clamp01(cutsceneState.progress);
            const float warmth = 0.18f + EaseOutCubic(progress) * 0.24f;

            const float bounceWindow = Clamp01((progress - 0.08f) / 0.5f);
            const float bounceOffset = -std::fabs(std::sin(progress * 22.0f)) * tileSize * 0.28f * bounceWindow;
            const char playerTile = gameState.GetRenderTile(playerPosition.x, playerPosition.y);
            const Rectangle playerDestination =
            {
                fieldStartX + static_cast<float>(playerPosition.x) * tileSize,
                fieldStartY + static_cast<float>(playerPosition.y) * tileSize,
                tileSize,
                tileSize
            };
            drawTile(
                playerTile,
                Rectangle{
                    playerDestination.x,
                    playerDestination.y + bounceOffset,
                    playerDestination.width,
                    playerDestination.height
                });

            const float helperArrival = EaseOutCubic(Clamp01((progress - 0.28f) / 0.22f));
            const float pushProgress = EaseInOutSine(Clamp01((progress - 0.48f) / 0.22f));
            const float stackCenterX = static_cast<float>(GetScreenWidth()) * 0.5f;
            const float offscreenMargin = tileSize * 2.0f;

            for (std::size_t crateIndex = 0; crateIndex < animatedCrates.size(); ++crateIndex)
            {
                const AnimatedCrate& animatedCrate = animatedCrates[crateIndex];
                const float direction = (animatedCrate.destination.x + animatedCrate.destination.width * 0.5f) < stackCenterX ? -1.0f : 1.0f;
                const float offscreenTargetX = direction < 0.0f
                    ? -animatedCrate.destination.width - offscreenMargin
                    : static_cast<float>(GetScreenWidth()) + offscreenMargin;
                const float crateX = animatedCrate.destination.x + (offscreenTargetX - animatedCrate.destination.x) * pushProgress;
                const Rectangle movedCrateDestination =
                {
                    crateX,
                    animatedCrate.destination.y,
                    animatedCrate.destination.width,
                    animatedCrate.destination.height
                };
                if (movedCrateDestination.x + movedCrateDestination.width > 0.0f && movedCrateDestination.x < static_cast<float>(GetScreenWidth()))
                {
                    drawTile(animatedCrate.tile, movedCrateDestination);
                }

                const float helperTargetX = movedCrateDestination.x - direction * tileSize * 0.92f;
                const float helperStartX = direction < 0.0f
                    ? static_cast<float>(GetScreenWidth()) + offscreenMargin
                    : -offscreenMargin;
                const float helperX = helperStartX + (helperTargetX - helperStartX) * helperArrival;
                const float helperBob = std::fabs(std::sin(progress * 18.0f + static_cast<float>(crateIndex) * 0.65f)) * tileSize * 0.07f;
                drawTile(
                    '@',
                    Rectangle{
                        helperX,
                        movedCrateDestination.y - helperBob,
                        tileSize,
                        tileSize
                    });
            }
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(Color{ 255, 152, 70, 255 }, warmth));
        }

        if (cutsceneState.isOverlayVisible)
        {
            const float alpha = Clamp01(cutsceneState.overlayOpacity);
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.35f + alpha * 0.5f));

            const int panelWidth = 460;
            const int panelHeight = 220;
            const int panelX = (GetScreenWidth() - panelWidth) / 2;
            const int panelY = (GetScreenHeight() - panelHeight) / 2;
            DrawRectangleRounded(
                Rectangle{ static_cast<float>(panelX), static_cast<float>(panelY), static_cast<float>(panelWidth), static_cast<float>(panelHeight) },
                0.12f,
                10,
                Fade(Color{ 32, 18, 14, 255 }, 0.82f * alpha));

            DrawText("Final Score", panelX + 112, panelY + 34, 38, Fade(Color{ 255, 226, 155, 255 }, alpha));
            DrawText(TextFormat("Total Score: %d", gameState.GetTotalScore()), panelX + 86, panelY + 98, 34, Fade(RAYWHITE, alpha));
            DrawText("Thanks for playing!", panelX + 98, panelY + 150, 28, Fade(Color{ 255, 202, 170, 255 }, alpha));
            DrawText("Press Q or Esc to exit", panelX + 110, panelY + 186, 22, Fade(LIGHTGRAY, alpha));
        }

        EndDrawing();
    }
}

#endif
