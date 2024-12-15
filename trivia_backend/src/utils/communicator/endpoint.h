#pragma once

#include <string>
#include <utility>
#include <kissnet.hpp>

struct Endpoint : public kissnet::endpoint {

    explicit Endpoint(const std::string &address, const int port) : kissnet::endpoint(address, port) {}

    explicit Endpoint(const kissnet::tcp_socket &socket) : kissnet::endpoint(socket.get_recv_endpoint()) {}

    explicit Endpoint(const kissnet::endpoint &endpoint) : kissnet::endpoint(endpoint) {}

    [[nodiscard]] std::string toString() const {
        return address + ":" + std::to_string(port);
    }

    friend std::ostream &operator<<(std::ostream &os, const Endpoint &ue) {
        os << ue.toString();
        return os;
    }
};
