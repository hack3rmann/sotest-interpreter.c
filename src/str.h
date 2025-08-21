#ifndef _SOTEST_STR_H
#define _SOTEST_STR_H

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

/// Sliceable possibly non-nul-terminated string with known length.
///
/// # Note
///
/// `len` does not count for the nul symbol if it is present.
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

/// Checks if two strings are equal
bool str_eq(Str self, Str other);

inline static size_t str_hash(Str const* item) {
    auto str = (Str const*) item;

    if (nullptr == str->ptr) {
        return (size_t) 918923801u;
    }

    // return hashmap_hash_default(str->ptr, str->len);
    return 42;
}

inline static int str_compare(Str const* a, Str const* b) {
    auto first = (Str const*) a;
    auto second = (Str const*) b;

    if (nullptr == first->ptr && nullptr == second->ptr) {
        return 0;
    } else if (nullptr == first->ptr) {
        return -1;
    } else if (nullptr == second->ptr) {
        return 1;
    }

    return strcmp(first->ptr, second->ptr);
}

/// Nul-terminated dynamically-sized string
///
/// # Note
///
/// `.str.len` does not count for nul-byte
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

/// # Undefined Behavior
///
/// `self` and `source` should point to non-overlapping chunks of memory
void string_append(String* self, Str source);

typedef enum ReadlineStatus {
    READLINE_SUCCESS = 0,
    READLINE_EOF = EOF,
} ReadlineStatus;

ReadlineStatus string_readline(String* self, FILE* stream);

inline static size_t string_hash(String const* item) {
    return str_hash((Str const*) item);
}

inline static int string_compare(String const* a, String const* b) {
    return str_compare((Str const*) a, (Str const*) b);
}

#endif  // !_SOTEST_STR_H
