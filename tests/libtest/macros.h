#ifndef _SOTEST_TESTS_MACROS_H
#define _SOTEST_TESTS_MACROS_H

#include "tests.h"

#define TEST(test_name)                                          \
    void __execute_test_##test_name();                           \
    __attribute__((constructor)) void __add_test_##test_name() { \
        tests_add(                                               \
            &TESTS, (Test) {.run = __execute_test_##test_name,   \
                            .name = Str(#test_name),             \
                            .path = Str(__FILE__),               \
                            .line_number = (size_t) __LINE__}    \
        );                                                       \
    }                                                            \
    void __execute_test_##test_name()

#endif  // !_SOTEST_TESTS_MACROS_H
