#include "str.h"

#include <ctype.h>
#include <stdlib.h>

Str str_slice(Str self, size_t start, size_t end) {
    if (end > self.len) {
        end = self.len;
    }

    if (start > self.len) {
        start = self.len;
    }

    if (start > end) {
        return (Str) {
            .ptr = self.ptr,
            .len = 0,
        };
    }

    return (Str) {
        .ptr = self.ptr + start,
        .len = end - start,
    };
}

Str str_trim_start(Str self) {
    // Handle null pointer
    if (nullptr == self.ptr) {
        return self;
    }

    // Handle empty string
    if (0 == self.len) {
        return self;
    }

    // Find the first non-whitespace character
    size_t start = 0;
    while (start < self.len && 0 != isspace(self.ptr[start])) {
        start++;
    }

    // Return substring starting from first non-whitespace character
    return (Str) {
        .ptr = self.ptr + start,
        .len = self.len - start,
    };
}

Str str_trim_end(Str self) {
    // Handle null pointer
    if (nullptr == self.ptr) {
        return self;
    }

    // Handle empty string
    if (0 == self.len) {
        return self;
    }

    // Find the last non-whitespace character
    size_t end = self.len;
    while (end > 0 && 0 != isspace(self.ptr[end - 1])) {
        end--;
    }

    // Return substring ending at last non-whitespace character
    return (Str) {
        .ptr = self.ptr,
        .len = end,
    };
}

Str str_trim(Str self) {
    // Handle null pointer
    if (nullptr == self.ptr) {
        return self;
    }

    // Handle empty string
    if (0 == self.len) {
        return self;
    }

    // Trim start and then end
    Str trimmed_start = str_trim_start(self);
    return str_trim_end(trimmed_start);
}

bool str_starts_with(Str self, Str prefix) {
    if (0 == prefix.len) {
        return true;
    }

    // Handle null pointers
    if (nullptr == self.ptr || nullptr == prefix.ptr) {
        return false;
    }

    // If prefix is longer than self, it can't start with prefix
    if (prefix.len > self.len) {
        return false;
    }

    // Compare the prefix length substring with the prefix
    return 0 == memcmp(self.ptr, prefix.ptr, prefix.len);
}

bool str_ends_with(Str self, Str suffix) {
    if (0 == suffix.len) {
        return true;
    }

    // Handle null pointers
    if (nullptr == self.ptr || nullptr == suffix.ptr) {
        return false;
    }

    // If suffix is longer than self, it can't end with suffix
    if (suffix.len > self.len) {
        return false;
    }

    // Compare the suffix length substring at the end with the suffix
    return 0 ==
           memcmp(self.ptr + self.len - suffix.len, suffix.ptr, suffix.len);
}

void str_write(Str self, FILE* stream) {
    fwrite((void*) self.ptr, sizeof(*self.ptr), self.len, stream);
}

String string_with_capacity(size_t capacity) {
    if (capacity < STRING_INITIAL_CAPACITY) {
        capacity = STRING_INITIAL_CAPACITY;
    }

    return (String) {
        .str =
            (Str) {
                .ptr = calloc(capacity, sizeof(char)),
                .len = 0,
            },
        .cap = capacity,
    };
}

void string_push(String* self, char symbol) {
    if (0 == self->cap) {
        *self = string_with_capacity(STRING_INITIAL_CAPACITY);
    } else if (self->str.len + 1 == self->cap) {
        self->cap *= STRING_GROWTH_RATE * self->cap;
        self->str.ptr = realloc(self->str.ptr, self->cap * sizeof(char));
    }

    self->str.ptr[self->str.len] = symbol;
    self->str.ptr[self->str.len + 1] = '\0';
    self->str.len += 1;
}

char string_pop(String* self) {
    if (0 == self->str.len) {
        return EOF;
    } else {
        auto symbol = self->str.ptr[self->str.len - 1];
        self->str.len -= 1;
        return symbol;
    }
}

void string_free(String* self) {
    free(self->str.ptr);
    *self = STRING_EMPTY;
}

ReadlineStatus string_readline(String* self, FILE* stream) {
    while (true) {
        auto symbol = fgetc(stream);

        if (EOF == symbol) {
            return READLINE_EOF;
        }

        if ('\n' == symbol) {
            break;
        }

        string_push(self, symbol);
    }

    return READLINE_SUCCESS;
}

void string_clear(String* self) { self->str.len = 0; }

void string_append(String* self, Str source) {
    bool needs_allocation = false;

    // Allow the algorithm to find best fit allocation size
    if (0 == self->cap) {
        self->cap = 1;
    }

    while (self->str.len + source.len + 1 > self->cap) {
        needs_allocation = true;
        self->cap *= STRING_GROWTH_RATE;
    }

    if (needs_allocation) {
        if (nullptr == self->str.ptr) {
            self->str.ptr = calloc(self->cap, sizeof(char));
        } else {
            self->str.ptr = realloc(self->str.ptr, self->cap * sizeof(char));
        }
    }

    memcpy(self->str.ptr + self->str.len, source.ptr, source.len);
    self->str.len += source.len;
    self->str.ptr[self->str.len] = '\0';
}

bool str_eq(Str self, Str other) {
    if (self.len != other.len) {
        return false;
    }

    return 0 == memcmp(self.ptr, other.ptr, self.len);
}

inline static size_t unaligned_load(char const* p) {
    size_t result;
    memcpy(&result, p, sizeof(result));
    return result;
}

inline static size_t load_bytes(char const* p, int n) {
    size_t result = 0;
    --n;
    do {
        result = (result << 8) + (unsigned char) (p[n]);
    } while (--n >= 0);
    return result;
}

inline static size_t shift_mix(size_t v) { return v ^ (v >> 47); }

/// Copied from
/// <https://github.com/LeoVen/C-Macro-Collections/blob/99f08e2b6e446e03fa3c7321b58e40ff3796d747/benchmarks/comparisons/hashtable/murmurhash.h#L31>
size_t murmur_hash(void const* ptr, size_t len, size_t seed) {
    static size_t const mul =
        (((size_t) 0xc6a4a793UL) << 32UL) + (size_t) 0x5bd1e995UL;
    char const* const buf = (char const*) (ptr);

    // Remove the bytes not divisible by the sizeof(size_t).  This
    // allows the main loop to process the data as 64-bit integers.
    size_t const len_aligned = len & ~(size_t) 0x7;
    char const* const end = buf + len_aligned;
    size_t hash = seed ^ (len * mul);

    for (char const* p = buf; p != end; p += 8) {
        size_t const data = shift_mix(unaligned_load(p) * mul) * mul;
        hash ^= data;
        hash *= mul;
    }

    if ((len & 0x7) != 0) {
        size_t const data = load_bytes(end, len & 0x7);
        hash ^= data;
        hash *= mul;
    }

    hash = shift_mix(hash) * mul;
    hash = shift_mix(hash);

    return hash;
}

int str_compare(Str const* a, Str const* b) {
    auto first = (Str const*) a;
    auto second = (Str const*) b;

    return strcmp(first->ptr ? first->ptr : "", second->ptr ? second->ptr : "");
}

size_t str_hash(Str const* item) {
    auto str = (Str const*) item;

    size_t constexpr MURMUR_SEED = 8687163864876198286ull;

    return murmur_hash(str->ptr ? str->ptr : "", str->len, MURMUR_SEED);
}
