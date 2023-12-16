#pragma once

#include <common/Network.h>
#include <network/Server.h>

class Server
{
public:
    Server();

public:
    void run();

private:
    network::Server<> _server;
};