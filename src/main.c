#include "str.h"
#include "parse.h"

#include <stdio.h>
#include <stdlib.h>

size_t constexpr LINE_CAPACITY = 256;
char LINE[LINE_CAPACITY];

int main(int argc, char* argv[]) {
    while (true) {
        printf(" >>> ");

        auto line = str_from_ptr(fgets(LINE, LINE_CAPACITY, stdin));

        // remove trailing newline
        line.ptr[line.len - 1] = '\0';
        line.len -= 1;

        auto command_line_result = command_line_parse(line);

        if (str_starts_with(line, Str("exit"))) {
            break;
        }

        auto tail = str_trim_end(command_line_result.tail);

        if (!command_line_result.has_value || 0 != tail.len) {
            fprintf(stderr, "error: failed to parse '%s' as command line\n", line.ptr);
            continue;
        }

        auto command_line = &command_line_result.value;

        if (0 != command_line->comment.len) {
            printf(" <<< #");
            str_write(command_line->comment, stdout);
            printf("\n");
        }

        if (command_line->has_command) {
            switch (command_line->command.type) {
            case COMMAND_TYPE_USE:
                printf(" <<< USE ");
                break;
            case COMMAND_TYPE_CALL:
                printf(" <<< CALL ");
                break;
            }

            str_write(command_line->command.content, stdout);

            printf("\n");
        }
    }
}
