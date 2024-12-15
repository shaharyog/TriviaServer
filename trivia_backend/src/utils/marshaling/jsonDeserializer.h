#pragma once

#include <string>
#include <vector>
#include "../requests/requests.h"
#include "../../errors/result.h"


class JsonDeserializer {
public:
    JsonDeserializer() = delete;  // Prevent construction
    ~JsonDeserializer() = delete;  // Prevent destruction

    static Result<LoginRequest> deserializeLoginRequest(const std::vector<unsigned char> &buffer);

    static Result<SignupRequest> deserializeSignupRequest(const std::vector<unsigned char> &buffer);

    static Result<GetPlayersInRoomRequest> deserializeGetPlayersInRoomRequest(const std::vector<unsigned char> &buffer);

    static Result<JoinRoomRequest> deserializeJoinRoomRequest(const std::vector<unsigned char> &buffer);

    static Result<CreateRoomRequest> deserializeCreateRoomRequest(const std::vector<unsigned char> &buffer);

    static Result<UpdateUserDataRequest> deserializeUpdateUserDataRequest(const std::vector<unsigned char> &buffer);

    static Result<SubmitAnswerRequest> deserializeSubmitAnswerRequest(const std::vector<unsigned char> &buffer);

    static Result<SubmitVerificationCodeRequest> deserializeSubmitVerificationCodeRequest(const std::vector<unsigned char> &buffer);

    static Result<ForgotPasswordRequest> deserializeForgotPasswordRequest(const std::vector<unsigned char> &buffer);
};

