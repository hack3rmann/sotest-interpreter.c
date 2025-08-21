#include "tests.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <sys/wait.h>

#define GREEN_START "\033[1;32m"
#define RED_START "\033[1;31m"
#define COLOR_END "\033[0m"

int run_test(Test test) {
    auto pid = fork();

    printf(
        "test %s - %s (line %zu)", test.path.ptr, test.name.ptr,
        test.line_number
    );

    int result = EXIT_SUCCESS;

    if (0 == pid) {
        test.run();
        _exit(EXIT_SUCCESS);
    } else if (pid > 0) {
        int status = 0;
        assert(pid == waitpid(pid, &status, 0));

        if ((WIFEXITED(status) && EXIT_SUCCESS == WEXITSTATUS(status))) {
            result = EXIT_SUCCESS;
            printf(" ... " GREEN_START "ok" COLOR_END);
        } else {
            result = EXIT_FAILURE;
            printf(" ... " RED_START "failed" COLOR_END);
        }

        putchar('\n');
    } else {
        abort();
    }

    return result;
}

int main() {
    printf("running %zu tests\n", TESTS.len);

    size_t n_passed = 0;
    size_t n_failed = 0;

    for (size_t i = 0; i < TESTS.len; ++i) {
        if (EXIT_SUCCESS == run_test(TESTS.ptr[i])) {
            n_passed += 1;
        } else {
            n_failed += 1;
        }
    }

    auto test_result = 0 == n_failed ? GREEN_START "ok" COLOR_END
                                     : RED_START "error" COLOR_END;

    printf(
        "\ntest result: %s. %zu passed; %zu failed\n", test_result, n_passed,
        n_failed
    );
}
