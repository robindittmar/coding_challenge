#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>
#include <gvm/util/jsonpull.h>

#include "message.h"


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

    message_finalize(msg);

    cJSON_Delete(obj);
    g_string_free(err_string, TRUE);
}

void read_json_messages_array(gvm_json_pull_parser_t* parser, gvm_json_pull_event_t* event, GList** messages) {
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

void print_message(void* data, void* user_data) {
    if (data == NULL) {
        return;
    }

    message_t* msg = data;
    printf("%s\n", msg->message->str);
}

void free_message(void* data, void* user_data) {
    if (data == NULL) {
        return;
    }

    message_free(data);
}

int main(void) {
    FILE* fp = fopen("messages.json", "r");
    if (!fp) {
        fputs("Could not open file message.json", stderr);
        return -1;
    }

    GList* messages = g_list_alloc();

    gvm_json_pull_parser_t parser;
    gvm_json_pull_event_t event;

    gvm_json_pull_parser_init(&parser, fp);
    gvm_json_pull_event_init(&event);

    while (event.error_message == NULL && event.type != GVM_JSON_PULL_EVENT_EOF) {
        gvm_json_pull_parser_next(&parser, &event);

        gchar* stringPath = gvm_json_path_to_string(event.path);

        if (strcmp(stringPath, "$['messages']") == 0 && event.type == GVM_JSON_PULL_EVENT_ARRAY_START) {
            read_json_messages_array(&parser, &event, &messages);
        }

        g_free(stringPath);
    }

    gvm_json_pull_event_cleanup(&event);
    gvm_json_pull_parser_cleanup(&parser);

    g_list_foreach(messages, print_message, NULL);
    g_list_foreach(messages, free_message, NULL);

    g_list_free(messages);

    fclose(fp);
    return 0;
}