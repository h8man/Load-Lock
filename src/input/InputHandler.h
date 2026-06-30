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
        Reset,
        Quit
    };

    class InputHandler
    {
    public:
        Command ReadCommand() const;
    };
}
