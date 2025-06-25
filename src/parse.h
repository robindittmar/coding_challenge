#ifndef PARSER_H
#define PARSER_H

#include <glib.h>
#include <gvm/util/jsonpull.h>

void read_messages_json_file(FILE* fp, GList** messages, gchar** error_message);

#endif //PARSER_H
