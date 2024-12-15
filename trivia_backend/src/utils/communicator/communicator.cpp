#include <iostream>
#include "communicator.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <thread>
#include <utility>
#include "../socketHelper/socketHelper.h"


Communicator::~Communicator() {
    _server_socket.close();
}


void Communicator::startHandleRequests() {
    bindAndListen();

    while (true) {
        kissnet::tcp_socket client_socket;

        try {
            client_socket = _server_socket.accept();
        }
        catch (const std::exception &e) {
            logError<Communicator>(__func__, "Failed to accept client connection", _server_endpoint);
            continue;
        }

        const auto client_uuid = generateUUID();
        const auto userEndpoint = Endpoint(client_socket);
        log<Communicator>( __func__, "Accepted new client, generated UUID: " + client_uuid , true, userEndpoint);

        std::unique_ptr<IRequestHandler> request_handler = _requestHandlerFactory.createLoginRequestHandler(userEndpoint);

        {
            std::lock_guard lock(_clientsMutex);
            _clients[client_uuid] = {std::move(client_socket), std::move(request_handler)};
        }

        std::thread client_thread = std::thread(&Communicator::handleClient, this, client_uuid, userEndpoint);
        client_thread.detach();
    }
}

void Communicator::bindAndListen() {
    // create server socket
    try {
        logServerProgress<Communicator>( __func__, "Creating server socket...");
        _server_socket = kissnet::tcp_socket(_server_endpoint);
        logServerResult(true);
    }
    catch (const std::exception &e) {
        logServerResult(false, true);
    }

    // bind server socket
    try {
        logServerProgress<Communicator>( __func__, "Binding server socket...");
        _server_socket.bind();
        logServerResult(true);
    }
    catch (const std::exception &e) {
        logServerResult(false, true);
    }

    // listen server socket
    try {
        logServerProgress<Communicator>( __func__, "Listening for client connections on (" + _server_endpoint.address + ", " + std::to_string(_server_endpoint.port) + ")...");
        _server_socket.listen();
        logServerResult(true);
    } catch (const std::exception &e) {
        logServerResult(false, true);
    }
}

void Communicator::handleClient(const std::string &client_uuid, const Endpoint &userEndpoint) {
    std::shared_lock sharedLock(_clientsMutex);
    auto &[client_socket, request_handler] = _clients[client_uuid];
    sharedLock.unlock();

    RequestInfo reqInfo;
    RequestResult reqResult;
    do {
        reqInfo = SocketHelper::getRequestInfo(client_socket);
        reqResult = request_handler->handleRequest(reqInfo);

        // Update request handler if necessary
        if (reqResult.newHandler != nullptr) {
            request_handler = std::move(reqResult.newHandler);
        }

        // if we handled a request that is not EXIT, send the response to the client
        if (reqInfo.requestId != RequestId::EXIT) {
            const auto res = SocketHelper::sendData(client_socket, reqResult.buffer);

            // if we couldn't send data, the client has disconnected, so pass exit request to the handler
            // to start the chain of logout
            if (res.has_value()) {
                request_handler->handleRequest({RequestId::EXIT, {}});
                reqInfo.requestId = RequestId::EXIT;
            }
        }

    } while (reqInfo.requestId != RequestId::EXIT);

    log<Communicator>( __func__, "Closing connection with client, UUID: " + client_uuid,  true, userEndpoint);
    client_socket.close();

    std::lock_guard lockGuard(_clientsMutex);
    _clients.erase(client_uuid);
}

std::string Communicator::generateUUID() {
    boost::uuids::uuid uuid{};

    // generate random uuid until it's unique, supposed to be in the first try.
    // there are  2^122, or 5.3×10^36 (5.3 undecillion) different uuids.
    // so it's not that big of a deal to just loop until it's unique
    // the chances of it not being unique are extremely low
    std::shared_lock lock(_clientsMutex);
    do {
        boost::uuids::random_generator gen;
        uuid = gen();
    } while (_clients.find(boost::uuids::to_string(uuid)) != _clients.end());
    lock.unlock();

    return boost::uuids::to_string(uuid);
}

