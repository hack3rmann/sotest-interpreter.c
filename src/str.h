#ifndef _SOTEST_STR_H
#define _SOTEST_STR_H

#include <stddef.h>
#include <stdio.h>
#include <string.h>

/// Sliceable nul-terminated string with known length.
///
/// # Note
///
/// `len` does not count for the nul symbol.
typedef struct Str {
    char* ptr;
    size_t len;
} Str;

Str constexpr STR_NULL = {.ptr = nullptr, .len = 0};

/// Construct `Str` from a literal string
#define Str(literal) ((Str) {.ptr = literal, .len = sizeof(literal) - 1})

/// Construct `Str` from a pointer to nul-terminated c-string
inline static Str str_from_ptr(char* ptr) {
    return (Str) {
        .ptr = ptr,
        .len = strlen(ptr),
    };
}

/// Get a substring from a given `Str`
Str str_slice(Str self, size_t start, size_t end);

/// Trim whitespace characters from start
Str str_trim_start(Str self);

/// Trim whitespace characters from end
Str str_trim_end(Str self);

/// Trim whitespace characters from start and from end
Str str_trim(Str self);

/// Check if the given string starts with another
bool str_starts_with(Str self, Str prefix);

/// Check if the given string ends with another
bool str_ends_with(Str self, Str suffix);

/// Write string to a stream
void str_write(Str self, FILE* stream);

typedef struct String {
    Str str;
    /// Internal buffer capacity (includes nul byte)
    size_t cap;
} String;

String constexpr STRING_EMPTY = {.str = STR_NULL, .cap = 0};
size_t constexpr STRING_GROWTH_RATE = 2;
size_t constexpr STRING_INITIAL_CAPACITY = 16;

String string_with_capacity(size_t capacity);

void string_clear(String* self);

void string_push(String* self, char symbol);

char string_pop(String* self);

void string_free(String* self);

typedef enum ReadlineStatus {
    READLINE_SUCCESS = 0,
    READLINE_EOF = EOF,
} ReadlineStatus;

ReadlineStatus string_readline(String* self, FILE* stream);

#endif  // !_SOTEST_STR_H
