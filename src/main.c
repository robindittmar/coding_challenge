#include <stdio.h>
#include <stdlib.h>

#include <glib.h>

#include "message.h"
#include "parse.h"


void process_message(void* data, void* user_data) {
    message_t* msg = data;

    message_decode(msg);
    printf("%s: %s\n", msg->timestamp->str, msg->message->str);

    message_free(data);
}

int main(void) {
    FILE* fp = fopen("messages.json", "r");
    if (!fp) {
        fputs("Could not open file 'message.json'\n", stderr);
        return -1;
    }

    gchar* err = NULL;
    GList* messages = NULL;
    read_messages_json_file(fp, &messages, &err);
    if (err) {
        fprintf(stderr, "Error parsing JSON file: %s\n", err);
        g_list_free(messages);
        g_free(err);
        return -1;
    }

    g_list_foreach(messages, process_message, NULL);
    g_list_free(messages);

    fclose(fp);
    return 0;
}

