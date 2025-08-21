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
    return 0 == memcmp(self.ptr + self.len - suffix.len, suffix.ptr, suffix.len);
}

void str_write(Str self, FILE* stream) {
    fwrite((void*) self.ptr, sizeof(*self.ptr), self.len, stream);
}

String string_with_capacity(size_t capacity) {
    if (capacity < STRING_INITIAL_CAPACITY) {
        capacity = STRING_INITIAL_CAPACITY;
    }

    return (String) {
        .str = (Str) {
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

void string_clear(String* self) {
    self->str.len = 0;
}
