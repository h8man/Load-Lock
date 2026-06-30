#include "AssetManager.h"

#include <fstream>

namespace assets
{
    std::string AssetManager::FindLevelPath(const std::string& levelName) const
    {
        const std::string candidates[] =
        {
            "assets/levels/" + levelName,
            "../assets/levels/" + levelName,
            "../../assets/levels/" + levelName
        };

        for (const std::string& candidate : candidates)
        {
            std::ifstream stream(candidate);
            if (stream.good())
            {
                return candidate;
            }
        }

        return candidates[0];
    }

    bool AssetManager::LoadLevelRows(const std::string& path, std::vector<std::string>& rows) const
    {
        std::ifstream file(path);
        if (!file)
        {
            return false;
        }

        rows.clear();

        std::string line;
        while (std::getline(file, line))
        {
            rows.push_back(line);
        }

        return !rows.empty();
    }
}
