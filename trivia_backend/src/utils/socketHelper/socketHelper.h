#pragma once

#include <vector>
#include <kissnet.hpp>
#include <optional>
#include "../requests/requests.h"
#include "../../requestHandlers/IRequestHandler.h"
#include "../../errors/result.h"

class SocketHelper {
public:
    SocketHelper() = delete;  // Prevent construction
    ~SocketHelper() = delete;  // Prevent destruction

    static RequestInfo getRequestInfo(kissnet::tcp_socket &socket);

    static std::optional<Error> sendData(kissnet::tcp_socket &socket, std::vector<unsigned char> message);

    static Result<std::vector<unsigned char>> getPartFromSocket(kissnet::tcp_socket &socket, unsigned int bytesToRead);

private:
    static Result<RequestId> getRequestId(kissnet::tcp_socket &socket);
    static Result<unsigned int> getRequestLength(kissnet::tcp_socket &socket);
};

