#include <network/Common.h>

#include "GameClient.h"

// can't just use main() because SDL2main will look for the function with these parameters specifically
int main(int argc, char* argv[]) {
	network::initENet();
	GameClient client;
	int result = client.run();
	return result;
}