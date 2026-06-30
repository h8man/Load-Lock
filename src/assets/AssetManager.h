#pragma once

#include <string>
#include <vector>

namespace assets
{
    struct LevelData
    {
        std::string name;
        std::vector<std::string> rows;
    };

    class AssetManager
    {
    public:
        bool LoadLevels(std::vector<LevelData>& levels, std::string& error) const;
    };
}
