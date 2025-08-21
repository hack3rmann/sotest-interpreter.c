#ifndef _SOTEST_ARGS_H
#define _SOTEST_ARGS_H

#include "str.h"

#include <stddef.h>

Str constexpr UNUSED_LONG_NAME = STR_NULL;
char constexpr UNUSED_SHORT_NAME = '\0';

size_t constexpr HELP_ARG_INDEX = 0;
size_t constexpr VERSION_ARG_INDEX = 1;

void print_version();

void print_help_message();

/// # Note
///
/// If neither of `long_name` nor `short_name` are provided this is considered a
/// positional argument.
typedef struct ArgEntry {
    /// `UNUSED_LONG_NAME` for no name
    Str long_name;
    /// Description
    Str description;
    /// `UNUSED_SHORT_NAME` for no name
    char short_name;
    /// Argument name. Should be set if the argument is positional
    Str argument_name;
    void (*immediate_callback)();
} ArgEntry;

bool arg_entry_is_positional(ArgEntry const* entry);

/// Finds next available positional argument in the list. Cycles back if
/// `previous + 1 >= N_ARG_ENTRIES`
size_t arg_entry_next_positional(size_t previous);

ArgEntry constexpr ARG_ENTRIES[] = {
    (ArgEntry) {
        .long_name = Str("help"),
        .short_name = 'h',
        .description = Str("show this message"),
        .immediate_callback = print_help_message,
    },
    (ArgEntry) {
        .long_name = Str("version"),
        .short_name = 'v',
        .description = Str("print version"),
        .immediate_callback = print_version,
    },
    (ArgEntry) {
        .description =
            Str("optional: `.sc` input file, will enter interactive mode if "
                "not present"),
        .argument_name = Str("FILE"),
    },
};

size_t constexpr N_ARG_ENTRIES = sizeof(ARG_ENTRIES) / sizeof(*ARG_ENTRIES);
size_t constexpr INVALID_ENTRY_INDEX = N_ARG_ENTRIES;

typedef struct Args {
    struct ArgumentMap* values;
} Args;

Args args_parse(size_t count, char** ptr);

Str args_get(Args const* self, Str long_flag);

void args_free(Args* self);

typedef enum FlagType : uint8_t {
    FLAG_TYPE_LONG = 0,
    FLAG_TYPE_SHORT = 1,
} FlagType;

typedef struct Flag {
    FlagType type;

    union {
        Str long_name;
        char short_name;
    } name;
} Flag;

typedef struct FlagParseResult {
    bool has_value;
    Str tail;
    /// Available only if `success == true`
    Flag value;
} FlagParseResult;

FlagParseResult flag_parse(Str source);

void flag_execute_immediately(Flag self);

size_t flag_find_entry(Flag self);

#endif  // !_SOTEST_ARGS_H
