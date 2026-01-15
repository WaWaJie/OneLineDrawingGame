#pragma once


#include"singleton.h"
#include"scene.h"
#include<SDL_ttf.h>

class ConfigManager :public Singleton<ConfigManager>
{
    friend class Singleton<ConfigManager>;

public:
    SceneType current_scene_type= SceneType::Menu;
    SDL_Point pos_cursor = { 0 };

    bool is_infinite = false;

};