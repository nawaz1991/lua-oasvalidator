#pragma once

#include <rapidjson/schema.h>
#include <string>
#include <unordered_map>
#include <exception>

typedef rapidjson::GenericValue<rapidjson::UTF8<>, rapidjson::CrtAllocator> ValueType;

class OASValidatorException : public std::exception {
private:
    std::string errorMsg;

public:
    explicit OASValidatorException(std::string message) : errorMsg(std::move(message)) {}

    [[nodiscard]] const char *
    what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_USE_NOEXCEPT override { return errorMsg.c_str(); }
};

class OASValidator {
private:
    class SchemaValidator {
    public:
        rapidjson::SchemaDocument *schema;
        rapidjson::SchemaValidator *validator;

        explicit SchemaValidator(rapidjson::SchemaDocument *schema,
                                 rapidjson::SchemaValidator *validator)
                : schema(schema), validator(validator) {}
    };

    class VALIDATORS {
    public:
        SchemaValidator *body_validator;

        explicit VALIDATORS(SchemaValidator *body_validator)
                : body_validator(body_validator) {}
    };

    std::unordered_map<std::string, VALIDATORS> validators;
    const bool validate_body;

public:
    OASValidator() = delete;

    explicit OASValidator(const std::string &specs, bool validate_body = true);

    bool ValidateBody(const std::string &path, const rapidjson::Document &body, ValueType &error);
};

