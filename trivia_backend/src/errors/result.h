#pragma once

#include <string>
#include <variant>
#include "error.h"


// Primary template for Result class
template <typename T>
struct Result : std::variant<T, Error> {
    using std::variant<T, Error>::variant;

    explicit operator bool() const noexcept { return std::holds_alternative<T>(*this); }

    [[nodiscard]] bool isError() const noexcept { return std::holds_alternative<Error>(*this); }

    T value() const { return std::get<T>(*this); }

    [[nodiscard]] Error error() const { return std::get<Error>(*this); }
};
