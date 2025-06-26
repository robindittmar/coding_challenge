#include "util.h"

#include <glib.h>

/**
 * @brief Parses a date/time string in ISO8601 format
 *
 * @param[in]  timestamp  The date/time string to parse
 *
 * @return Unix timestamp
 */
uint64_t parse_message_time(const gchar* timestamp) {
    GDateTime* dt = g_date_time_new_from_iso8601(timestamp, g_time_zone_new_utc());
    uint64_t unix_timestamp = g_date_time_to_unix(dt);
    g_date_time_unref(dt);

    return unix_timestamp;
}
