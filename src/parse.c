#include "parse.h"

#include "message.h"

void json_read_messages_array(gvm_json_pull_parser_t* parser, gvm_json_pull_event_t* event, GList** messages);
void json_read_message_object(gvm_json_pull_parser_t* parser, message_t* msg);

/**
 * @brief Parses the 'messages' property of the provided JSON filestream, and writes result to parameter messages
 *
 * @param[in]   fp  Open/valid filestream pointing to JSON file
 * @param[out]  messages  The list of parsed messages
 */
void read_messages_json_file(FILE* fp, GList** messages) {
    gvm_json_pull_parser_t parser;
    gvm_json_pull_event_t event;

    gvm_json_pull_parser_init(&parser, fp);
    gvm_json_pull_event_init(&event);

    while (event.error_message == NULL && event.type != GVM_JSON_PULL_EVENT_EOF) {
        gvm_json_pull_parser_next(&parser, &event);

        gchar* stringPath = gvm_json_path_to_string(event.path);

        if (strcmp(stringPath, "$['messages']") == 0 && event.type == GVM_JSON_PULL_EVENT_ARRAY_START) {
            json_read_messages_array(&parser, &event, messages);
        }

        g_free(stringPath);
    }

    gvm_json_pull_event_cleanup(&event);
    gvm_json_pull_parser_cleanup(&parser);
}

void json_read_messages_array(gvm_json_pull_parser_t* parser, gvm_json_pull_event_t* event, GList** messages) {
    while (event->error_message == NULL && event->type != GVM_JSON_PULL_EVENT_ARRAY_END) {
        gvm_json_pull_parser_next(parser, event);

        if (event->type == GVM_JSON_PULL_EVENT_OBJECT_START) {
            message_t *cur_msg = message_alloc();
            json_read_message_object(parser, cur_msg);

            *messages = g_list_insert_sorted(*messages, cur_msg, message_compare_sort);
        }
    }

    gvm_json_pull_event_cleanup(event);
}

void json_read_message_object(gvm_json_pull_parser_t* parser, message_t* msg) {
    GString* err_string = g_string_sized_new(1024);

    cJSON* obj = gvm_json_pull_expand_container(parser, &err_string->str);
    if (!obj) {
        fprintf(stderr, "Error expanding JSON: %s", err_string->str);
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
    g_string_free(err_string, TRUE);
}

