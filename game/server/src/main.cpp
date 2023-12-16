#include <thread>

#include "Game.h"
#include "Server.h"

int main(int argc, char* argv[])
{
	network::initENet();
	Server server;
	server.run();
}