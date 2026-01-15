#include"game_scene.h"
//cout


void GameScene::on_enter()
{

    SDL_Log("Enter Game Scene");
	is_button_down = false;
    tile_num_owned = 0;
    is_game_over = false;
    win_anim_can_show = true;
    anim_win->reset();
    game_restart();
}  

void GameScene::on_exit()
{
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
            if (tile_type == (int)TileType::End && !is_game_over)
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
    delete map_cache;
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
        ConfigManager::get_instance()->pos_cursor.x = event.motion.x;
        ConfigManager::get_instance()->pos_cursor.y = event.motion.y;
        cur_tile_idx_cursor = { ConfigManager::get_instance()->pos_cursor.y / TILE_SIZE,(ConfigManager::get_instance()->pos_cursor.x - st_j) / TILE_SIZE };
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
    TTF_SetFontSize(ResourcesManager::get_instance()->find_font("ipix"), 40);
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
        rect_dst_text_fg.w = width_text ;
        rect_dst_text_fg.h = height_text ;
        rect_dst_text_bg= rect_dst_text_fg;
        rect_dst_text_bg.x -= 2;
        rect_dst_text_bg.y += 2;
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

        SDL_RenderCopy(renderer, tex_text_background, NULL, &rect_dst_text_bg);
        SDL_RenderCopy(renderer, tex_text_foreground, NULL, &rect_dst_text_fg);

        SDL_FreeSurface(surf_text_bg);
        SDL_FreeSurface(surf_text_fg);
    }
    
    rect_dst_img.x = 0;
    rect_dst_img.y = rect_dst_ini.y + 5 * (96 + 10) + 30;

    if (SDL_PointInRect(&ConfigManager::get_instance()->pos_cursor, &rect_dst_img))
    {
        SDL_RenderCopy(renderer, ResourcesManager::get_instance()->find_texture("exit_hovered"), nullptr, &rect_dst_img);
        if (is_button_down)
        {
            ConfigManager::get_instance()->current_scene_type = SceneType::Menu;
        }
    }
    else SDL_RenderCopy(renderer, ResourcesManager::get_instance()->find_texture("exit"), nullptr, &rect_dst_img);
    rect_dst_img.x = rect_dst_img.w + 30;
    if (SDL_PointInRect(&ConfigManager::get_instance()->pos_cursor, &rect_dst_img))
    {
        SDL_RenderCopy(renderer, ResourcesManager::get_instance()->find_texture("replay_hovered"), nullptr, &rect_dst_img);
        if (is_button_down && tile_num_owned != 0)
        {
            Mix_PlayChannel(-1, ResourcesManager::get_instance()->find_audio("click"), 0);
            game_restart();
        }

    }
    else SDL_RenderCopy(renderer, ResourcesManager::get_instance()->find_texture("replay"), nullptr, &rect_dst_img);
    rect_dst_img.x = 0;
    rect_dst_img.y += 100;
    if (SDL_PointInRect(&ConfigManager::get_instance()->pos_cursor, &rect_dst_img))
    {
        SDL_RenderCopy(renderer, ResourcesManager::get_instance()->find_texture("in_hovered"), nullptr, &rect_dst_img);
        if (is_button_down)
        {
            import_map();
            game_restart();
        }
    }
    else SDL_RenderCopy(renderer, ResourcesManager::get_instance()->find_texture("in"), nullptr, &rect_dst_img);   

}

void GameScene::update_tile()
{
    //越界检查
    if (ConfigManager::get_instance()->pos_cursor.x < st_j || cur_tile_idx_cursor.x < 0 || cur_tile_idx_cursor.y < 0 || cur_tile_idx_cursor.x >= 8 || cur_tile_idx_cursor.y >= 8)
        return;
	bool is_in_target_tile = (map_cache->m_mp[cur_tile_idx_cursor.x][cur_tile_idx_cursor.y] == (int)TileType::Idle)
		|| (map_cache->m_mp[cur_tile_idx_cursor.x][cur_tile_idx_cursor.y] == (int)TileType::End);

	if (is_in_target_tile && is_button_down)
	{
		//检查与当前游标所在的格子与当前角色所在的格子的曼哈顿距离是否为1
        int manhaton_distance = abs(idx_cur.x - cur_tile_idx_cursor.x) + abs(idx_cur.y - cur_tile_idx_cursor.y);
        if (manhaton_distance == 1)
        {
            idx_cur = cur_tile_idx_cursor;
            //更新缓存地图和角色位置
            if (map_cache->m_mp[idx_cur.x][idx_cur.y] != (int)TileType::End)
            {
                map_cache->m_mp[idx_cur.x][idx_cur.y] = (int)TileType::Selected;
                tile_num_owned++;
            }
        }
	}  
}

