#include "common/Network.h"
#include "logging/Logger.h"
#include "network/Server.h"

static constexpr int MAX_NUM_CLIENTS{ 32 };
static constexpr int READ_INTERVAL_MILLIS { 20 };

int main(int argc, char* argv[])
{
	network::initENet();
	network::Server server(common::PORT_NUMBER, MAX_NUM_CLIENTS);
	server.run(READ_INTERVAL_MILLIS);
}