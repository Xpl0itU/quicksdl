#include "GameManager.h"
#ifdef __WIIU__
#include <romfs-wiiu.h>
#include "StateUtils.h"
#endif

int main(int argc, char* argv[]) {

#ifdef __WIIU__
	romfsInit();
	State::init();
#endif

	QuickSDL::GameManager* game = QuickSDL::GameManager::Instance();

	game->Run();

	QuickSDL::GameManager::Release();
	game = NULL;

#ifdef __WIIU__
	romfsExit();
	State::shutdown();
#endif

	return 0;
}