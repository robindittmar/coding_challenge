#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include <gvm/util/jsonpull.h>

char encode_rot13(char c) {
    if (c >= 'a' && c <= 'z') {
        c -= 13;
        if (c < 'a') {
            c += 26;
        }
    } else if (c >= 'A' && c <= 'Z') {
        c -= 13;
        if (c < 'A') {
            c += 26;
        }
    }

    return c;
}

void json_read_message_object(gvm_json_pull_parser_t* parser) {
    GString* err_string = g_string_new(NULL);

    cJSON* obj = gvm_json_pull_expand_container(parser, &err_string->str);
    cJSON* curr = obj->child;

    while (curr) {
        if (strcmp(curr->string, "id") == 0) {
            printf("%s = %d\n", curr->string, curr->valueint);
        } else if (strcmp(curr->string, "timestamp") == 0) {
            printf("%s = %s\n", curr->string, curr->valuestring);
        } else if (strcmp(curr->string, "isEncoded") == 0) {
            printf("%s = %d\n", curr->string, curr->valueint);
        } else if (strcmp(curr->string, "message") == 0) {
            int len = strlen(curr->valuestring);
            char* newstr = strndup(curr->valuestring, len);
            for (int i = 0; i < len; i++) {
                newstr[i] = encode_rot13(newstr[i]);
            }

            printf("%s = %s\n", curr->string, curr->valuestring);
            printf("encoded(%s) = %s\n", curr->string, newstr);

            free(newstr);
        }

        curr = curr->next;
    }

    //cJSON_Delete(obj);
    g_string_free(err_string, TRUE);
}

void read_json_messages_array(gvm_json_pull_parser_t* parser) {
    gvm_json_pull_event_t event;
    gvm_json_pull_event_init(&event);

    while (event.error_message == NULL && event.type != GVM_JSON_PULL_EVENT_ARRAY_END) {
        gvm_json_pull_parser_next(parser, &event);

        if (event.type == GVM_JSON_PULL_EVENT_OBJECT_START) {
            json_read_message_object(parser);
        }
    }

    gvm_json_pull_event_cleanup(&event);
}

int main(void) {
    FILE* fp = fopen("messages.json", "r");
    if (!fp) {
        return -1;
    }

    gvm_json_pull_parser_t parser;
    gvm_json_pull_event_t event;

    gvm_json_pull_parser_init(&parser, fp);
    gvm_json_pull_event_init(&event);

    while (event.error_message == NULL && event.type != GVM_JSON_PULL_EVENT_EOF) {
        gvm_json_pull_parser_next(&parser, &event);

        gchar* stringPath = gvm_json_path_to_string(event.path);

        if (strcmp(stringPath, "$['messages']") == 0 && event.type == GVM_JSON_PULL_EVENT_ARRAY_START) {
            read_json_messages_array(&parser);
        }

        g_free(stringPath);
    }

    gvm_json_pull_event_cleanup(&event);
    gvm_json_pull_parser_cleanup(&parser);

    fclose(fp);
    return 0;
}