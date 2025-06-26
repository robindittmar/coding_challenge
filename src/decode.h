#ifndef DECODE_H
#define DECODE_H

#include <glib.h>

char rot13(char c);
GString* decode_rot13(const char* encoded);

#endif //DECODE_H
