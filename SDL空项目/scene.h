#pragma once

#include<SDL.h>


enum class SceneType
{
	Menu, Game, Editor
};

class Scene
{
public:
	Scene() = default;
	~Scene()= default;

	virtual void on_enter() {}
	virtual void on_exit() {}

	virtual void on_update(float delta) {}
	virtual void on_render(SDL_Renderer* renderer) {}
	virtual void on_input(const SDL_Event& event) {}

};