// game_scene.cpp - 添加在文件末尾

void GameScene::import_map()
{
    // 1. 初始化Windows打开文件对话框
    OPENFILENAME ofn = { 0 };
    wchar_t szOpenPath[MAX_PATH] = L""; // 存储用户选择的路径（宽字符）

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = NULL; // 若有SDL窗口句柄，可填 SDL_GetWindowFromRenderer(renderer) 关联窗口
    ofn.lpstrFilter = L"CSV文件 (*.csv)\0*.csv\0所有文件 (*.*)\0*.*\0"; // 文件筛选器
    ofn.lpstrFile = szOpenPath;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY; // 文件必须存在
    ofn.lpstrTitle = L"选择地图文件"; // 对话框标题

    // 2. 弹出打开对话框，用户选择文件
    if (GetOpenFileName(&ofn))
    {
        // 宽字符转多字节（适配C++文件操作）
        char openPath[MAX_PATH] = { 0 };
        WideCharToMultiByte(CP_ACP, 0, szOpenPath, -1, openPath, MAX_PATH, NULL, NULL);

        SDL_Log(u8"导入地图文件：%s", openPath);

        try
        {
            // 3. 创建新的地图对象
            Map* newMap = new Map(openPath);

            // 4. 检查地图有效性（8x8）
            bool validMap = true;
            int startCount = 0;
            int endCount = 0;

            // 验证地图数据
            for (int i = 0; i < 8; ++i)
            {
                for (int j = 0; j < 8; ++j)
                {
                    int tileValue = newMap->m_mp[i][j];

                    // 检查值范围
                    if (tileValue < -1 || tileValue > 3)
                    {
                        validMap = false;
                        SDL_Log(u8"错误：地图数据超出范围 (%d, %d): %d", i, j, tileValue);
                    }

                    // 统计起点和终点
                    if (tileValue == (int)TileType::Start)
                        startCount++;
                    else if (tileValue == (int)TileType::End)
                        endCount++;
                }
            }

            // 检查起点和终点数量
            if (startCount != 1 || endCount != 1)
            {
                validMap = false;
                SDL_Log(u8"错误：地图必须有且只有一个起点和终点。起点数：%d，终点数：%d", startCount, endCount);
            }

            if (validMap)
            {
                // 5. 删除旧地图，使用新地图
                delete map_ini;
                delete map_cache;

                map_ini = new Map(openPath);
                map_cache = new Map(openPath);

                // 6. 重置游戏状态
                tile_num_needed = 0;
                tile_num_owned = 0;

                // 重新计算需要走过的格子数和起点位置
                for (int i = 0; i < 8; ++i)
                {
                    for (int j = 0; j < 8; ++j)
                    {
                        if (map_ini->m_mp[i][j] == (int)TileType::Start)
                        {
                            idx_cur.x = i;
                            idx_cur.y = j;
                            SDL_Log(u8"新起点位置: %d, %d", idx_cur.x, idx_cur.y);
                        }
                        else if (map_ini->m_mp[i][j] == (int)TileType::Idle)
                        {
                            tile_num_needed++;
                        }
                    }
                }

                // 7. 重置游戏
                game_restart();

                SDL_Log(u8"地图导入成功！需要走过的格子数：%d", tile_num_needed);
            }
            else
            {
                // 无效地图，删除临时对象
                delete newMap;
                SDL_Log(u8"导入失败：地图文件格式无效");

                // 可选：显示错误消息框
                MessageBox(NULL,
                    L"地图文件格式无效！\n请确保：\n1. 地图为8x8格\n2. 数值范围：-1到3\n3. 有且只有一个起点(2)和终点(3)",
                    L"导入错误",
                    MB_OK | MB_ICONERROR);
            }
        }
        catch (const std::exception& e)
        {
            SDL_Log(u8"导入失败：读取文件时发生错误 - %s", e.what());
             
            // 可选：显示错误消息框
            MessageBox(NULL,
                L"读取文件时发生错误！\n请确保文件格式正确且未被占用。",
                L"导入错误",
                MB_OK | MB_ICONERROR);
        }
    }
    else
    {
        SDL_Log(u8"用户取消了导入操作");
    }
}