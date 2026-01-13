#include"game_scene.h"

void GameScene::on_enter()
{
    SDL_Log("Enter Game Scene");
	is_button_down = false;

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
            if (tile_type == (int)TileType::End)
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
    editor_map = new Map("map.csv");
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            if (editor_map->m_mp[i][j] == (int)TileType::Start)
            {
                idx_cur.x = i;
                idx_cur.y = j;
                std::cout << idx_cur.x << "," << idx_cur.y << std::endl;
                break;
            }
        }
    }
}

GameScene::~GameScene()
{
    delete editor_map;
    delete rabbit;
}

void GameScene::on_update(float delta)
{
    rabbit->on_update(delta);
    rabbit->set_pos(idx_cur.x * TILE_SIZE + st_j, idx_cur.y * TILE_SIZE);
}

void GameScene::on_render(SDL_Renderer* renderer)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    static SDL_Rect rect_bg_dst = { 0,0,st_j,786 };
    //SDL_RenderCopy(renderer, ResourcesManager::get_instance()->find_texture("880"), NULL, &rect_bg_dst);
	render_menu(renderer);          //菜单
    render_map(renderer,editor_map); //地图
    rabbit->on_render(renderer);    //玩家

}

void GameScene::on_input(const SDL_Event& event)
{
    switch (event.type)
    {
    case SDL_MOUSEBUTTONDOWN:
        is_button_down = true;
        break;
    case SDL_MOUSEMOTION:
        if (is_button_down)
        {
            cursor_pos.x = event.motion.x;
            cursor_pos.y = event.motion.y;
        }
        break;

    }
}

void GameScene::render_menu(SDL_Renderer* renderer)
{
    static SDL_Rect rect_dst_ini = { 0,0,TILE_SIZE,TILE_SIZE };
    static SDL_Rect rect_dst_need = rect_dst_ini;
    static SDL_Rect rect_dst_text_fg = rect_dst_need;
    static SDL_Rect rect_dst_text_bg = rect_dst_need;
    static std::string tile_def[5] = { u8"禁用格",u8"正常格",u8"已选格",u8"起始格",u8"终止格" };
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
}
