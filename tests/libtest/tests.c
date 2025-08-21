#include "tests.h"

#include <stdlib.h>

Tests TESTS = TESTS_EMPTY;

void tests_free(Tests* self) {
    free(self->ptr);
    *self = TESTS_EMPTY;
}

void tests_add(Tests* self, Test test) {
    if (0 == self->cap) {
        self->cap = 2;
        self->ptr = malloc(sizeof(*self->ptr) * self->cap);
    } else if (self->len == self->cap) {
        self->cap += self->cap / 2;
        self->ptr = realloc(self->ptr, sizeof(*self->ptr) * self->cap);
    }

    self->ptr[self->len] = test;
    self->len += 1;
}

__attribute__((destructor)) void global_tests_free() {
    tests_free(&TESTS);
}
