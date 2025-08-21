#include "libtest/macros.h"

#include <assert.h>
#include <parse.h>
#include <interpreter.h>

TEST(parse_prefix) {
    auto r = parse_prefix(Str("foobar"), Str("foo"));

    assert(r.has_value);
    assert(str_eq(r.value, Str("foo")));
    assert(str_eq(r.tail, Str("bar")));

    r = parse_prefix(Str("foobar"), Str("baz"));

    assert(!r.has_value);
    assert(str_eq(r.tail, Str("foobar")));
}

TEST(parse_path) {
    auto r = parse_path(Str("/path/to/a/thing tail"));

    assert(r.has_value);
    assert(str_eq(r.value, Str("/path/to/a/thing")));
    assert(str_eq(r.tail, Str(" tail")));

    r = parse_path(Str(" /not/a/path"));

    assert(!r.has_value);
    assert(str_eq(r.tail, Str(" /not/a/path")));
}

TEST(parse_comment) {
    auto r = parse_comment(Str("# comment all the way up to a newline\n"));

    assert(r.has_value);
    assert(str_eq(r.value, Str(" comment all the way up to a newline")));
    assert(str_eq(r.tail, Str("\n")));

    r = parse_comment(Str("  # comment after a whitespace"));

    assert(!r.has_value);
    assert(str_eq(r.tail, Str("  # comment after a whitespace")));
}

TEST(parse_function_name) {
    auto r = parse_function_name(Str("a_function_name-tail"));

    assert(r.has_value);
    assert(str_eq(r.value, Str("a_function_name")));
    assert(str_eq(r.tail, Str("-tail")));

    r = parse_function_name(Str("42foo"));

    assert(!r.has_value);
    assert(str_eq(r.tail, Str("42foo")));
}

TEST(parse_command) {
    auto r = command_parse(Str("use path/to/library # comment"));

    assert(r.has_value);
    assert(r.value.type == COMMAND_TYPE_USE);
    assert(str_eq(r.value.content, Str("path/to/library")));
    assert(str_eq(r.tail, Str(" # comment")));

    r = command_parse(Str("call function_name # comment"));

    assert(r.has_value);
    assert(r.value.type == COMMAND_TYPE_CALL);
    assert(str_eq(r.value.content, Str("function_name")));
    assert(str_eq(r.tail, Str(" # comment")));

    r = command_parse(Str("use @broken/path # comment"));

    assert(!r.has_value);
    assert(str_eq(r.tail, Str("use @broken/path # comment")));

    r = command_parse(Str("call 404_function_not_found # comment"));

    assert(!r.has_value);
    assert(str_eq(r.tail, Str("call 404_function_not_found # comment")));

    r = command_parse(Str("exit # comment"));

    assert(!r.has_value);
    assert(str_eq(r.tail, Str("exit # comment")));

    r = command_parse(Str("use path\nnot a command"));
    
    assert(r.has_value);
    assert(r.value.type == COMMAND_TYPE_USE);
    assert(str_eq(r.value.content, Str("path")));
    assert(str_eq(r.tail, Str("\nnot a command")));
}

TEST(parse_command_line) {
    auto r = command_line_parse(Str("call function # with a comment\nwith a tail"));

    assert(r.has_value);
    assert(r.value.has_command);
    assert(r.value.command.type == COMMAND_TYPE_CALL);
    assert(str_eq(r.value.command.content, Str("function")));
    assert(str_eq(r.value.comment, Str(" with a comment")));
    assert(str_eq(r.tail, Str("\nwith a tail")));

    r = command_line_parse(Str("use path/to/lib\nno comment"));

    assert(r.has_value);
    assert(r.value.has_command);
    assert(r.value.command.type == COMMAND_TYPE_USE);
    assert(str_eq(r.value.command.content, Str("path/to/lib")));
    assert(str_eq(r.value.comment, Str("")));
    assert(str_eq(r.tail, Str("\nno comment")));
}
