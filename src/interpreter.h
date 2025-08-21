#ifndef _SOTEST_INTERPRETER_H
#define _SOTEST_INTERPRETER_H

#include "str.h"

#include <stdint.h>

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

typedef void (*ExecutorFunction)();

typedef struct Executor {
    String current_library_path;
    String current_function_name;
    void* library_handle;
    ExecutorFunction function;
} Executor;

Executor constexpr EXECUTOR_EMPTY = {
    .current_library_path = STRING_EMPTY,
    .current_function_name = STRING_EMPTY,
    .library_handle = nullptr,
    .function = nullptr,
};

typedef struct ExecutorResult {
    enum : uint8_t {
        EXECUTOR_SUCCESS = 0,
        EXECUTOR_LOAD_FAILED = 1,
        EXECUTOR_LIBRARY_NOT_LOADED = 2,
        EXECUTOR_FIND_SYMBOL_FAILED = 3,
    } status;

    /// Available only if `status` is `EXECUTOR_LOAD_FAILED` or
    /// `EXECUTOR_FIND_SYMBOL_FAILED`
    Str dl_error;
} ExecutorResult;

/// # Error
///
/// Returns `.status = EXECUTOR_LOAD_FAILED` with `.dl_error` containing the
/// nul-terminated error description str when can not load library
ExecutorResult executor_load_library(Executor* self, Str path);

/// # Error
///
/// Returns `.status = EXECUTOR_FIND_SYMBOL_FAILED` with `.dl_error` containing the
/// nul-terminated error description str when can not load library
ExecutorResult executor_call_function(Executor* self, Str name);

void executor_free(Executor* self);

#endif  // !_SOTEST_INTERPRETER_H
