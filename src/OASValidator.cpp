#include "OASValidator.hpp"
#include <fstream>
#include <algorithm>

OASValidator::OASValidator(const std::string &specs, bool validate_body):validate_body(validate_body) {
    if(specs.empty())
    {
        throw OASValidatorException("File path");
    }

    std::ifstream ifs(specs);
    if(!ifs.is_open())
    {
        throw OASValidatorException("Cannot open file: "+specs);
    }

    std::string         content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
    rapidjson::Document document;
    document.Parse(content.c_str());
    ifs.close();
    if(document.HasParseError())
    {
        throw OASValidatorException("Unable to parse specs: " + specs);
    }

    const rapidjson::Value& paths = document["paths"];
    for(rapidjson::Value::ConstMemberIterator pathItr = paths.MemberBegin(); pathItr != paths.MemberEnd(); ++pathItr)
    {
        std::string key = pathItr->name.GetString();
        key += "_";

        for(rapidjson::Value::ConstMemberIterator methodItr = pathItr->value.MemberBegin(); methodItr != pathItr->value.MemberEnd(); ++methodItr)
        {
            key += methodItr->name.GetString();

            if(methodItr->value.HasMember("requestBody"))
            {
                if(methodItr->value["requestBody"].HasMember("content"))
                {
                    if(methodItr->value["requestBody"]["content"].HasMember("application/json"))
                    {
                        if(methodItr->value["requestBody"]["content"]["application/json"].HasMember("schema"))
                        {

                            auto _schema = new rapidjson::SchemaDocument(methodItr->value["requestBody"]["content"]["application/json"]["schema"]);
                            auto _validator = new rapidjson::SchemaValidator(*_schema);
                            VALIDATORS validator(new SchemaValidator(_schema, _validator));

                            std::transform(key.begin(), key.end(), key.begin(), [](unsigned char c) { return std::tolower(c); });
                            validators.emplace(key, std::move(validator));
                        }
                    }
                }
            }
        }
    }
}

//todo change to ptr for body to make consistent
bool OASValidator::ValidateBody(const std::string& path, const rapidjson::Document &body, ValueType &error) {

    auto validator = validators.at(path).body_validator->validator;
    if(body.Accept(*validator))
    {
        validator->Reset();
        return true;
    }
    else
    {
        error = validator->GetError();
        validator->Reset();
        return false;
    }
}
