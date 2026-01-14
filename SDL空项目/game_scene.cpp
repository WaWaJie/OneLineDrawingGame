#include"game_scene.h"
//cout


void GameScene::on_enter()
{
    Mix_PlayChannel(-1, ResourcesManager::get_instance()->find_audio("game_audio"), -1);

    SDL_Log("Enter Game Scene");
	is_button_down = false;
    tile_num_owned = 0;
    is_game_over = false;
    win_anim_can_show = true;
    anim_win->reset();
}  

void GameScene::on_exit()
{
    Mix_HaltChannel(-1);
}

void GameScene::render_map(SDL_Renderer*renderer,Map* mp)
{
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            int tile_type = mp->m_mp[i][j];
            std::string tile_name = "tile_" + std::to_string(tile_type);
            SDL_Rect dst_rect = { j * 96 + st_j,i * 96,96,96 };
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            SDL_RenderCopy(renderer, ResourcesManager::get_instance()->find_texture(tile_name), NULL, &dst_rect);
            SDL_RenderDrawRect(renderer, &dst_rect);
            if (tile_type == (int)TileType::End && win_anim_can_show)
            {
                static SDL_Rect rect_src_target = { 0,0,48,48 };
                SDL_RenderCopy(renderer, ResourcesManager::get_instance()->find_texture("target"), &rect_src_target, &dst_rect);
            }
        }
    }
}

GameScene::GameScene()
{
    rabbit = new Rabbit();
    map_ini = new Map("map.csv");
    map_cache=new Map("map.csv");
    anim_win = new Animation();
    anim_win->set_animation(ResourcesManager::get_instance()->find_texture("target"), 5, 2, { 0,1,2,3,4 });
    anim_win->set_interval(0.2f);
    anim_win->set_loop(true);
    anim_win->set_on_finished([&]()
        {
            win_anim_can_show = false;
        });

    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            if (map_ini->m_mp[i][j] == (int)TileType::Start)
            {
                idx_cur.x = i;
                idx_cur.y = j;
                std::cout << "Start Position: " << idx_cur.x << "," << idx_cur.y << std::endl;
            }
            else if (map_ini->m_mp[i][j] == (int)TileType::Idle)
            {
                tile_num_needed++;
            }
        }
    }
}

GameScene::~GameScene()
{
    delete map_ini;
    delete rabbit;
}

void GameScene::on_update(float delta)
{
    update_tile();
    rabbit->on_update(delta);
    rabbit->set_pos(idx_cur.y * TILE_SIZE + st_j, idx_cur.x * TILE_SIZE);
    check_win();

    if(is_game_over&&win_anim_can_show)anim_win->on_update(delta);
}

void GameScene::on_render(SDL_Renderer* renderer)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    static SDL_Rect rect_bg_dst = { 0,0,st_j,786 };
    //SDL_RenderCopy(renderer, ResourcesManager::get_instance()->find_texture("880"), NULL, &rect_bg_dst);
	render_menu(renderer);          //菜单
    render_map(renderer,map_cache); //地图
    rabbit->on_render(renderer);    //玩家
    static SDL_Point pos_dst_win = { 150,0};

    if (is_game_over && win_anim_can_show)anim_win->on_render(renderer, pos_dst_win, 0, 15);
}

void GameScene::on_input(const SDL_Event& event)
{
    switch (event.type)
    {
    case SDL_MOUSEBUTTONDOWN:
        is_button_down = true;
        break;
    case SDL_MOUSEBUTTONUP:
        is_button_down = false;
        break;
    case SDL_MOUSEMOTION:
        cursor_pos.x = event.motion.x;
        cursor_pos.y = event.motion.y;
        cur_tile_idx_cursor = { cursor_pos.y / TILE_SIZE,(cursor_pos.x - st_j) / TILE_SIZE };  
        break;
           
    }
}

