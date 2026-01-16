#pragma once

#include"scene.h"
#include"map.h"
#include"rabbit.h"
#include"text_button.h"
#include"config_manager.h"

#include<Windows.h>	

#include <random>
#include <queue>
#include <algorithm>
#include <set>
#include <sstream>

class GameScene : public Scene
{
public:
	GameScene();
	~GameScene();

	void on_enter()override;
	void on_exit()override;

	void on_update(float delta)override;
	void on_render(SDL_Renderer* renderer)override;
	void on_input(const SDL_Event& event)override;

private:
	void render_map(SDL_Renderer* renderer, Map* mp);
	void render_menu(SDL_Renderer* renderer);

	void update_tile();
	void generate_random_map();

	void check_win()
	{
		if (!is_game_over && tile_num_owned == tile_num_needed && map_cache->m_mp[idx_cur.x][idx_cur.y] == (int)TileType::End)
		{
			is_game_over = true;
			std::cout << "You Win!" << std::endl;
		}
	}

	void import_map();
    void export_map();

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

    bool is_map_reachable()
    {
        // 找到起点
        SDL_Point start;
        bool found_start = false;

        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                if (map_ini->m_mp[i][j] == (int)TileType::Start) {
                    start.x = i;
                    start.y = j;
                    found_start = true;
                    break;
                }
            }
            if (found_start) break;
        }

        if (!found_start) return false;

        // 使用BFS检查从起点是否可以到达所有正常格、已选格和终点
        bool visited[8][8] = { false };
        std::queue<SDL_Point> q;
        q.push(start);
        visited[start.x][start.y] = true;

        int reachable_count = 0;
        int total_reachable = 0;

        // 计算需要到达的格子总数
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                int tile = map_ini->m_mp[i][j];
                if (tile == (int)TileType::Idle || tile == (int)TileType::End) {
                    total_reachable++;
                }
            }
        }

        // BFS
        while (!q.empty()) {
            SDL_Point current = q.front();
            q.pop();

            // 检查当前格子
            int tile = map_ini->m_mp[current.x][current.y];
            if (tile == (int)TileType::Idle || tile == (int)TileType::End) {
                reachable_count++;
            }

            // 检查四个方向
            SDL_Point directions[4] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };
            for (const auto& dir : directions) {
                SDL_Point next = { current.x + dir.x, current.y + dir.y };

                // 检查边界
                if (next.x < 0 || next.x >= 8 || next.y < 0 || next.y >= 8) continue;

                // 检查是否已访问
                if (visited[next.x][next.y]) continue;

                // 检查是否可通行（不是禁用格）
                int next_tile = map_ini->m_mp[next.x][next.y];
                if (next_tile != (int)TileType::Failed) {
                    visited[next.x][next.y] = true;
                    q.push(next);
                }
            }
        }

        // 如果所有需要到达的格子都可达，返回true
        return reachable_count == total_reachable;
    }

private:
	Map* map_ini = nullptr;		//初始地图
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

	int tile_num_needed = 0;
	int tile_num_owned = 0;
	bool is_game_over = false;
	Animation* anim_win = nullptr;
	bool win_anim_can_show = true;

};
