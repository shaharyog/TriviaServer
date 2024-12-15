#pragma once

#include <string>
#include <kissnet.hpp>
#include <utility>
#include <map>
#include <mutex>
#include <shared_mutex>
#include "../../requestHandlers/IRequestHandler.h"
#include "../../errors/error.h"
#include "../../requestHandlers/requestHandlerFactory.h"

class Communicator {
public:
    explicit Communicator(Endpoint  server_endpoint, RequestHandlerFactory &requestHandlerFactory) :
            _server_endpoint(std::move(server_endpoint)),
            _requestHandlerFactory(requestHandlerFactory) {}

    ~Communicator();

    void startHandleRequests();

private:
    // SocketHelper method
    void bindAndListen();

    void handleClient(const std::string &client_uuid, const Endpoint &client_endpoint);

    std::string generateUUID();

    // server related members
    Endpoint _server_endpoint;
    kissnet::tcp_socket _server_socket;
    RequestHandlerFactory &_requestHandlerFactory;

    // clients related members
    // map of clients: UUID -> (socket, handler)
    std::map<std::string, std::pair<kissnet::tcp_socket, std::unique_ptr<IRequestHandler>>> _clients;
    mutable std::shared_mutex _clientsMutex;
};