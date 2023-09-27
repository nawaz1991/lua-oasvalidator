#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

void* create_document();
void delete_document(void* doc);

void parse_json(const char* json, void* doc);
const char* stringify_json(void* doc);
void delete_string(const char* str);

#ifdef __cplusplus
}
#endif

#endif
