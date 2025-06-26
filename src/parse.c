#include "parse.h"

#include "message.h"


/**
 * @brief Initializes parse context
 *
 * The object initialized with this function must be passed to parse_context_cleanup() for destruction
 *
 * @param[in]  ctx  the context to initialize
 * @param[in]  fp  Open/valid filestream pointing to JSON file
 * @param[in]  callback  Callback being called for every JSON message object
 * @param[in]  user_data  User data being passed through to the callback
 */
void parse_context_init(parse_context_t* ctx, FILE* fp, parse_message_callback_t callback, void* user_data) {
    ctx->callback = callback;
    ctx->user_data = user_data;
    ctx->error_message = NULL;

    gvm_json_pull_parser_init(&ctx->parser, fp);
    gvm_json_pull_event_init(&ctx->event);
}

/**
 * @brief Cleans up parse context
 *
 * @param[in]  ctx  the context to destroy
 */
void parse_context_cleanup(parse_context_t* ctx) {
    if (ctx->error_message) {
        g_free(ctx->error_message);
    }

    gvm_json_pull_event_cleanup(&ctx->event);
    gvm_json_pull_parser_cleanup(&ctx->parser);
}

void json_read_messages_array(parse_context_t* ctx);
void json_read_message_object(parse_context_t* ctx);

/**
 * @brief Parses the 'messages' property of the provided JSON filestream, and writes result to parameter messages
 *
 * @param[in]   fp  Open/valid filestream pointing to JSON file
 * @param[out]  messages  The list of parsed messages
 * @param[out]  error_message Error message from parsing
 */
void parse_message_json_file(parse_context_t* ctx) {
    while (ctx->event.error_message == NULL && ctx->event.type != GVM_JSON_PULL_EVENT_EOF) {
        gvm_json_pull_parser_next(&ctx->parser, &ctx->event);

        gchar* stringPath = gvm_json_path_to_string(ctx->event.path);
        if (strcmp(stringPath, "$['messages']") == 0 && ctx->event.type == GVM_JSON_PULL_EVENT_ARRAY_START) {
            json_read_messages_array(ctx);
            if (ctx->error_message) {
                g_free(stringPath);
                break;
            }
        }

        g_free(stringPath);
    }

    if (ctx->event.error_message && ctx->error_message != NULL) {
        ctx->error_message = g_strdup(ctx->event.error_message);
    }
}

void json_read_messages_array(parse_context_t* ctx) {
    while (ctx->event.error_message == NULL && ctx->event.type != GVM_JSON_PULL_EVENT_ARRAY_END) {
        gvm_json_pull_parser_next(&ctx->parser, &ctx->event);

        if (ctx->event.type == GVM_JSON_PULL_EVENT_OBJECT_START) {
            json_read_message_object(ctx);
            if (ctx->error_message) {
                return;
            }
        }
    }

    if (ctx->event.error_message) {
        ctx->error_message = g_strdup(ctx->event.error_message);
    }
}

void json_read_message_object(parse_context_t* ctx) {
    cJSON* obj = gvm_json_pull_expand_container(&ctx->parser, &ctx->error_message);
    if (!obj) {
        return;
    }

    cJSON* cur = obj->child;

    uint64_t id = 0;
    gchar* timestamp = NULL;
    gboolean is_encoded = 0;
    gchar* message = NULL;

    while (cur) {
        if (strcmp(cur->string, "id") == 0) {
            id = cur->valueint;
        } else if (strcmp(cur->string, "timestamp") == 0) {
            timestamp = cur->valuestring;
        } else if (strcmp(cur->string, "isEncoded") == 0) {
            is_encoded = cur->valueint;
        } else if (strcmp(cur->string, "message") == 0) {
            message = cur->valuestring;
        }

        cur = cur->next;
    }

    ctx->callback(ctx->user_data, id, timestamp, is_encoded, message);
    cJSON_Delete(obj);
}

