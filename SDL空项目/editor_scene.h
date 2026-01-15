#pragma once

#include"scene.h"
#include"map.h"
#include"rabbit.h"
#include"text_button.h"
#include"config_manager.h"

#include<vector>
#include<Windows.h>

class EditorScene : public Scene
{
public:
	EditorScene();
	~EditorScene();

	void on_enter()override;
	void on_exit()override;

	void on_update(float delta)override;
	void on_render(SDL_Renderer* renderer)override;
	void on_input(const SDL_Event& event)override;

private:
	void render_map(SDL_Renderer* renderer, Map* mp);
	void render_menu(SDL_Renderer* renderer);

	void update_tile();

	void game_restart()
	{
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				map_cache->m_mp[i][j] = -1;
			}
		}
		path_list.clear();
	}

	void use_path_info()
	{
		for (int i = 0; i < path_list.size(); i++)
		{
			if(i==0)map_cache->m_mp[path_list[i].x][path_list[i].y] = (int)TileType::Start;	//设置起点
			else if (i == path_list.size() - 1 && i != 0)map_cache->m_mp[path_list[i].x][path_list[i].y] = (int)TileType::End;	//设置终点
			else map_cache->m_mp[path_list[i].x][path_list[i].y] = (int)TileType::Idle;	//设置路径
		}
	}

	void export_map();

private:
	Map* map_cache = nullptr;	//缓存地图
	Rabbit* rabbit = nullptr;

	bool is_button_down = false;
	bool can_restart = true;
	SDL_Point cur_tile_idx_cursor = { 0 };
	SDL_Point idx_cur = { 0 };

	int st_j = 300;

	SDL_Texture* tex_text_background = nullptr;
	SDL_Texture* tex_text_foreground = nullptr;

	SDL_Color color_text_fg = { 255,255,255,255 };
	SDL_Color color_text_bg = { 125,125,125,255 };

	std::vector<SDL_Point>path_list;
};
