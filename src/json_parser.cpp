#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <string>

extern "C" {
#include "json_parser.h"
}

void* create_document() {
    return new rapidjson::Document;
}

void delete_document(void* doc) {
    delete static_cast<rapidjson::Document*>(doc);
}

void parse_json(const char* json, void* doc) {
    static_cast<rapidjson::Document*>(doc)->Parse(json);
}

const char* stringify_json(void* doc) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    static_cast<rapidjson::Document*>(doc)->Accept(writer);
    std::string* result = new std::string(buffer.GetString());
    return result->c_str();
}

void delete_string(const char* str) {
    delete str;
}
