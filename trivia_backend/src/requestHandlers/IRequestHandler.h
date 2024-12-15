#pragma once

#include <ctime>
#include <vector>
#include <memory>
#include "../utils/requests/requests.h"

struct RequestInfo;
struct RequestResult;

// Interface for request handlers
class IRequestHandler {
public:
    virtual ~IRequestHandler() = default;

    virtual bool isRequestRelevant(const RequestInfo &request) = 0;

    virtual RequestResult handleRequest(const RequestInfo &request) = 0;

};