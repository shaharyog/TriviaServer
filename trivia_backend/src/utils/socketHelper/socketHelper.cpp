#include <iostream>
#include "socketHelper.h"

Result<std::vector<unsigned char>>
SocketHelper::getPartFromSocket(kissnet::tcp_socket &socket, unsigned int bytesToRead) {
    try {

        std::vector<unsigned char> buffer(bytesToRead);
        if (auto [size, valid] = socket.recv(reinterpret_cast<std::byte *>(buffer.data()), buffer.size()); valid) {
            if (valid.value == kissnet::socket_status::cleanly_disconnected)
                return Error(ErrorType::DisconnectedClient);

            if (size != bytesToRead)
                return Error(ErrorType::InvalidRequest, "Invalid request length");

            return buffer;
        }
    }
    catch (const std::exception &e) {
        return Error(ErrorType::Socket, "Error receiving from socket.");
    }

    return Error(ErrorType::Socket);   // unknown socket error
}

std::optional<Error> SocketHelper::sendData(kissnet::tcp_socket &socket, std::vector<unsigned char> message) {
    if (auto [size, valid] = socket.send(reinterpret_cast<const std::byte *>(message.data()), message.size()); valid) {
        if (valid.value == kissnet::socket_status::cleanly_disconnected)
            return Error(ErrorType::DisconnectedClient);

        return std::nullopt;
    }

    return Error(ErrorType::Socket);   // unknown socket error
}


Result<RequestId> SocketHelper::getRequestId(kissnet::tcp_socket &socket) {
    const auto requestIdRes = getPartFromSocket(socket, 1);
    if (requestIdRes.isError())
        return requestIdRes.error();

    auto requestId = static_cast<RequestId>(requestIdRes.value()[0]);
    switch (requestId) {
        case RequestId::LOGIN_REQUEST:
        case RequestId::SIGNUP_REQUEST:
        case RequestId::LOGOUT_REQUEST:
        case RequestId::GET_ROOMS_REQUEST:
        case RequestId::GET_PLAYERS_IN_ROOM_REQUEST:
        case RequestId::GET_HIGHSCORES_REQUEST:
        case RequestId::GET_PERSONAL_STATS_REQUEST:
        case RequestId::JOIN_ROOM_REQUEST:
        case RequestId::CREATE_ROOM_REQUEST:
        case RequestId::GET_USER_DATA_REQUEST:
        case RequestId::UPDATE_USER_DATA_REQUEST:
        case RequestId::CLOSE_ROOM_REQUEST:
        case RequestId::START_GAME_REQUEST:
        case RequestId::GET_ROOM_STATE_REQUEST:
        case RequestId::LEAVE_ROOM_REQUEST:
        case RequestId::LEAVE_GAME_REQUEST:
        case RequestId::GET_QUESTION_REQUEST:
        case RequestId::SUBMIT_ANSWER_REQUEST:
        case RequestId::GET_GAME_RESULTS_REQUEST:
        case RequestId::SUBMIT_VERIFICATION_CODE_REQUEST:
        case RequestId::RESEND_VERIFICATION_CODE_REQUEST:
        case RequestId::FORGOT_PASSWORD_REQUEST:
        case RequestId::EXIT:
            break;
        default:
            return Error(ErrorType::InvalidRequest, "Invalid request ID.");
    }
    return requestId;
}

// this function does not return result because if any error happens, we pass EXIT request to the handler
// to start the chain of logout
RequestInfo SocketHelper::getRequestInfo(kissnet::tcp_socket &socket) {
    const auto userEndpoint = Endpoint(socket);

    const auto requestId = getRequestId(socket);
    if (requestId.isError()) {
        log<SocketHelper>(__func__, requestId.error().message, false, userEndpoint);
        return RequestInfo{RequestId::EXIT,  {}};
    }

    const auto requestLength = getRequestLength(socket);
    if (requestLength.isError()) {
        log<SocketHelper>(__func__, requestId.error().message, false,
                          userEndpoint);
        return RequestInfo{RequestId::EXIT, {}};
    }

    const auto buffer = getPartFromSocket(socket, requestLength.value());
    if (buffer.isError()) {
        log<SocketHelper>(__func__, requestId.error().message, false,
                          userEndpoint);
        return RequestInfo{RequestId::EXIT,  {}};
    }

    return RequestInfo{requestId.value(), buffer.value()};
}

Result<unsigned int> SocketHelper::getRequestLength(kissnet::tcp_socket &socket) {
    const auto bytes_result = getPartFromSocket(socket, 4);
    if (bytes_result.isError())
        return bytes_result.error();

    const auto &bytes = bytes_result.value();

    if (bytes.size() != 4)
        return Error(ErrorType::InvalidRequest, "Invalid request size header.");

    unsigned int value = bytes[0] << 24 | bytes[1] << 16 | bytes[2] << 8 | bytes[3];    // big endian
    return value;
}



