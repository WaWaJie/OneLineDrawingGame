#pragma once


#include"singleton.h"
#include"scene.h"
#include<SDL_ttf.h>
#include<fstream>
#include<string>
#include<sstream>

class ConfigManager :public Singleton<ConfigManager>
{
    friend class Singleton<ConfigManager>;

public:
    void load_ratio_back(const char* path)
    {
        std::ifstream file(path);
        if (!file.good())
        {
            SDL_Log("Can't open file %s", path);
            return;
        }
        std::string line;
        while (std::getline(file, line))
        {
            ratio_back= std::stoi(line);
            if (ratio_back <= 0)ratio_back = 0;
            if (ratio_back >= 100)ratio_back = 100;
            SDL_Log("ratio_back: %d", ratio_back);
        }
        file.close();
    }

public:
    SceneType current_scene_type= SceneType::Menu;
    SDL_Point pos_cursor = { 0 };

    bool is_infinite = false;

    int ratio_back = 50;
};