#pragma once

#include <string>
#include "../utils/marshaling/jsonSerializer.h"
#include "../log.h"
#include "../utils/communicator/endpoint.h"


enum class ErrorType {
    Unknown,
    Database,
    InvalidRequest,
    Socket,
    DisconnectedClient,
    InvalidSignupCredentials,
    InvalidLoginCredentials,
    DeserializationError,
    NotFound,
    AlreadyExists,
    UserNotAuthorized,
    NotImplemented,
    HttpError,
    RoomIsFull,
    NotEnoughPlayers
};

struct Error {
    // basic constructors
    Error() = default;
    explicit Error(const char *message) : message(message) {}
    explicit Error(std::string message) : message(std::move(message)) {}
    Error(ErrorType type, const char *message) : message(message), type(type) {}
    Error(ErrorType type, std::string message) : message(std::move(message)), type(type) {}

    // predefined error messages constructor
    explicit Error(ErrorType type) : type(type) {
        switch (type) {
            case ErrorType::Database:
                message = "Database error";
                break;
            case ErrorType::InvalidRequest:
                message = "Invalid request";
                break;
            case ErrorType::Socket:
                message = "Socket error";
                break;
            case ErrorType::DisconnectedClient:
                message = "Client disconnected";
                break;
            case ErrorType::InvalidSignupCredentials:
                message = "Invalid signup credentials";
                break;
            case ErrorType::InvalidLoginCredentials:
                message = "Invalid login credentials";
                break;
            case ErrorType::DeserializationError:
                message = "Deserialization error";
                break;
            case ErrorType::NotFound:
                message = "Not found";
                break;
            case ErrorType::AlreadyExists:
                message = "Already exists";
                break;
            case ErrorType::UserNotAuthorized:
                message = "User not authorized";
                break;
            case ErrorType::NotImplemented:
                message = "Not implemented";
                break;
            case ErrorType::HttpError:
                message = "Http error";
                break;
            case ErrorType::RoomIsFull:
                message = "Room is full";
                break;
            case ErrorType::NotEnoughPlayers:
                message = "Not enough players in room";
                break;
            default:
                message = "Unknown error";
                break;
        }
    }

    template <typename T>
    [[nodiscard]] RequestResult toRequestResult(const std::string& functionName, const  Endpoint& endpoint) const {
        logError<T>(functionName, message, endpoint);
        return {JsonSerializer::serializeResponse({message}), nullptr};
    }

    std::string message = "Unknown error";
    ErrorType type = ErrorType::Unknown;
};