#pragma once

namespace input
{
    enum class Command
    {
        None,
        MoveUp,
        MoveDown,
        MoveLeft,
        MoveRight,
        PreviousLevel,
        NextLevel,
        Reset,
        Quit
    };

    class InputHandler
    {
    public:
        Command ReadCommand() const;
    };
}
