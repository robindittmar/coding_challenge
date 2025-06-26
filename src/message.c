#include "message.h"

#include <assert.h>
#include "util.h"


/**
 * @brief Allocates and initializes a new message_t
 *
 * message_t object allocated using this method must be destroyed using message_free()
 *
 * @return New message_t, caller takes ownership
 */
message_t* message_alloc() {
    message_t* msg = g_malloc(sizeof(message_t));
    assert(msg);

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
    g_free(msg);
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
    msg->timestamp = g_string_new(NULL);
    msg->content = g_string_new(NULL);
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
    if (msg->content) {
        g_string_free(msg->content, TRUE);
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
    msg->unix_timestamp = parse_message_time(timestamp);

    g_string_erase(msg->timestamp, 0, (gssize)msg->timestamp->len);
    g_string_insert(msg->timestamp, 0, timestamp);
}

/**
 * @brief Sets the .content of message_t
 *
 * @param[in]  msg  The message_t to use
 * @param[in]  content  The value to set
 */
void message_set_content(message_t* msg, const gchar* content) {
    g_string_erase(msg->content, 0, (gssize)msg->content->len);
    g_string_insert(msg->content, 0, content);
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
