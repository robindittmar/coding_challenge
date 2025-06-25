#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdint.h>
#include <glib.h>


typedef struct message {
    uint64_t id;
    uint64_t unix_timestamp;
    GString* timestamp;
    GString* message;
    gboolean is_encoded;
} message_t;

message_t* message_alloc();
void message_free(message_t* msg);

void message_init(message_t* msg);
void message_cleanup(message_t* msg);

void message_set_id(message_t *msg, uint64_t id);
void message_set_timestamp(message_t* msg, const gchar* timestamp);
void message_set_message(message_t* msg, const gchar* message);
void message_set_is_encoded(message_t* msg, gboolean is_encoded);
void message_finalize(message_t* msg);

gint message_compare_sort(gconstpointer a, gconstpointer b);

#endif //MESSAGE_H
