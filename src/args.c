#include "args.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include "str.h"

#define K String
#define V String
#define SNAME ArgumentMap
#define PFX argument_map

#include <cmc/hashmap.h>

static int local_string_compare(String a, String b) {
    return string_compare(&a, &b);
}

static void local_string_free(String string) { string_free(&string); }

static size_t local_string_hash(String string) { return string_hash(&string); }

struct ArgumentMap_fkey ARGUMENT_MAP_FKEY = {
    .cmp = local_string_compare,
    .free = local_string_free,
    .hash = local_string_hash,
};

struct ArgumentMap_fval ARGUMENT_MAP_FVAL = {
    .cmp = local_string_compare,
    .free = local_string_free,
    .hash = local_string_hash,
};

#define UNDERLINE_START "\033[4m"
#define UNDERLINE_END "\033[24m"

bool arg_entry_is_positional(ArgEntry const* entry) {
    return entry->short_name == UNUSED_SHORT_NAME &&
           entry->long_name.len == UNUSED_LONG_NAME.len &&
           entry->long_name.ptr == UNUSED_LONG_NAME.ptr;
}

Args args_parse(size_t count, char** ptr) {
    bool is_flagged = false;
    size_t flag_entry_index = N_ARG_ENTRIES;
    auto positional_arg_index = N_ARG_ENTRIES;

    auto values =
        argument_map_new(16, 0.5, &ARGUMENT_MAP_FKEY, &ARGUMENT_MAP_FVAL);
    assert(nullptr != values);

    for (size_t i = 1; i < count; ++i) {
        auto arg = str_from_ptr(ptr[i]);
        auto flag_result = flag_parse(arg);

        // The value is flag
        if (flag_result.has_value && 0 == flag_result.tail.len) {
            flag_execute_immediately(flag_result.value);

            auto entry_index = flag_find_entry(flag_result.value);

            if (INVALID_ENTRY_INDEX == entry_index) {
                argument_map_free(values);

                switch (flag_result.value.type) {
                case FLAG_TYPE_LONG:
                    fprintf(
                        stderr, "error: unknown flag '--%s'\n",
                        flag_result.value.name.long_name.ptr
                    );
                    exit(EXIT_FAILURE);
                case FLAG_TYPE_SHORT:
                    fprintf(
                        stderr, "error: unknown flag '-%c'\n",
                        flag_result.value.name.short_name
                    );
                    exit(EXIT_FAILURE);
                }
            }

            // Flag with an argument
            if (0 != ARG_ENTRIES[entry_index].argument_name.len) {
                flag_entry_index = entry_index;
                is_flagged = true;
            }
        } else {  // The value is an argument
            auto value = STRING_EMPTY;
            auto key = STRING_EMPTY;

            string_append(&value, arg);

            size_t entry_index = flag_entry_index;

            if (!is_flagged) {
                positional_arg_index =
                    arg_entry_next_positional(positional_arg_index);
                entry_index = positional_arg_index;
            }

            if (INVALID_ENTRY_INDEX == entry_index) {
                fprintf(stderr, "unexpected argument '%s'\n", arg.ptr);

                string_free(&key);
                string_free(&value);
                argument_map_free(values);

                exit(EXIT_FAILURE);
            }

            auto entry = &ARG_ENTRIES[entry_index];

            if (0 != entry->long_name.len) {
                string_append(&key, entry->long_name);
            } else if (UNUSED_SHORT_NAME != entry->short_name) {
                string_push(&key, entry->short_name);
            } else {
                string_append(&key, entry->argument_name);
            }

            argument_map_insert(values, key, value);

            is_flagged = false;
        }
    }

    return (Args) {
        .values = values,
    };
}

void print_version() {
    printf("%s version %s\n", PROJECT_NAME, PROJECT_VERSION);
}

