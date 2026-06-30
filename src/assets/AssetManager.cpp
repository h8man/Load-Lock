#include "AssetManager.h"
#include "AssetManager.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace
{
    std::string FindLevelsDirectory()
    {
        const std::string candidates[] =
        {
            "assets/levels",
            "../assets/levels",
            "../../assets/levels"
        };

        for (const std::string& candidate : candidates)
        {
            const fs::path candidatePath(candidate);
            if (fs::exists(candidatePath) && fs::is_directory(candidatePath))
            {
                return candidatePath.string();
            }
        }

        return candidates[0];
    }

    std::string ToLower(std::string value)
    {
        std::transform(value.begin(), value.end(), value.begin(), [](unsigned char character)
        {
            return static_cast<char>(std::tolower(character));
        });

        return value;
    }

    std::string BuildLevelName(const std::string& fileName, int levelIndex)
    {
        const std::string::size_type extensionIndex = fileName.find_last_of('.');
        const std::string baseName = extensionIndex == std::string::npos ? fileName : fileName.substr(0, extensionIndex);
        return levelIndex == 1 ? baseName : baseName + " #" + std::to_string(levelIndex);
    }

    void AppendLevelBlock(const std::string& fileName, int levelIndex, std::vector<std::string>& rows, std::vector<assets::LevelData>& levels)
    {
        if (rows.empty())
        {
            return;
        }

        assets::LevelData level;
        level.name = BuildLevelName(fileName, levelIndex);
        level.rows.swap(rows);
        levels.push_back(level);
        rows.clear();
    }

    bool LoadLevelsFromFile(const std::string& path, const std::string& fileName, std::vector<assets::LevelData>& levels)
    {
        std::ifstream file(path);
        if (!file)
        {
            return false;
        }

        std::vector<std::string> rows;
        std::string line;
        int levelIndex = 1;

        while (std::getline(file, line))
        {
            if (!line.empty() && line.back() == '\r')
            {
                line.pop_back();
            }

            if (!line.empty() && line[0] == ';')
            {
                AppendLevelBlock(fileName, levelIndex, rows, levels);
                if (!levels.empty() && levels.back().name == BuildLevelName(fileName, levelIndex))
                {
                    ++levelIndex;
                }
                continue;
            }

            if (line.empty())
            {
                AppendLevelBlock(fileName, levelIndex, rows, levels);
                if (!levels.empty() && levels.back().name == BuildLevelName(fileName, levelIndex))
                {
                    ++levelIndex;
                }
                continue;
            }

            rows.push_back(line);
        }

        AppendLevelBlock(fileName, levelIndex, rows, levels);
        return true;
    }
}

namespace assets
{
    bool AssetManager::LoadLevels(std::vector<LevelData>& levels, std::string& error) const
    {
        levels.clear();

        const fs::path levelsDirectory(FindLevelsDirectory());
        if (!fs::exists(levelsDirectory) || !fs::is_directory(levelsDirectory))
        {
            error = "Levels directory was not found: " + levelsDirectory.string() + ".";
            return false;
        }

        std::vector<std::string> fileNames;
        for (const fs::directory_entry& entry : fs::directory_iterator(levelsDirectory))
        {
            if (!fs::is_regular_file(entry.status()))
            {
                continue;
            }

            if (ToLower(entry.path().extension().string()) == ".sok")
            {
                fileNames.push_back(entry.path().filename().string());
            }
        }

        if (fileNames.empty())
        {
            error = "No .sok files were found in " + levelsDirectory.string() + ".";
            return false;
        }

        std::sort(fileNames.begin(), fileNames.end());

        for (const std::string& fileName : fileNames)
        {
            if (!LoadLevelsFromFile((levelsDirectory / fileName).string(), fileName, levels))
            {
                error = "Failed to read level file: " + fileName;
                return false;
            }
        }

        if (levels.empty())
        {
            error = "No playable levels were found in .sok files.";
            return false;
        }

        return true;
    }
}