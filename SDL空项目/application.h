#pragma once
#include<SDL.h>
#include<SDL2_gfxPrimitives.h>
#include<SDL_image.h>
#include<SDL_mixer.h>
#include<SDL_ttf.h>
#include<iostream>
#include<vector>
#include<Windows.h>

#include"singleton.h"
#include"resources_manager.h"
#include"scene_manager.h"
#include"map.h"
#include"rabbit.h"

class Application: public Singleton<Application>
{
	friend class Singleton<Application>;
public:
    Application();
    ~Application();
	void on_run();
	void on_update(float delta);
	void on_render();
	void on_input(const SDL_Event& event);


private:
	const float fps = 60.0f;
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	SDL_Event event;
	int width = 1038, height = 768;
	bool is_quit = false;

	SDL_Rect rect_cursor = { 0,0,38,38 };
};