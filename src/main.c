#include "str.h"
#include "parse.h"

#include <stdio.h>

int main(int argc, char* argv[]) {
    auto buf = STRING_EMPTY;

    while (true) {
        printf(" >>> ");

        string_clear(&buf);

        if (READLINE_EOF == string_readline(&buf, stdin)) {
            break;
        }

        auto line = str_trim(buf.str);

        auto command_line_result = command_line_parse(str_trim(line));

        if (str_starts_with(line, Str("exit"))) {
            break;
        }

        auto tail = str_trim_end(command_line_result.tail);

        if (!command_line_result.has_value || 0 != tail.len) {
            fprintf(
                stderr, "error: failed to parse '%s' as `CommandLine`\n",
                line.ptr
            );
            continue;
        }

        auto command_line = &command_line_result.value;

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

    string_free(&buf);
}
