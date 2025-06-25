#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include "message.h"
#include "parse.h"


void decode_and_print_message(void* data, void* user_data) {
    if (data == NULL) {
        return;
    }

    message_t* msg = data;
    message_decode(msg);

    printf("%s: %s\n", msg->timestamp->str, msg->message->str);
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

    read_messages_json_file(fp, &messages);
    g_list_foreach(messages, decode_and_print_message, NULL);

    g_list_foreach(messages, free_message, NULL);
    g_list_free(messages);

    fclose(fp);
    return 0;
}