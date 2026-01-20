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
    ConfigManager::get_instance()->is_infinite = false;
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
             
            if (ConfigManager::get_instance()->is_infinite)
            {
                anim_win->reset(); 
                generate_random_map();
                game_restart();
            }
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
            Mix_PlayChannel(-1, ResourcesManager::get_instance()->find_audio("click"), 0);
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
            Mix_PlayChannel(-1, ResourcesManager::get_instance()->find_audio("click"), 0);
            import_map();
            game_restart();
        }
    }
    else SDL_RenderCopy(renderer, ResourcesManager::get_instance()->find_texture("in"), nullptr, &rect_dst_img);   
    rect_dst_img.x = rect_dst_img.w + 30;
    if (SDL_PointInRect(&ConfigManager::get_instance()->pos_cursor, &rect_dst_img))
    {
        SDL_RenderCopy(renderer, ResourcesManager::get_instance()->find_texture("out_hovered"), nullptr, &rect_dst_img);
        if (is_button_down)
        {
            Mix_PlayChannel(-1, ResourcesManager::get_instance()->find_audio("click"), 0);
            export_map();
        }
    }
    else SDL_RenderCopy(renderer, ResourcesManager::get_instance()->find_texture("out"), nullptr, &rect_dst_img);
      
    

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

void GameScene::export_map()
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
                if(map_cache->m_mp[i][j] == (int)TileType::Selected)
                    csvFile << (int)TileType::Idle; // 将选中格转换为空闲格
                else csvFile << map_cache->m_mp[i][j]; // 写入当前格子值
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


