#pragma once

#include"scene.h"
#include"game_scene.h"
#include"singleton.h"


class SceneManager : public Singleton<SceneManager>
{
	friend class Singleton<SceneManager>;
public:
	SceneManager()
	{
		menu_scene = new Scene(); //这里可以替换为具体的场景类
		game_scene = new GameScene();
		editor_scene = new Scene();
		current_scene = game_scene;
		current_scene->on_enter();
	}


	void on_update(float delta)
	{
		if (current_scene)
			current_scene->on_update(delta);
	}
	void on_render(SDL_Renderer * renderer)
	{
		if (current_scene)
			current_scene->on_render(renderer);
	}
	void on_input(const SDL_Event& event)
	{
		if (current_scene)
			current_scene->on_input(event);
	}
	void switch_to(SceneType type)
	{
		current_scene->on_exit();
		switch (type)
		{
		case SceneType::Menu:
			current_scene = menu_scene;
			break;
		case SceneType::Game:
			current_scene = game_scene;
			break;
		case SceneType::Editor:
			current_scene = editor_scene;
			break;
		default:
			break;
		}
		current_scene->on_enter();
	}


private:
	Scene* current_scene = nullptr;
	Scene* menu_scene = nullptr;
	Scene* game_scene = nullptr;
	Scene* editor_scene = nullptr;

};