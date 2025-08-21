#include "parse.h"
#include "interpreter.h"

#include <ctype.h>

ParseResult parse_prefix(Str source, Str prefix) {
    if (!str_starts_with(source, prefix)) {
        return (ParseResult) {
            .has_value = false,
            .tail = source,
        };
    }

    return (ParseResult) {
        .has_value = true,
        .value = str_slice(source, 0, prefix.len),
        .tail = str_slice(source, prefix.len, source.len),
    };
}

ParseResult parse_path(Str source) {
    if (0 == source.len) {
        return (ParseResult) {
            .has_value = false,
            .tail = source,
        };
    }

    size_t end = 0;

    for (; end < source.len; ++end) {
        auto symbol = source.ptr[end];

        if (!isalnum(symbol) && '/' != symbol && '.' != symbol &&
            '-' != symbol && '_' != symbol)
        {
            break;
        }
    }

    // If no valid path characters found
    if (0 == end) {
        return (ParseResult) {
            .has_value = false,
            .tail = source,
        };
    }

    // Return the parsed path
    return (ParseResult) {
        .has_value = true,
        .value = str_slice(source, 0, end),
        .tail = str_slice(source, end, source.len),
    };
}

ParseResult parse_comment(Str source) {
    if (!str_starts_with(source, Str("#"))) {
        return (ParseResult) {
            .has_value = false,
            .tail = source,
        };
    }

    size_t end = 1;

    for (; end < source.len; ++end) {
        if ('\n' == source.ptr[end]) {
            break;
        }
    }

    return (ParseResult) {
        .has_value = true,
        .value = str_slice(source, 1, end),
        .tail = str_slice(source, end, source.len),
    };
}

ParseResult parse_function_name(Str source) {
    // The fist character is a letter
    if (0 == source.len || !isalpha(source.ptr[0])) {
        return (ParseResult) {
            .has_value = false,
            .tail = source,
        };
    }

    size_t end = 1;

    for (; end < source.len; ++end) {
        auto symbol = source.ptr[end];

        if (!isalnum(symbol) && '_' != symbol) {
            break;
        }
    }

    return (ParseResult) {
        .has_value = true,
        .value = str_slice(source, 0, end),
        .tail = str_slice(source, end, source.len),
    };
}

CommandParseResult command_parse(Str source) {
    auto command_result = parse_prefix(source, Str("use"));
    auto command_type = (CommandType) {};

    if (command_result.has_value) {
        command_type = COMMAND_TYPE_USE;
    } else {
        command_result = parse_prefix(source, Str("call"));

        if (command_result.has_value) {
            command_type = COMMAND_TYPE_CALL;
        } else {
            return (CommandParseResult) {
                .has_value = false,
                .tail = source,
            };
        }
    }

    auto content_str = str_trim_start(command_result.tail);

    // Should trim at least one whitespace
    if (content_str.len == command_result.tail.len) {
        return (CommandParseResult) {
            .has_value = false,
            .tail = source,
        };
    }

    auto content_result = (ParseResult) {};

    switch (command_type) {
    case COMMAND_TYPE_USE:
        content_result = parse_path(content_str);
        break;
    case COMMAND_TYPE_CALL:
        content_result = parse_function_name(content_str);
        break;
    }

    if (!content_result.has_value) {
        return (CommandParseResult) {
            .has_value = false,
            .tail = source,
        };
    }

    return (CommandParseResult) {
        .has_value = true,
        .value =
            (Command) {
                .content = content_result.value,
                .type = command_type,
            },
        .tail = content_result.tail,
    };
}

CommandLineParseResult command_line_parse(Str source) {
    // We have 3 cases here:
    // 1. <COMMAND> # <COMMENT> \n
    // 2. # <COMMENT> \n
    // 3. \n

    auto command_result = command_parse(source);
    auto comment_result = parse_comment(str_trim_start(command_result.tail));

    return (CommandLineParseResult) {
        .has_value = true,
        .value =
            (CommandLine) {
                .has_command = command_result.has_value,
                .command = command_result.value,
                .comment =
                    comment_result.has_value ? comment_result.value : Str(""),
            },
        .tail = comment_result.has_value ? comment_result.tail
                                         : command_result.tail,
    };
}
