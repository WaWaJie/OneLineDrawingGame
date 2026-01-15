#include"menu_scene.h"

void MenuScene::on_enter()
{
	
	button_start_game->reset();
	button_level_editor->reset();
	button_infinite->reset();
}

void MenuScene::on_exit()
{

}

void MenuScene::on_update(float delta)
{

	button_start_game->on_update(delta, ConfigManager::get_instance()->pos_cursor);
	button_level_editor->on_update(delta, ConfigManager::get_instance()->pos_cursor);
	button_infinite->on_update(delta, ConfigManager::get_instance()->pos_cursor);
	
	if (!rabbit)
	{
		rabbit = new Rabbit();
		rabbit->set_pos(-10, -10);
		rabbit->set_scale(8.0);
	}
	if (!anim_target)
	{
		anim_target = new Animation();
		anim_target->set_animation(ResourcesManager::get_instance()->find_texture("target"), 5, 2, { 0,1,2,3,4 });
		anim_target->set_loop(true);
		anim_target->set_interval(0.25f);
	}
	rabbit->on_update(delta);
	anim_target->on_update(delta);


	if (button_start_game->get_pushed())
	{
		ConfigManager::get_instance()->current_scene_type = SceneType::Game;
	}
	if (button_level_editor->get_pushed())
	{
		ConfigManager::get_instance()->current_scene_type = SceneType::Editor;
	}
}

void MenuScene::on_render(SDL_Renderer* renderer)
{
	SDL_RenderCopy(renderer, ResourcesManager::get_instance()->find_texture("menu_background2"), nullptr, nullptr);

	button_start_game->on_render(renderer);
	button_level_editor->on_render(renderer);
	button_infinite->on_render(renderer);
	rabbit->on_render(renderer);

	anim_target->on_render(renderer, { 700,400 }, 0, 8);
}

void MenuScene::on_input(const SDL_Event& event)
{
	switch (event.type)
	{
	case SDL_MOUSEMOTION:
		ConfigManager::get_instance()->pos_cursor.x = event.motion.x;
		ConfigManager::get_instance()->pos_cursor.y = event.motion.y;
		break;
	default:
		break;
	}

	button_start_game->on_input(event, ConfigManager::get_instance()->pos_cursor);
	button_level_editor->on_input(event, ConfigManager::get_instance()->pos_cursor);
	button_infinite->on_input(event, ConfigManager::get_instance()->pos_cursor);
}