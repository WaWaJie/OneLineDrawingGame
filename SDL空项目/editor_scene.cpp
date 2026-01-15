#include"editor_scene.h"
//cout


void EditorScene::on_enter()
{

    SDL_Log("Enter Game Scene");
    is_button_down = false;
    game_restart();
}

void EditorScene::on_exit()
{
}

void EditorScene::render_map(SDL_Renderer* renderer, Map* mp)
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
            if (tile_type == (int)TileType::End)
            {
                static SDL_Rect rect_src_target = { 0,0,48,48 };
                SDL_RenderCopy(renderer, ResourcesManager::get_instance()->find_texture("target"), &rect_src_target, &dst_rect);
            }
        }
    }
}

EditorScene::EditorScene()
{
    rabbit = new Rabbit();
    map_cache = new Map("map.csv");

    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            map_cache->m_mp[i][j] = -1;
        }
    }
}

EditorScene::~EditorScene()
{
    delete map_cache;
    delete rabbit;
}

void EditorScene::on_update(float delta)
{
    update_tile();
    rabbit->on_update(delta);
    rabbit->set_pos(idx_cur.y * TILE_SIZE + st_j, idx_cur.x * TILE_SIZE);
}

void EditorScene::on_render(SDL_Renderer* renderer)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    static SDL_Rect rect_bg_dst = { 0,0,st_j,786 };
    //SDL_RenderCopy(renderer, ResourcesManager::get_instance()->find_texture("880"), NULL, &rect_bg_dst);
    render_menu(renderer);          //菜单
    render_map(renderer, map_cache); //地图
    if (!path_list.empty())rabbit->on_render(renderer);    //玩家
    static SDL_Point pos_dst_win = { 150,0 };

}

void EditorScene::on_input(const SDL_Event& event)
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

void EditorScene::render_menu(SDL_Renderer* renderer)
{
    SDL_SetRenderDrawColor(renderer, 0, 255, 127, 255);
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

        std::string str_text = tile_def[i];
        SDL_Surface* surf_text_bg = TTF_RenderUTF8_Blended(ResourcesManager::get_instance()->find_font("ipix"), str_text.c_str(), color_text_bg);
        SDL_Surface* surf_text_fg = TTF_RenderUTF8_Blended(ResourcesManager::get_instance()->find_font("ipix"), str_text.c_str(), color_text_fg);

        int width_text = surf_text_bg->w, height_text = surf_text_bg->h;
        tex_text_background = SDL_CreateTextureFromSurface(renderer, surf_text_bg);
        tex_text_foreground = SDL_CreateTextureFromSurface(renderer, surf_text_fg);

        rect_dst_need.y = rect_dst_ini.y + i * (96 + 10);
        std::string tile_name = "tile_" + std::to_string(i - 1);
        SDL_RenderCopy(renderer, ResourcesManager::get_instance()->find_texture(tile_name), NULL, &rect_dst_need);
        rect_dst_text_fg.x = rect_dst_need.x + TILE_SIZE + 10;
        rect_dst_text_fg.y = rect_dst_need.y;
        rect_dst_text_fg.w = width_text;
        rect_dst_text_fg.h = height_text;
        rect_dst_text_bg = rect_dst_text_fg;
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
        if (is_button_down && !path_list.empty())
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
        SDL_RenderCopy(renderer, ResourcesManager::get_instance()->find_texture("out_hovered"), nullptr, &rect_dst_img);
        if (is_button_down)
        {
            export_map();
        }
    }
    else SDL_RenderCopy(renderer,ResourcesManager::get_instance()->find_texture("out"), nullptr, &rect_dst_img);
}

void EditorScene::update_tile()
{
    //越界检查
    if (ConfigManager::get_instance()->pos_cursor.x < st_j || cur_tile_idx_cursor.x < 0 || cur_tile_idx_cursor.y < 0 || cur_tile_idx_cursor.x >= 8 || cur_tile_idx_cursor.y >= 8)
        return;

    if (is_button_down&&path_list.empty())
    {
        idx_cur = cur_tile_idx_cursor;
        path_list.push_back(idx_cur);
        map_cache->m_mp[cur_tile_idx_cursor.x][cur_tile_idx_cursor.y] = (int)TileType::Idle;
        use_path_info();
        return;
    } 

    bool is_in_target_tile = (map_cache->m_mp[cur_tile_idx_cursor.x][cur_tile_idx_cursor.y] == (int)TileType::Failed);

    if (is_in_target_tile && is_button_down)
    {
        //检查与当前游标所在的格子与当前角色所在的格子的曼哈顿距离是否为1
        int manhaton_distance = abs(idx_cur.x - cur_tile_idx_cursor.x) + abs(idx_cur.y - cur_tile_idx_cursor.y);
        if (manhaton_distance == 1)
        {
            idx_cur = cur_tile_idx_cursor;
            map_cache->m_mp[idx_cur.x][idx_cur.y] = (int)TileType::Idle;
            path_list.push_back(idx_cur);
            use_path_info();
        }
    }
}

void EditorScene::export_map()
{
    // 1. 初始化Windows保存文件对话框
    OPENFILENAME ofn = { 0 };
    wchar_t szSavePath[MAX_PATH] = L""; // 存储用户选择的路径（宽字符）

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = NULL; // 若有SDL窗口句柄，可填 SDL_GetWindowFromRenderer(renderer) 关联窗口
    ofn.lpstrFilter = L"CSV文件 (*.csv)\0*.csv\0所有文件 (*.*)\0*.*\0"; // 文件筛选器
    ofn.lpstrFile = szSavePath;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrDefExt = L"csv"; // 默认扩展名
    ofn.Flags = OFN_EXPLORER | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT; // 覆盖提示、隐藏只读
    ofn.lpstrTitle = L"导出地图为CSV"; // 对话框标题

    // 2. 弹出保存对话框，用户选择路径后写入文件
    if (GetSaveFileName(&ofn))
    {
        // 宽字符转多字节（适配C++文件操作）
        char savePath[MAX_PATH] = { 0 };
        WideCharToMultiByte(CP_ACP, 0, szSavePath, -1, savePath, MAX_PATH, NULL, NULL);

        // 3. 打开文件并写入8x8地图数据
        std::ofstream csvFile(savePath);
        if (!csvFile.is_open())
        {
            SDL_Log(u8"导出失败：无法打开文件 %s", savePath);
            return;
        }

        // 遍历map_cache的8x8数组，按行写入CSV
        for (int i = 0; i < 8; ++i)
        {
            for (int j = 0; j < 8; ++j)
            {
                csvFile << map_cache->m_mp[i][j]; // 写入当前格子值
                if (j != 7) csvFile << ",";       // 最后一列不加逗号
            }
            csvFile << std::endl; // 每行结束换行
        }

        csvFile.close();
        SDL_Log(u8"地图导出成功！路径：%s", savePath);
    }
    else
    {
        SDL_Log(u8"用户取消了导出操作");
    }
}

