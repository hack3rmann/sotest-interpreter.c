#include "str.h"
#include "interpreter.h"

#include <stdio.h>

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    auto buf = STRING_EMPTY;
    auto executor = EXECUTOR_EMPTY;

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

        if (!command_line->has_command) {
            continue;
        }

        switch (command_line->command.type) {
        case COMMAND_TYPE_USE: {
            auto const result =
                executor_load_library(&executor, command_line->command.content);

            if (EXECUTOR_LOAD_FAILED == result.status) {
                fprintf(
                    stderr, "error: failed to load library: %s\n",
                    result.dl_error.ptr
                );

                continue;
            }
        } break;
        case COMMAND_TYPE_CALL: {
            auto const result =
                executor_call_function(&executor, command_line->command.content);

            if (EXECUTOR_FIND_SYMBOL_FAILED == result.status) {
                fprintf(
                    stderr, "error: failed to call the function: %s\n",
                    result.dl_error.ptr
                );

                continue;
            }
        } break;
        }
    }

    executor_free(&executor);
    string_free(&buf);
}
