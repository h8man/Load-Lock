# Load & Lock

*A data-driven Sokoban implementation in C/C++.*

## Overview

**Load & Lock** is a modern implementation of the classic Sokoban puzzle game, built in **C/C++** using a **module-based** and **data-driven** architecture.

The primary goal of this project is to provide a clean, maintainable codebase that separates game logic, rendering, input, and asset management while keeping gameplay entirely driven by external data.

## Architecture

The project is organized into independent modules with clear responsibilities.

```text
src/
├── core/          # Engine core
├── game/          # Gameplay logic
├── renderer/      # Rendering
├── input/         # Input handling
├── assets/        # Asset management
├── audio/         # Audio (optional)
├── utils/         # Common utilities
└── platform/      # Platform-specific code
```

Game content lives outside the executable whenever possible.

```text
assets/
├── levels/
├── textures/
├── fonts/
├── audio/
└── config/
```

## Data-Driven Design

Rather than hardcoding game content, **Load & Lock** loads its data from external files.

This approach allows modifying and expanding the game without recompiling the project.

## License

This project is licensed under the MIT License.