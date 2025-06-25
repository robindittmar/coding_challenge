#include "message.h"

/**
 * @brief Converts a single character using the ROT13 cipher
 *
 * @param[in]  c  The character to convert.
 *
 * @return c, but encoded using ROT13
 */
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

/**
 * @brief Parses a date/time string in ISO8601 format
 *
 * @param[in]  timestamp  The date/time string to parse
 *
 * @return Unix timestamp
 */
uint64_t message_parse_time(const gchar* timestamp) {
    GDateTime* dt = g_date_time_new_from_iso8601(timestamp, g_time_zone_new_utc());
    return g_date_time_to_unix(dt);
}

/**
 * @brief Allocates and initializes a new message_t
 *
 * message_t objects allocated using this method must be destroyed using message_free()
 *
 * @return New message_t, caller takes ownership
 */
message_t* message_alloc() {
    message_t* msg = malloc(sizeof(message_t));
    if (!msg) {
        abort();
    }

    message_init(msg);
    return msg;
}

/**
 * @brief Cleans allocated memory for message_t
 *
 * @param[in]  msg  The message_t that should be destroyed
 */
void message_free(message_t* msg) {
    message_cleanup(msg);
    free(msg);
}

/**
 * @brief Initializes a message_t object
 *
 * message_t objects initialized with this function must be cleaned up using message_cleanup
 *
 * @param[in]  msg  Pointer to already allocated memory, caller keeps ownership
 */
void message_init(message_t* msg) {
    msg->id = 0;
    msg->unix_timestamp = 0;
    msg->is_encoded = FALSE;
    msg->timestamp = g_string_new(NULL);
    msg->message = g_string_new(NULL);
}

/**
 * @brief Cleans allocated memory from message_t
 *
 * @param[in]  msg  The message_t that should be destroyed
 */
void message_cleanup(message_t* msg) {
    if (msg->timestamp) {
        g_string_free(msg->timestamp, TRUE);
    }
    if (msg->message) {
        g_string_free(msg->message, TRUE);
    }

    memset(msg, 0, sizeof(message_t));
}

/**
 * @brief Sets the .id of message_t
 *
 * @param[in]  msg  The message_t to use
 * @param[in]  id  The value to set
 */
void message_set_id(message_t *msg, uint64_t id) {
    msg->id = id;
}

/**
 * @brief Sets the .timestamp of message_t and derives .unix_timestamp
 *
 * @param[in]  msg  The message_t to use
 * @param[in]  timestamp  The value to set (and derive .unix_timestamp from)
 */
void message_set_timestamp(message_t* msg, const gchar* timestamp) {
    msg->unix_timestamp = message_parse_time(timestamp);

    g_string_erase(msg->timestamp, 0, (gssize)msg->timestamp->len);
    g_string_insert(msg->timestamp, 0, timestamp);
}

/**
 * @brief Sets the .message of message_t
 *
 * @param[in]  msg  The message_t to use
 * @param[in]  message  The value to set
 */
void message_set_message(message_t* msg, const gchar* message) {
    g_string_erase(msg->message, 0, (gssize)msg->message->len);
    g_string_insert(msg->message, 0, message);
}

/**
 * @brief Sets the .is_encoded of message_t
 *
 * @param[in]  msg  The message_t to use
 * @param[in]  is_encoded  The value to set
 */

void message_set_is_encoded(message_t* msg, gboolean is_encoded) {
    msg->is_encoded = is_encoded;
}

/**
 * @brief Runs optional decoding step for message property
 *
 * @param[in]  msg  The message_t to use
 */
void message_decode(message_t* msg) {
    if (msg->is_encoded) {
        gsize length = msg->message->len;
        GString* decoded = g_string_sized_new(length);

        for (int i = 0; i < length; i++) {
            g_string_append_c_inline(decoded, encode_rot13(msg->message->str[i]));
        }

        g_string_free(msg->message, TRUE);
        msg->message = decoded;
        msg->is_encoded = FALSE;
    }
}

/**
 * @brief GCompareFunc for message_t
 *
 * @param[in]  a  message_t a
 * @param[in]  b  message_t b
 */
gint message_compare_sort(gconstpointer a, gconstpointer b) {
    const message_t* msg_a = a;
    const message_t* msg_b = b;

    // Sort by timestamp
    if (msg_a->unix_timestamp > msg_b->unix_timestamp) {
        return 1;
    }
    if (msg_a->unix_timestamp < msg_b->unix_timestamp) {
        return -1;
    }

    // If timestamps are equal, sort by ID
    if (msg_a->id > msg_b->id) {
        return 1;
    }
    if (msg_a->id < msg_b->id) {
        return -1;
    }

    return 0;
}