// 尝试生成一条单一路径的地图
bool generate_single_path_map(int maze[8][8], std::mt19937& gen)
{
    std::uniform_int_distribution<> dis(0, 7);
    std::uniform_int_distribution<> percent_dis(0, 99);

    // 1. 随机选择起点和终点（确保它们在不同位置）
    SDL_Point start, end;

    // 使用4种固定模式，确保距离足够
    int pattern = percent_dis(gen) % 4;
    switch (pattern) {
    case 0: start = { 0, 0 }; end = { 7, 7 }; break;
    case 1: start = { 0, 7 }; end = { 7, 0 }; break;
    case 2: start = { 1, 1 }; end = { 6, 6 }; break;
    case 3: start = { 1, 6 }; end = { 6, 1 }; break;
    }

    // 2. 初始化地图
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            maze[i][j] = -1; // 全部设为禁用格
        }
    }

    maze[start.x][start.y] = (int)TileType::Start;
    maze[end.x][end.y] = (int)TileType::End;

    // 3. 生成路径
    std::vector<SDL_Point> path;
    path.push_back(start);

    SDL_Point current = start;
    bool visited[8][8] = { false };
    visited[start.x][start.y] = true;

    int max_steps = 30; // 最大步数限制
    int step = 0;

    while ((current.x != end.x || current.y != end.y) && step < max_steps) {
        step++;

        // 获取所有可能的移动方向
        std::vector<SDL_Point> possible_moves;
        SDL_Point moves[4] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };

        for (const auto& move : moves) {
            SDL_Point next = { current.x + move.x, current.y + move.y };

            // 检查边界
            if (next.x < 0 || next.x >= 8 || next.y < 0 || next.y >= 8) {
                continue;
            }

            // 检查是否已访问
            if (visited[next.x][next.y]) {
                continue;
            }

            // 检查格子类型：只能移动到禁用格或终点
            if (maze[next.x][next.y] == -1 || maze[next.x][next.y] == (int)TileType::End) {
                possible_moves.push_back(next);
            }
        }

        // 如果没有可能的移动，路径生成失败
        if (possible_moves.empty()) {
            // 尝试回溯一步（最多回溯3步）
            bool backtrack_success = false;
            for (int backtrack = 0; backtrack < 3 && path.size() > 1; ++backtrack) {
                // 移除最后一步
                path.pop_back();
                SDL_Point prev = path.back();

                // 将当前位置恢复为禁用格
                if (!(current.x == start.x && current.y == start.y) &&
                    !(current.x == end.x && current.y == end.y)) {
                    maze[current.x][current.y] = -1;
                    visited[current.x][current.y] = false;
                }

                current = prev;

                // 重新检查可能的移动
                possible_moves.clear();
                for (const auto& move : moves) {
                    SDL_Point next = { current.x + move.x, current.y + move.y };

                    if (next.x < 0 || next.x >= 8 || next.y < 0 || next.y >= 8) continue;
                    if (visited[next.x][next.y]) continue;
                    if (maze[next.x][next.y] == -1 || maze[next.x][next.y] == (int)TileType::End) {
                        possible_moves.push_back(next);
                    }
                }

                if (!possible_moves.empty()) {
                    backtrack_success = true;
                    break;
                }
            }

            if (!backtrack_success) {
                return false; // 生成失败
            }
        }

        // 选择下一个移动
        SDL_Point next;

        // 计算到终点的方向
        int dx = end.x - current.x;
        int dy = end.y - current.y;

        // 创建一个偏好列表
        std::vector<SDL_Point> preferred_moves;
        std::vector<SDL_Point> other_moves;

        for (const auto& move : possible_moves) {
            // 判断是否朝向终点
            bool toward_end = false;
            if (dx > 0 && move.x > current.x) toward_end = true;
            if (dx < 0 && move.x < current.x) toward_end = true;
            if (dy > 0 && move.y > current.y) toward_end = true;
            if (dy < 0 && move.y < current.y) toward_end = true;

            if (toward_end) {
                preferred_moves.push_back(move);
            }
            else {
                other_moves.push_back(move);
            }
        }

        // 50%概率选择朝向终点的方向，50%概率选择其他方向
        if (!preferred_moves.empty() && percent_dis(gen) % 100 < ConfigManager::get_instance()->ratio_back) {
            std::uniform_int_distribution<> pref_dis(0, preferred_moves.size() - 1);
            next = preferred_moves[pref_dis(gen)];
        }
        else if (!other_moves.empty()) {
            std::uniform_int_distribution<> other_dis(0, other_moves.size() - 1);
            next = other_moves[other_dis(gen)];
        }
        else {
            std::uniform_int_distribution<> move_dis(0, possible_moves.size() - 1);
            next = possible_moves[move_dis(gen)];
        }

        // 移动到下一个位置
        current = next;
        visited[current.x][current.y] = true;
        path.push_back(current);

        // 将路径上的格子设为正常格（除了起点和终点）
        if (!(current.x == start.x && current.y == start.y) &&
            !(current.x == end.x && current.y == end.y)) {
            maze[current.x][current.y] = (int)TileType::Idle;
        }
    }

    // 检查是否到达终点
    if (current.x != end.x || current.y != end.y) {
        return false; // 没有到达终点
    }

    // 检查路径长度是否合理（至少10个Idle格子）
    int idle_count = 0;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (maze[i][j] == (int)TileType::Idle) {
                idle_count++;
            }
        }
    }

    if (idle_count < 8) {
        return false; // 路径太短
    }

    return true; // 生成成功
}

