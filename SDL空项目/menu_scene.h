#pragma once


#include"scene.h"
#include"text_button.h"
#include"config_manager.h"

#include"rabbit.h"

class MenuScene :public Scene
{
public:
	MenuScene()
	{
		button_start_game = new TextButton(u8"开始游戏", { (1068 - 200) / 2,200,200,60 }, { 1,1,1,1 });
		button_level_editor = new TextButton(u8"关卡编辑", { (1068 - 200) / 2,300,200,60 }, { 1,1,1,1 });
		button_start_game->set_color_text({
147,112,219,255 });
		button_level_editor->set_color_text({
147,112,219,255 });

	}

	~MenuScene()
	{
		delete button_start_game;
		delete button_level_editor;
		delete rabbit;
		delete anim_target;
	}
	void on_enter() override;
	void on_exit() override;

	void on_update(float delta) override;
	void on_render(SDL_Renderer* renderer) override;
	void on_input(const SDL_Event& event) override;

private:
	bool is_button = false;
	TextButton* button_start_game = nullptr;
	TextButton* button_level_editor = nullptr;

	Rabbit*rabbit = nullptr;
	Animation*anim_target = nullptr;
};