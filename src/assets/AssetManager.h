#pragma once

#include <string>
#include <vector>

namespace assets
{
    class AssetManager
    {
    public:
        std::string FindLevelPath(const std::string& levelName) const;
        bool LoadLevelRows(const std::string& path, std::vector<std::string>& rows) const;
    };
}
