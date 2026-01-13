#pragma once

#include"scene.h"
#include"map.h"
#include"rabbit.h"

class GameScene : public Scene
{
public:
	GameScene();
	~GameScene();

	void on_enter()override;

	void on_update(float delta)override;
	void on_render(SDL_Renderer*renderer)override;
	void on_input(const SDL_Event& event)override;

private:
	void render_map(SDL_Renderer* renderer, Map* mp);
	void render_menu(SDL_Renderer* renderer);

private:
	Map* editor_map = nullptr;
	SDL_Point idx_cur = { 0 };
	Rabbit* rabbit = nullptr;

	bool is_button_down = false;
	SDL_Point cursor_pos = { 0 };

	int st_j = 300;

	SDL_Texture* tex_text_background = nullptr;
	SDL_Texture* tex_text_foreground = nullptr;

	SDL_Color color_text_fg = { 255,255,255,255 };
    SDL_Color color_text_bg = { 125,125,125,255 };
};
