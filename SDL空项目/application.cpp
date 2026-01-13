#include"application.h"

Application::Application()
	:width(1068), height(768), is_quit(false)
{
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_PNG|IMG_INIT_JPG);
    Mix_Init(MIX_INIT_MP3);
    TTF_Init();

    SetProcessDPIAware();

    window = SDL_CreateWindow(u8"一笔画", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_ShowCursor(SDL_DISABLE);
       
    ResourcesManager::get_instance()->load(renderer);
   

}

Application::~Application()
{

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    Mix_Quit();
    TTF_Quit();
    SDL_Quit();
}

void Application::on_run()
{
    //获取时间
    Uint64 last_counter = SDL_GetPerformanceCounter();
    Uint64 counter_freq = SDL_GetPerformanceFrequency();

    while (!is_quit)
    {
        while (SDL_PollEvent(&event))
			on_input(event);
        //逻辑更新
        {
            Uint64 current_counter = SDL_GetPerformanceCounter();
            float delta=(float)(current_counter - last_counter) / counter_freq;
            last_counter = current_counter;

			on_update(delta);

            if (delta * 1000 < 1000.0 / fps)
                SDL_Delay((1000.0 / fps) - delta * 1000);

        }
        //画面渲染
        on_render();
    }

}

void Application::on_update(float delta)
{
	SceneManager::get_instance()->on_update(delta);

}

void Application::on_render()
{

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

	SceneManager::get_instance()->on_render(renderer);
    SDL_RenderCopy(renderer, ResourcesManager::get_instance()->find_texture("crosshair"), NULL, &rect_cursor);  //准星

    SDL_RenderPresent(renderer);

}

void Application::on_input(const SDL_Event& event)
{
    switch (event.type)
    {
    case SDL_QUIT:
        is_quit = true;
        break;
    case SDL_MOUSEMOTION:
        rect_cursor.x = event.motion.x - rect_cursor.w / 2;
        rect_cursor.y = event.motion.y - rect_cursor.h / 2;
        break;
    }
}