void GameScene::render_menu(SDL_Renderer* renderer)
{
    SDL_SetRenderDrawColor(renderer,0, 255, 127, 255);
    SDL_RenderClear(renderer);
    static SDL_Rect rect_dst_ini = { 0,0,TILE_SIZE,TILE_SIZE };     //初始位置
    static SDL_Rect rect_dst_need = rect_dst_ini;                   //需要渲染的位置
    static SDL_Rect rect_dst_text_fg = rect_dst_need;               //文字前景位置
    static SDL_Rect rect_dst_text_bg = rect_dst_need;               //文字背景位置
    static SDL_Rect rect_dst_img = { 0,0,TILE_SIZE ,TILE_SIZE };                    //（退出，重开）图标位置
    static std::string tile_def[5] = { u8"禁用格",u8"正常格",u8"已选格",u8"起始格",u8"终点格" };
    const static float text_scale = 2.5f;
    for (int i = 0; i < 5; i++)
    {
        SDL_DestroyTexture(tex_text_background);
		SDL_DestroyTexture(tex_text_foreground);
        tex_text_background = nullptr;
		tex_text_foreground = nullptr;

        std::string str_text= tile_def[i];
		SDL_Surface* surf_text_bg = TTF_RenderUTF8_Blended(ResourcesManager::get_instance()->find_font("ipix"), str_text.c_str(), color_text_bg);
		SDL_Surface* surf_text_fg = TTF_RenderUTF8_Blended(ResourcesManager::get_instance()->find_font("ipix"), str_text.c_str(), color_text_fg);  

        int width_text=surf_text_bg->w, height_text=surf_text_bg->h;
        tex_text_background = SDL_CreateTextureFromSurface(renderer, surf_text_bg);
		tex_text_foreground = SDL_CreateTextureFromSurface(renderer, surf_text_fg);

        rect_dst_need.y = rect_dst_ini.y + i * (96+10);
        std::string tile_name = "tile_" + std::to_string(i - 1);
        SDL_RenderCopy(renderer, ResourcesManager::get_instance()->find_texture(tile_name), NULL, &rect_dst_need);
        rect_dst_text_fg.x = rect_dst_need.x + TILE_SIZE + 10;
        rect_dst_text_fg.y = rect_dst_need.y;
        rect_dst_text_fg.w = width_text * text_scale;
        rect_dst_text_fg.h = height_text * text_scale;
        rect_dst_text_bg= rect_dst_text_fg;
        rect_dst_text_bg.x -= 2;
        rect_dst_text_bg.y += 2;

        SDL_RenderCopy(renderer, tex_text_background, NULL, &rect_dst_text_bg);
        SDL_RenderCopy(renderer, tex_text_foreground, NULL, &rect_dst_text_fg);

        SDL_FreeSurface(surf_text_bg);
        SDL_FreeSurface(surf_text_fg);
    }
    
    rect_dst_img.x = 0;
    rect_dst_img.y = rect_dst_ini.y + 5 * (96 + 10) + 30;

    if (SDL_PointInRect(&cursor_pos, &rect_dst_img))
    {
        SDL_RenderCopy(renderer, ResourcesManager::get_instance()->find_texture("exit_hovered"), nullptr, &rect_dst_img);
    }
    else SDL_RenderCopy(renderer, ResourcesManager::get_instance()->find_texture("exit"), nullptr, &rect_dst_img);
    rect_dst_img.x = rect_dst_img.w + 30;
    if (SDL_PointInRect(&cursor_pos, &rect_dst_img))
    {
        SDL_RenderCopy(renderer, ResourcesManager::get_instance()->find_texture("replay_hovered"), nullptr, &rect_dst_img);
        if (is_button_down && tile_num_owned != 0)
        {
            Mix_PlayChannel(-1, ResourcesManager::get_instance()->find_audio("click"), 0);
            game_restart();
        }

    }
    else SDL_RenderCopy(renderer, ResourcesManager::get_instance()->find_texture("replay"), nullptr, &rect_dst_img);

}

void GameScene::update_tile()
{
    //越界检查
    if (cursor_pos.x < st_j || cur_tile_idx_cursor.x < 0 || cur_tile_idx_cursor.y < 0 || cur_tile_idx_cursor.x >= 8 || cur_tile_idx_cursor.y >= 8)
        return;
	bool is_in_target_tile = (map_cache->m_mp[cur_tile_idx_cursor.x][cur_tile_idx_cursor.y] == (int)TileType::Idle)
		|| (map_cache->m_mp[cur_tile_idx_cursor.x][cur_tile_idx_cursor.y] == (int)TileType::End);

	if (is_in_target_tile && is_button_down)
	{
		//检查与当前游标所在的格子与当前角色所在的格子的曼哈顿距离是否为1
        int manhaton_distance = abs(idx_cur.x - cur_tile_idx_cursor.x) + abs(idx_cur.y - cur_tile_idx_cursor.y);
        if (manhaton_distance == 1)
        {
            //更新缓存地图和角色位置
            idx_cur = cur_tile_idx_cursor;
            if (map_cache->m_mp[idx_cur.x][idx_cur.y] != (int)TileType::End)
            {
                map_cache->m_mp[idx_cur.x][idx_cur.y] = (int)TileType::Selected;
                tile_num_owned++;
            }
        }
	}
}