#include <stdio.h>
#include <stdlib.h>

#include <glib.h>

#include "decode.h"
#include "message.h"
#include "parse.h"


void parse_message_callback(void* user_data, uint64_t id, const gchar* timestamp, gboolean is_encoded, const gchar* message) {
    GPtrArray* messages = user_data;

    message_t* msg = message_alloc();
    message_set_id(msg, id);
    message_set_timestamp(msg, timestamp);

    if (is_encoded) {
        GString* decoded = decode_rot13(message);
        message_set_content(msg, decoded->str);
        g_string_free(decoded, TRUE);
    } else {
        message_set_content(msg, message);
    }

    g_ptr_array_add(messages, msg);
}

void print_message(void* data, void* user_data) {
    message_t* msg = data;
    printf("%s: %s\n", msg->timestamp->str, msg->content->str);
}

int main(int argc, char** argv) {
    gchar* json_filepath;
    if (argc > 1) {
        json_filepath = argv[1];
    } else {
        json_filepath = "messages.json";
    }

    FILE* fp = fopen(json_filepath, "r");
    if (!fp) {
        fprintf(stderr, "Could not open file '%s'\n", json_filepath);
        return -1;
    }

    GPtrArray* messages = g_ptr_array_new_with_free_func((GDestroyNotify)message_free);

    parse_context_t ctx;
    parse_context_init(&ctx, fp, &parse_message_callback, messages);

    parse_message_json_file(&ctx);
    if (ctx.error_message) {
        fprintf(stderr, "Error parsing JSON file: %s\n", ctx.error_message);
        g_ptr_array_free(messages, TRUE);
        parse_context_cleanup(&ctx);
        return -1;
    }
    parse_context_cleanup(&ctx);
    fclose(fp);

    g_ptr_array_sort(messages, message_compare_sort);
    g_ptr_array_foreach(messages, print_message, NULL);
    g_ptr_array_free(messages, TRUE);

    return 0;
}

