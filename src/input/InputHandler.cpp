#include "InputHandler.h"

#include <cctype>
#include <conio.h>

namespace input
{
    Command InputHandler::ReadCommand() const
    {
        const int key = _getch();

        if (key == 0 || key == 224)
        {
            const int arrow = _getch();
            switch (arrow)
            {
            case 72:
                return Command::MoveUp;
            case 80:
                return Command::MoveDown;
            case 75:
                return Command::MoveLeft;
            case 77:
                return Command::MoveRight;
            default:
                return Command::None;
            }
        }

        switch (std::tolower(static_cast<unsigned char>(key)))
        {
        case 'w':
            return Command::MoveUp;
        case 's':
            return Command::MoveDown;
        case 'a':
            return Command::MoveLeft;
        case 'd':
            return Command::MoveRight;
        case 'r':
            return Command::Reset;
        case 'q':
            return Command::Quit;
        default:
            return Command::None;
        }
    }
}
