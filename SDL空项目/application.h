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


	void render_map(Map* mp);

private:
	const float fps = 60.0f;
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	SDL_Event event;
	int width = 968, height = 768;
	bool is_quit = false;

	Map* editor_map = nullptr;

	SDL_Rect rect_cursor = { 0,0,38,38 };

	SDL_Point idx_cur = { 0 };
	Rabbit* rabbit = nullptr;
};