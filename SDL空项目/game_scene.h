#pragma once

#include"scene.h"
#include"map.h"
#include"rabbit.h"
#include"text_button.h"

class GameScene : public Scene
{
public:
	GameScene();
	~GameScene();

	void on_enter()override;
	void on_exit()override;

	void on_update(float delta)override;
	void on_render(SDL_Renderer*renderer)override;
	void on_input(const SDL_Event& event)override;

private:
	void render_map(SDL_Renderer* renderer, Map* mp);
	void render_menu(SDL_Renderer* renderer);

	void update_tile();

	void check_win()
	{
		if (!is_game_over && tile_num_owned == tile_num_needed && map_cache->m_mp[idx_cur.x][idx_cur.y] == (int)TileType::End)
		{
			is_game_over = true;
			std::cout << "You Win!" << std::endl;
		}
	}

	void game_restart()
	{
		win_anim_can_show = true;
		is_game_over = false;
		tile_num_owned = 0;
		anim_win->reset();
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				map_cache->m_mp[i][j] = map_ini->m_mp[i][j];

				if (map_ini->m_mp[i][j] == (int)TileType::Start)
				{
					idx_cur.x = i;
					idx_cur.y = j;
				}
			}
		}

	}

private:
	Map* map_ini = nullptr;		//初始地图
	Map* map_cache = nullptr;	//缓存地图
	Rabbit* rabbit = nullptr;	

	bool is_button_down = false;	
	bool can_restart = true;
	SDL_Point cursor_pos = { 0 };
	SDL_Point cur_tile_idx_cursor = { 0 };
	SDL_Point idx_cur = { 0 };

	int st_j = 300;

	SDL_Texture* tex_text_background = nullptr;
	SDL_Texture* tex_text_foreground = nullptr;

	SDL_Color color_text_fg = { 255,255,255,255 };
    SDL_Color color_text_bg = { 125,125,125,255 };

	int tile_num_needed = 0;
	int tile_num_owned = 0;
	bool is_game_over = false;
	Animation* anim_win = nullptr;
	bool win_anim_can_show = true;

};
