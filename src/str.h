#ifndef _SOTEST_STR_H
#define _SOTEST_STR_H

#include <stddef.h>
#include <stdio.h>
#include <string.h>

/// Sliceable nul-terminated string with known length.
///
/// # Note
///
/// `len` does not count for nul symbol.
typedef struct Str {
    char* ptr;
    size_t len;
} Str;

#define Str_NULL ((Str) {.ptr = nullptr, .len = 0})

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

Str str_trim_start(Str self);

Str str_trim_end(Str self);

/// Trim whitespace characters from start and from end
Str str_trim(Str self);

/// Check if the given string starts with another
bool str_starts_with(Str self, Str prefix);

/// Check if the given string ends with another
bool str_ends_with(Str self, Str suffix);

/// Write string to a stream
void str_write(Str self, FILE* stream);

#endif  // !_SOTEST_STR_H
