#include <stdio.h>
#include <gvm/util/jsonpull.h>


int main(void) {
    FILE* fp = fopen("messages.json", "r");
    if (!fp) {
        return -1;
    }

    gvm_json_pull_parser_t parser;
    gvm_json_pull_parser_init(&parser, fp);
    // TBD
    gvm_json_pull_parser_cleanup(&parser);

    fclose(fp);
    return 0;
}