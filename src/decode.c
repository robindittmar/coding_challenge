#include "decode.h"

/**
 * @brief Converts a single character using the ROT13 cipher
 *
 * @param[in]  c  The character to convert.
 *
 * @return c, but encoded/decoded using ROT13
 */
char rot13(char c) {
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
 * @brief Runs ROT13 cipher on whole string
 *
 * Caller takes ownership of the memory and must clean it up
 *
 * @param[in]  encoded  The text to decode
 */
GString* decode_rot13(const char* encoded) {
    gsize length = strlen(encoded);
    GString* decoded = g_string_sized_new(length);

    for (int i = 0; i < length; i++) {
        g_string_append_c_inline(decoded, rot13(encoded[i]));
    }

    return decoded;
}
