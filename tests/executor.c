#include "libtest/macros.h"

#include <assert.h>
#include <interpreter.h>
#include <parse.h>

TEST(executor_new_and_free) {
    auto executor = executor_new();
    executor_free(&executor);
}

TEST(executor_load_library) {
    auto executor = executor_new();
    auto r = executor_load_library(&executor, Str("build/libtest1.so"));

    assert(r.status == EXECUTOR_SUCCESS);

    executor_free(&executor);
}

TEST(executor_load_library_not_found) {
    auto executor = executor_new();
    auto r = executor_load_library(&executor, Str("build/nonexistent.so"));

    assert(r.status == EXECUTOR_LOAD_FAILED);
    assert(r.dl_error.ptr != nullptr);
    assert(r.dl_error.len > 0);

    executor_free(&executor);
}

TEST(executor_call_function) {
    auto executor = executor_new();

    auto load_result =
        executor_load_library(&executor, Str("build/libtest1.so"));
    assert(load_result.status == EXECUTOR_SUCCESS);

    auto call_result = executor_call_function(&executor, Str("foo"));
    assert(call_result.status == EXECUTOR_SUCCESS);

    executor_free(&executor);
}

TEST(executor_call_function_not_found) {
    auto executor = executor_new();

    auto load_result =
        executor_load_library(&executor, Str("build/libtest1.so"));
    assert(load_result.status == EXECUTOR_SUCCESS);

    auto call_result =
        executor_call_function(&executor, Str("nonexistent_function"));
    assert(call_result.status == EXECUTOR_FIND_SYMBOL_FAILED);
    assert(call_result.dl_error.ptr != nullptr);
    assert(call_result.dl_error.len > 0);

    executor_free(&executor);
}

TEST(executor_call_function_no_library_loaded) {
    auto executor = executor_new();

    auto call_result = executor_call_function(&executor, Str("foo"));
    assert(call_result.status == EXECUTOR_LIBRARY_NOT_LOADED);
    assert(str_eq(call_result.dl_error, Str("no library loaded")));

    executor_free(&executor);
}

TEST(executor_command_line_execution) {
    auto executor = executor_new();

    auto use_result = command_line_parse(Str("use build/libtest1.so\n"));
    assert(use_result.has_value);
    assert(use_result.value.has_command);
    assert(use_result.value.command.type == COMMAND_TYPE_USE);

    auto load_result =
        executor_load_library(&executor, use_result.value.command.content);
    assert(load_result.status == EXECUTOR_SUCCESS);

    auto call_result = command_line_parse(Str("call foo\n"));
    assert(call_result.has_value);
    assert(call_result.value.has_command);
    assert(call_result.value.command.type == COMMAND_TYPE_CALL);

    auto exec_result =
        executor_call_function(&executor, call_result.value.command.content);
    assert(exec_result.status == EXECUTOR_SUCCESS);

    executor_free(&executor);
}

TEST(executor_multiple_libraries) {
    auto executor = executor_new();

    auto r = executor_load_library(&executor, Str("build/libtest1.so"));
    assert(r.status == EXECUTOR_SUCCESS);

    r = executor_load_library(&executor, Str("build/libtest2.so"));
    assert(r.status == EXECUTOR_SUCCESS);

    r = executor_call_function(&executor, Str("foo"));
    assert(r.status == EXECUTOR_SUCCESS);

    r = executor_call_function(&executor, Str("qux"));
    assert(r.status == EXECUTOR_SUCCESS);

    executor_free(&executor);
}

TEST(executor_script_execution) {
    auto executor = executor_new();

    Str constexpr SCRIPT =
        Str("use build/libtest1.so # Load first library\n"
            "call foo # Call function from first library\n"
            "use build/libtest2.so # Load second library\n"
            "call qux # Call function unique to second library\n");

    auto r = command_line_parse(SCRIPT);

    assert(r.has_value);
    assert(r.value.has_command);
    assert(r.value.command.type == COMMAND_TYPE_USE);
    assert(str_eq(r.value.comment, Str(" Load first library")));

    auto exec = executor_load_library(&executor, r.value.command.content);

    assert(exec.status == EXECUTOR_SUCCESS);

    r = command_line_parse(str_trim_start(r.tail));

    assert(r.has_value);
    assert(r.value.has_command);
    assert(r.value.command.type == COMMAND_TYPE_CALL);
    assert(str_eq(r.value.comment, Str(" Call function from first library")));

    exec = executor_call_function(&executor, r.value.command.content);

    assert(exec.status == EXECUTOR_SUCCESS);

    r = command_line_parse(str_trim_start(r.tail));

    assert(r.has_value);
    assert(r.value.has_command);
    assert(r.value.command.type == COMMAND_TYPE_USE);
    assert(str_eq(r.value.comment, Str(" Load second library")));

    exec = executor_load_library(&executor, r.value.command.content);

    assert(exec.status == EXECUTOR_SUCCESS);

    r = command_line_parse(str_trim_start(r.tail));

    assert(r.has_value);
    assert(r.value.has_command);
    assert(r.value.command.type == COMMAND_TYPE_CALL);
    assert(
        str_eq(r.value.comment, Str(" Call function unique to second library"))
    );

    exec = executor_call_function(&executor, r.value.command.content);

    assert(exec.status == EXECUTOR_SUCCESS);

    executor_free(&executor);
}

TEST(executor_load_same_library_twice) {
    auto executor = executor_new();

    auto r = executor_load_library(&executor, Str("build/libtest1.so"));
    assert(r.status == EXECUTOR_SUCCESS);

    r = executor_load_library(&executor, Str("build/libtest1.so"));
    assert(r.status == EXECUTOR_SUCCESS);

    executor_free(&executor);
}

TEST(executor_malformed_paths) {
    auto executor = executor_new();

    auto r = executor_load_library(&executor, Str(""));
    assert(r.status == EXECUTOR_LOAD_FAILED);

    r = executor_load_library(&executor, Str("build/"));
    assert(r.status == EXECUTOR_LOAD_FAILED);

    executor_free(&executor);
}
