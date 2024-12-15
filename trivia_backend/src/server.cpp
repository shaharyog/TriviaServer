#include "server.h"
#include <iostream>
#include <thread>

void Server::run()
{
    logServerProgress<Server>( __func__, "Starting server on (" + _server_endpoint.toString() + ")...");
    logServerResult(true);

    logServerProgress<Server>( __func__, "Starting Communicator thread...");
    std::thread t_communicator(&Communicator::startHandleRequests, &_communicator);
    t_communicator.detach();
    logServerResult(true);

    std::string input;
    do {
        std::getline(std::cin, input);
        if (input == "clear" || input == "CLEAR")
            std::cout << "\033[2J\033[1;1H";

    } while (input != "EXIT" && input != "exit");

    log<Server>(__func__, "Shutting down server...", true, _server_endpoint);
}