void print_help_message() {
    printf("%s v%s %s\n\n", PROJECT_NAME, PROJECT_VERSION, PROJECT_DESCRIPTION);

    if (isatty(STDOUT_FILENO)) {
        printf(
            UNDERLINE_START "Usage:" UNDERLINE_END " %s [OPTIONS]", PROJECT_NAME
        );
    } else {
        printf("Usage: %s [OPTIONS]", PROJECT_NAME);
    }

    bool has_positional_args = false;
    bool has_non_positional_args = false;

    for (size_t i = 0; i < N_ARG_ENTRIES; ++i) {
        if (arg_entry_is_positional(&ARG_ENTRIES[i]) &&
            0 != ARG_ENTRIES[i].argument_name.len)
        {
            has_positional_args = true;
            printf(" <%s>", ARG_ENTRIES[i].argument_name.ptr);
        }

        has_non_positional_args = true;
    }

    printf("\n\n");

    if (has_positional_args) {
        if (isatty(STDOUT_FILENO)) {
            printf(UNDERLINE_START "Arguments:\n" UNDERLINE_END);
        } else {
            printf("Arguments:\n");
        }

        for (size_t i = 0; i < N_ARG_ENTRIES; ++i) {
            auto arg = &ARG_ENTRIES[i];

            if (!arg_entry_is_positional(arg) || 0 == arg->argument_name.len) {
                continue;
            }

            printf(
                "  <%s>%-*s%s\n", arg->argument_name.ptr,
                8 - (int) arg->argument_name.len, "", arg->description.ptr
            );
        }
    }

    printf("\n");

    if (has_non_positional_args) {
        if (isatty(STDOUT_FILENO)) {
            printf(UNDERLINE_START "Options:\n" UNDERLINE_END);
        } else {
            printf("Options:\n");
        }

        for (size_t i = 0; i < N_ARG_ENTRIES; ++i) {
            auto arg = &ARG_ENTRIES[i];

            if (arg_entry_is_positional(arg)) {
                continue;
            }

            bool has_short_name = UNUSED_SHORT_NAME != arg->short_name;

            if (has_short_name) {
                printf("  -%c", arg->short_name);
            } else {
                printf("    ");
            }

            auto separator = has_short_name ? ',' : ' ';
            int offset = 16;

            if (0 != arg->long_name.len) {
                printf("%c --%s", separator, arg->long_name.ptr);
                offset -= 4 + (int) arg->long_name.len;
            }

            if (nullptr != arg->argument_name.ptr) {
                printf(" <%s>", arg->argument_name.ptr);
                offset -= 3 + (int) arg->argument_name.len;
            }

            printf(
                "%-*s%s\n", offset, "",
                arg->description.ptr ? arg->description.ptr : ""
            );
        }
    }
}

static bool is_valid_flag_char(char symbol) {
    return isalnum(symbol) || '-' == symbol || '_' == symbol;
}

FlagParseResult flag_parse(Str source) {
    if (str_starts_with(source, Str("--"))) {
        auto name = str_slice(source, 2, source.len);
        size_t end = 0;

        for (; end < name.len; ++end) {
            auto symbol = name.ptr[end];

            if (!is_valid_flag_char(symbol)) {
                break;
            }
        }

        if (0 == end) {
            return (FlagParseResult) {
                .has_value = false,
                .tail = source,
            };
        }

        return (FlagParseResult) {
            .has_value = true,
            .value =
                (Flag) {
                    .name.long_name = str_slice(name, 0, end),
                    .type = FLAG_TYPE_LONG,
                },
            .tail = str_slice(name, end, name.len),
        };
    } else if (str_starts_with(source, Str("-"))) {
        auto name = str_slice(source, 1, source.len);

        if (0 == name.len || !is_valid_flag_char(name.ptr[0])) {
            return (FlagParseResult) {
                .has_value = false,
                .tail = source,
            };
        }

        return (FlagParseResult) {
            .has_value = true,
            .value =
                (Flag) {
                    .name.short_name = name.ptr[0],
                    .type = FLAG_TYPE_SHORT,
                },
            .tail = str_slice(source, 2, source.len),
        };
    }

    return (FlagParseResult) {
        .has_value = false,
        .tail = source,
    };
}

void flag_execute_immediately(Flag self) {
    for (size_t i = 0; i < N_ARG_ENTRIES; ++i) {
        auto arg = &ARG_ENTRIES[i];

        if (nullptr == arg->immediate_callback) {
            continue;
        }

        if ((FLAG_TYPE_LONG == self.type &&
             str_eq(arg->long_name, self.name.long_name)) ||
            (FLAG_TYPE_SHORT == self.type &&
             arg->short_name == self.name.short_name))
        {
            arg->immediate_callback();
            exit(EXIT_SUCCESS);
        }
    }
}

size_t flag_find_entry(Flag self) {
    for (size_t i = 0; i < N_ARG_ENTRIES; ++i) {
        auto arg = &ARG_ENTRIES[i];

        if ((FLAG_TYPE_LONG == self.type &&
             str_eq(self.name.long_name, arg->long_name)) ||
            (FLAG_TYPE_SHORT == self.type &&
             self.name.short_name == arg->short_name))
        {
            return i;
        }
    }

    return INVALID_ENTRY_INDEX;
}

size_t arg_entry_next_positional(size_t previous) {
    auto start = previous + 1 < N_ARG_ENTRIES ? previous + 1 : 0;

    for (size_t i = start; i < N_ARG_ENTRIES; ++i) {
        if (arg_entry_is_positional(&ARG_ENTRIES[i])) {
            return i;
        }
    }

    return N_ARG_ENTRIES;
}

void args_free(Args* self) {
    if (nullptr != self->values) {
        argument_map_free(self->values);
        self->values = nullptr;
    }
}

Str args_get(Args const* self, Str long_flag) {
    return argument_map_get(self->values, (String) {.str = long_flag}).str;
}
