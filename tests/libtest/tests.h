#ifndef _SOTEST_TESTS_H
#define _SOTEST_TESTS_H

#include <str.h>
#include <stddef.h>

typedef void (*TestFunction)();

typedef struct Test {
    TestFunction run;
    Str name;
    Str path;
    size_t line_number;
} Test;

typedef struct Tests {
    Test* ptr;
    size_t len;
    size_t cap;
} Tests;

Tests constexpr TESTS_EMPTY = (Tests) {.ptr = nullptr, .len = 0, .cap = 0};

extern Tests TESTS;

void tests_add(Tests* self, Test test);

void tests_free(Tests* self);

#endif  // !_SOTEST_TESTS_H
