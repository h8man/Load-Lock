#pragma once
#pragma once

#include <chrono>

namespace game
{
    struct CutsceneState
    {
        bool isActive = false;
        float progress = 0.0f;
        bool isOverlayVisible = false;
        float overlayOpacity = 0.0f;
    };

    void ResetCutsceneState(CutsceneState& cutsceneState);
    void StartCutscene(CutsceneState& cutsceneState);
    void FinishCutscene(CutsceneState& cutsceneState);
    void SetCutsceneProgress(CutsceneState& cutsceneState, float progress);
    void SetCutsceneOverlay(CutsceneState& cutsceneState, bool isVisible, float opacity);

    class CutscenePlayer
    {
    public:
        using Clock = std::chrono::steady_clock;

        void Reset();
        void Start(Clock::time_point now);
        void Update(Clock::time_point now);
        const CutsceneState& GetState() const;

    private:
        CutsceneState state_;
        Clock::time_point startedAt_ = Clock::time_point::min();
    };
}
