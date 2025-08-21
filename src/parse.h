#ifndef _SOTEST_PARSE_H
#define _SOTEST_PARSE_H

#include "str.h"

#include <stdint.h>

typedef struct ParseResult {
    bool has_value;
    Str tail;
    /// Available only if `success == true`
    Str value;
} ParseResult;

ParseResult parse_prefix(Str source, Str prefix);

ParseResult parse_path(Str source);

ParseResult parse_comment(Str source);

ParseResult parse_function_name(Str source);

#endif  // !_SOTEST_PARSE_H
