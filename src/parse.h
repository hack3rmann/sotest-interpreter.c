#ifndef _SOTEST_PARSE_H
#define _SOTEST_PARSE_H

#include "str.h"

#include <stdint.h>

typedef struct ParseResult {
    bool has_value;
    Str tail;
    /// Available only if `success == true`
    Str value;
} ParseResult;

ParseResult parse_prefix(Str source, Str prefix);

ParseResult parse_path(Str source);

ParseResult parse_comment(Str source);

ParseResult parse_function_name(Str source);

typedef enum CommandType : uint8_t {
    COMMAND_TYPE_USE = 0,
    COMMAND_TYPE_CALL,
} CommandType;

typedef struct Command {
    Str content;
    CommandType type;
} Command;

typedef struct CommandParseResult {
    bool has_value;
    Str tail;
    /// Available only if `success == true`
    Command value;
} CommandParseResult;

CommandParseResult command_parse(Str source);

typedef struct CommandLine {
    bool has_command;
    Str comment;
    /// Available only if `has_command == true`
    Command command;
} CommandLine;

typedef struct CommandLineParseResult {
    bool has_value;
    Str tail;
    /// Available only if `success == true`
    CommandLine value;
} CommandLineParseResult;

CommandLineParseResult command_line_parse(Str source);

#endif  // !_SOTEST_PARSE_H
