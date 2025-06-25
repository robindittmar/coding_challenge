#include "parse.h"

#include "message.h"

void json_read_messages_array(gvm_json_pull_parser_t* parser, gvm_json_pull_event_t* event, GList** messages, gchar** error_message);
void json_read_message_object(gvm_json_pull_parser_t* parser, message_t* msg, gchar** error_message);

/**
 * @brief Parses the 'messages' property of the provided JSON filestream, and writes result to parameter messages
 *
 * @param[in]   fp  Open/valid filestream pointing to JSON file
 * @param[out]  messages  The list of parsed messages
 * @param[out]  error_message Error message from parsing
 */
void read_messages_json_file(FILE* fp, GList** messages, gchar** error_message) {
    gvm_json_pull_parser_t parser;
    gvm_json_pull_event_t event;

    gvm_json_pull_parser_init(&parser, fp);
    gvm_json_pull_event_init(&event);

    while (event.error_message == NULL && event.type != GVM_JSON_PULL_EVENT_EOF) {
        gvm_json_pull_parser_next(&parser, &event);

        gchar* stringPath = gvm_json_path_to_string(event.path);

        if (strcmp(stringPath, "$['messages']") == 0 && event.type == GVM_JSON_PULL_EVENT_ARRAY_START) {
            json_read_messages_array(&parser, &event, messages, error_message);
            if (error_message && *error_message != NULL) {
                g_free(stringPath);
                break;
            }
        }

        g_free(stringPath);
    }

    if (event.error_message != NULL && error_message != NULL) {
        *error_message = g_strdup(event.error_message);
    }

    gvm_json_pull_event_cleanup(&event);
    gvm_json_pull_parser_cleanup(&parser);
}

void json_read_messages_array(gvm_json_pull_parser_t* parser, gvm_json_pull_event_t* event, GList** messages, gchar** error_message) {
    while (event->error_message == NULL && event->type != GVM_JSON_PULL_EVENT_ARRAY_END) {
        gvm_json_pull_parser_next(parser, event);

        if (event->type == GVM_JSON_PULL_EVENT_OBJECT_START) {
            message_t* cur_msg = message_alloc();

            json_read_message_object(parser, cur_msg, error_message);
            if (error_message && *error_message != NULL) {
                message_free(cur_msg);
                return;
            }

            *messages = g_list_insert_sorted(*messages, cur_msg, message_compare_sort);
        }
    }

    if (event->error_message != NULL && error_message != NULL) {
        *error_message = g_strdup(event->error_message);
    }
}

void json_read_message_object(gvm_json_pull_parser_t* parser, message_t* msg, gchar** error_message) {
    cJSON* obj = gvm_json_pull_expand_container(parser, error_message);
    if (!obj) {
        return;
    }

    cJSON* cur = obj->child;

    while (cur) {
        if (strcmp(cur->string, "id") == 0) {
            message_set_id(msg, cur->valueint);
        } else if (strcmp(cur->string, "timestamp") == 0) {
            message_set_timestamp(msg, cur->valuestring);
        } else if (strcmp(cur->string, "isEncoded") == 0) {
            message_set_is_encoded(msg, cur->valueint);
        } else if (strcmp(cur->string, "message") == 0) {
            message_set_message(msg, cur->valuestring);
        }

        cur = cur->next;
    }

    cJSON_Delete(obj);
}

