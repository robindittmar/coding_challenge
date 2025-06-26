#ifndef PARSER_H
#define PARSER_H

#include <glib.h>
#include <stdint.h>
#include <gvm/util/jsonpull.h>

typedef void (*parse_message_callback_t)(void* user_data, uint64_t id, const gchar* timestamp, gboolean is_encoded, const gchar* message);

typedef struct parse_context {
    gvm_json_pull_parser_t parser;
    gvm_json_pull_event_t event;
    parse_message_callback_t callback;
    void* user_data;
    gchar* error_message;
} parse_context_t;

void parse_context_init(parse_context_t* ctx, FILE* fp, parse_message_callback_t callback, void* user_data);
void parse_context_cleanup(parse_context_t* ctx);

void parse_message_json_file(parse_context_t* ctx);

#endif //PARSER_H
