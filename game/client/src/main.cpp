#include "Game.h"

#include <thread>

#include "network/Common.h"

// can't just use main() because SDL2main will look for the function with these parameters specifically
int main(int argc, char* argv[]) {
	network::initENet();
	Game* game = new Game();
	int result = game->run();
	return result;
}