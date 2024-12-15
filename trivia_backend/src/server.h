#pragma once

#include <string>
#include "utils/communicator/communicator.h"
#include "utils/databaseAccess/IDatabasae.h"
#include "utils/databaseAccess/sqliteDatabase.h"
#include "requestHandlers/requestHandlerFactory.h"


class Server {
public:
    explicit Server(const Endpoint &endpoint) : _server_endpoint(endpoint),
                                                _database(std::make_shared<SqliteDatabase>()),
                                                _requestHandlerFactory(_database),
                                                _communicator(endpoint, _requestHandlerFactory) {}

    ~Server() = default;

    void run();

private:
    Endpoint _server_endpoint;
    Communicator _communicator;
    std::shared_ptr<IDatabase> _database;
    RequestHandlerFactory _requestHandlerFactory;
};


