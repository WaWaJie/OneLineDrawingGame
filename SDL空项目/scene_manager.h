#pragma once

#include"scene.h"
#include"game_scene.h"
#include"menu_scene.h"
#include"singleton.h"
#include"editor_scene.h"


class SceneManager : public Singleton<SceneManager>
{
	friend class Singleton<SceneManager>;

private:
	SceneManager()
	{
		menu_scene = new MenuScene(); //这里可以替换为具体的场景类
		game_scene = new GameScene();
		editor_scene = new EditorScene();
		current_scene = menu_scene;
		current_scene->on_enter();
	}

public:
	

	void on_update(float delta)
	{
		if (current_scene)
			current_scene->on_update(delta);

		if (current_scene_type != ConfigManager::get_instance()->current_scene_type)
		{
			current_scene_type = ConfigManager::get_instance()->current_scene_type;
			switch_to(current_scene_type);
		}
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

	SceneType current_scene_type = SceneType::Menu;
};