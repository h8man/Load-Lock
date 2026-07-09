#include "Cutscene.h"
#include "Cutscene.h"

#include <algorithm>

namespace
{
    float Clamp01(float value)
    {
        return std::clamp(value, 0.0f, 1.0f);
    }

    constexpr auto kFinalCutsceneDuration = std::chrono::milliseconds(6500);
    constexpr auto kFinalScoreFadeStart = std::chrono::milliseconds(4700);
    constexpr auto kFinalScoreFadeDuration = std::chrono::milliseconds(1600);
}

namespace game
{
    void ResetCutsceneState(CutsceneState& cutsceneState)
    {
        cutsceneState = {};
    }

    void StartCutscene(CutsceneState& cutsceneState)
    {
        ResetCutsceneState(cutsceneState);
        cutsceneState.isActive = true;
    }

    void FinishCutscene(CutsceneState& cutsceneState)
    {
        cutsceneState.isActive = false;
        cutsceneState.progress = 1.0f;
        cutsceneState.isOverlayVisible = true;
        cutsceneState.overlayOpacity = 1.0f;
    }

    void SetCutsceneProgress(CutsceneState& cutsceneState, float progress)
    {
        cutsceneState.progress = Clamp01(progress);
    }

    void SetCutsceneOverlay(CutsceneState& cutsceneState, bool isVisible, float opacity)
    {
        cutsceneState.isOverlayVisible = isVisible;
        cutsceneState.overlayOpacity = Clamp01(opacity);
    }

    void CutscenePlayer::Reset()
    {
        ResetCutsceneState(state_);
        startedAt_ = Clock::time_point::min();
    }

    void CutscenePlayer::Start(Clock::time_point now)
    {
#ifdef LOAD_AND_LOCK_USE_RAYLIB
        StartCutscene(state_);
        startedAt_ = now;
#else
        (void)now;
#endif
    }

    void CutscenePlayer::Update(Clock::time_point now)
    {
#ifdef LOAD_AND_LOCK_USE_RAYLIB
        if (!state_.isActive)
        {
            return;
        }

        const auto elapsed = now - startedAt_;
        SetCutsceneProgress(
            state_,
            std::chrono::duration<float>(elapsed).count() / std::chrono::duration<float>(kFinalCutsceneDuration).count());

        if (elapsed >= kFinalScoreFadeStart)
        {
            SetCutsceneOverlay(
                state_,
                true,
                std::chrono::duration<float>(elapsed - kFinalScoreFadeStart).count() / std::chrono::duration<float>(kFinalScoreFadeDuration).count());
        }
        else
        {
            SetCutsceneOverlay(state_, false, 0.0f);
        }

        if (elapsed >= kFinalCutsceneDuration)
        {
            FinishCutscene(state_);
        }
#else
        (void)now;
#endif
    }

    const CutsceneState& CutscenePlayer::GetState() const
    {
        return state_;
    }
}
