#include "str.h"
#include "interpreter.h"
#include "args.h"

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    auto args = args_parse((size_t) argc, argv);

    auto buf = STRING_EMPTY;
    auto executor = executor_new();
    auto input = stdin;

    auto file_argument = args_get(&args, Str("FILE"));
    bool reading_from_file = 0 != file_argument.len;

    if (reading_from_file) {
        input = fopen(file_argument.ptr, "rb");

        if (nullptr == input) {
            fprintf(
                stderr, "failed to open file '%s': %s\n", file_argument.ptr,
                strerror(errno)
            );

            executor_free(&executor);
            string_free(&buf);
            args_free(&args);

            exit(EXIT_FAILURE);
        }
    }

    while (true) {
        // Print arrows in terminal-mode only
        if (isatty(STDIN_FILENO) && !reading_from_file) {
            printf(" >>> ");
        }

        string_clear(&buf);

        if (READLINE_EOF == string_readline(&buf, input)) {
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

        if (!command_line->has_command) {
            continue;
        }

        switch (command_line->command.type) {
        case COMMAND_TYPE_USE: {
            auto const result =
                executor_load_library(&executor, command_line->command.content);

            if (EXECUTOR_SUCCESS != result.status) {
                fprintf(
                    stderr, "error: failed to load library: %s\n",
                    result.dl_error.ptr
                );

                continue;
            }
        } break;
        case COMMAND_TYPE_CALL: {
            auto const result = executor_call_function(
                &executor, command_line->command.content
            );

            if (EXECUTOR_SUCCESS != result.status) {
                fprintf(
                    stderr, "error: failed to call the function: %s\n",
                    result.dl_error.ptr
                );

                continue;
            }
        } break;
        }
    }

    if (reading_from_file) {
        fclose(input);
    }

    executor_free(&executor);
    string_free(&buf);
    args_free(&args);
}