// 生成保底地图（永远不会失败）
void generate_fallback_map(int maze[8][8])
{
    // 使用一个固定的、永远不会失败的蛇形路径
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            maze[i][j] = -1; // 全部设为禁用格
        }
    }

    // 起点在(0,0)，终点在(7,7)
    maze[0][0] = (int)TileType::Start;
    maze[7][7] = (int)TileType::End;

    // 生成一个复杂的蛇形路径
    // 第一行：从(0,0)到(0,6)
    for (int j = 1; j <= 6; ++j) {
        maze[0][j] = (int)TileType::Idle;
    }

    // 向下：(0,6)到(6,6)
    for (int i = 1; i <= 6; ++i) {
        maze[i][6] = (int)TileType::Idle;
    }

    // 向左：(6,6)到(6,1)
    for (int j = 5; j >= 1; --j) {
        maze[6][j] = (int)TileType::Idle;
    }

    // 向上：(6,1)到(1,1)
    for (int i = 5; i >= 1; --i) {
        maze[i][1] = (int)TileType::Idle;
    }

    // 向右：(1,1)到(1,5)
    for (int j = 2; j <= 5; ++j) {
        maze[1][j] = (int)TileType::Idle;
    }

    // 向下：(1,5)到(5,5)
    for (int i = 2; i <= 5; ++i) {
        maze[i][5] = (int)TileType::Idle;
    }

    // 向左：(5,5)到(5,2)
    for (int j = 4; j >= 2; --j) {
        maze[5][j] = (int)TileType::Idle;
    }

    // 向上：(5,2)到(2,2)
    for (int i = 4; i >= 2; --i) {
        maze[i][2] = (int)TileType::Idle;
    }

    // 向右：(2,2)到(2,4)
    for (int j = 3; j <= 4; ++j) {
        maze[2][j] = (int)TileType::Idle;
    }

    // 向下：(2,4)到(4,4)
    for (int i = 3; i <= 4; ++i) {
        maze[i][4] = (int)TileType::Idle;
    }

    // 向左：(4,4)到(4,3)
    maze[4][3] = (int)TileType::Idle;

    // 向下：(4,3)到(7,3)
    for (int i = 5; i <= 7; ++i) {
        maze[i][3] = (int)TileType::Idle;
    }

    // 向右：(7,3)到(7,7)
    for (int j = 4; j <= 7; ++j) {
        if (maze[7][j] != (int)TileType::End) {
            maze[7][j] = (int)TileType::Idle;
        }
    }
}

// 更简单的保底地图（如果上面的太复杂）
void generate_simple_fallback_map(int maze[8][8])
{
    // 最简单的L形路径
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            maze[i][j] = -1;
        }
    }

    maze[0][0] = (int)TileType::Start;
    maze[7][7] = (int)TileType::End;

    // 向右走7步
    for (int j = 1; j <= 7; ++j) {
        maze[0][j] = (int)TileType::Idle;
    }

    // 向下走6步（跳过已经设置为Idle的(0,7)）
    for (int i = 1; i <= 6; ++i) {
        maze[i][7] = (int)TileType::Idle;
    }
}

void GameScene::generate_random_map()
{
    // 创建随机数生成器
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 7);
    std::uniform_int_distribution<> percent_dis(0, 99);

    // 1. 先预设一个简单的地图作为保底
    int temp_map[8][8];

    // 2. 使用多次尝试的方法，如果失败则使用保底方案
    bool success = false;
    int max_attempts = 10; // 最多尝试10次

    for (int attempt = 0; attempt < max_attempts && !success; ++attempt) {
        success = generate_single_path_map(temp_map, gen);
        if (success) {
            SDL_Log(u8"第%d次尝试生成地图成功", attempt + 1);
        }
    }

    // 3. 如果所有尝试都失败，使用保底方案
    if (!success) {
        SDL_Log(u8"多次尝试失败，使用保底地图");
        generate_fallback_map(temp_map);
    }

    // 4. 更新地图对象
    delete map_ini;
    delete map_cache;

    map_ini = new Map();
    map_cache = new Map();

    // 复制地图数据
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            map_ini->m_mp[i][j] = temp_map[i][j];
            map_cache->m_mp[i][j] = temp_map[i][j];
        }
    }

    // 5. 重新计算需要走过的格子数和起点位置
    tile_num_needed = 0;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (map_ini->m_mp[i][j] == (int)TileType::Start) {
                idx_cur.x = i;
                idx_cur.y = j;
                SDL_Log(u8"地图起点: (%d, %d)", idx_cur.x, idx_cur.y);
            }
            else if (map_ini->m_mp[i][j] == (int)TileType::Idle) {
                tile_num_needed++;
            }
        }
    }

    SDL_Log(u8"地图生成成功！需要走过的格子数: %d", tile_num_needed);
}


