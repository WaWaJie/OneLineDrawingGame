#define SDL_MAIN HANDLED

#include"application.h"

int main(int argc, char* argv[]) 
{
	Application::get_instance()->on_run();

    return 0;
}
