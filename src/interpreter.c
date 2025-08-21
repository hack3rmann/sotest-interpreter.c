#include "interpreter.h"
#include "str.h"

#include <dlfcn.h>

typedef void* LibraryHandle;

#define CMC_EXT_ITER

#define K String
#define V LibraryHandle
#define SNAME LibraryMap
#define PFX library_map

#include <cmc/hashmap.h>

#define K String
#define V ExecutorFunction
#define SNAME FunctionMap
#define PFX function_map

#include <cmc/hashmap.h>

static int local_string_compare(String a, String b) {
    return string_compare(&a, &b);
}

static void local_string_free(String string) { string_free(&string); }

static size_t local_string_hash(String string) { return string_hash(&string); }

static void local_handle_free(LibraryHandle handle) { dlclose(handle); }

struct FunctionMap_fkey FUNCTION_MAP_FKEY = {
    .cmp = local_string_compare,
    .free = local_string_free,
    .hash = local_string_hash,
};

struct FunctionMap_fval FUNCTION_MAP_FVAL = {};

struct LibraryMap_fkey LIBRARY_MAP_FKEY = {
    .cmp = local_string_compare,
    .free = local_string_free,
    .hash = local_string_hash,
};

struct LibraryMap_fval LIBRARY_MAP_FVAL = {
    .free = local_handle_free,
};

Executor executor_new() {
    return (Executor) {
        .functions =
            function_map_new(32, 0.5, &FUNCTION_MAP_FKEY, &FUNCTION_MAP_FVAL),
        .libraries =
            library_map_new(32, 0.5, &LIBRARY_MAP_FKEY, &LIBRARY_MAP_FVAL),
    };
}

ExecutorResult executor_load_library(Executor* self, Str path) {
    if (library_map_contains(self->libraries, (String) {.str = path})) {
        return (ExecutorResult) {
            .status = EXECUTOR_SUCCESS,
        };
    }

    auto path_copy = STRING_EMPTY;
    string_append(&path_copy, path);

    auto handle = dlopen(path_copy.str.ptr, RTLD_LAZY);

    if (nullptr == handle) {
        string_free(&path_copy);

        return (ExecutorResult) {
            .dl_error = str_from_ptr(dlerror()),
            .status = EXECUTOR_LOAD_FAILED,
        };
    }

    library_map_insert(self->libraries, path_copy, handle);

    return (ExecutorResult) {
        .status = EXECUTOR_SUCCESS,
    };

    return (ExecutorResult) {};
}

ExecutorResult executor_call_function(Executor* self, Str function_name) {
    auto function =
        function_map_get(self->functions, (String) {.str = function_name});

    if (nullptr != function) {
        function();

        return (ExecutorResult) {
            .status = EXECUTOR_SUCCESS,
        };
    }

    auto name_copy = STRING_EMPTY;
    string_append(&name_copy, function_name);

    auto result = (ExecutorResult) {};

    for (auto it = library_map_iter_start(self->libraries);
         !library_map_iter_at_end(&it); library_map_iter_next(&it))
    {
        auto handle = library_map_iter_value(&it);
        auto function = (ExecutorFunction) dlsym(handle, name_copy.str.ptr);

        if (nullptr == function) {
            result.dl_error = str_from_ptr(dlerror());
            result.status = EXECUTOR_FIND_SYMBOL_FAILED;
            continue;
        }

        function();
        function_map_insert(self->functions, name_copy, function);

        return (ExecutorResult) {
            .status = EXECUTOR_SUCCESS,
        };
    }

    if (0 == library_map_count(self->libraries)) {
        result.status = EXECUTOR_LIBRARY_NOT_LOADED;
        result.dl_error = Str("no library loaded");
    }

    return result;
}

void executor_free(Executor* self) {
    library_map_free(self->libraries);
    function_map_free(self->functions);

    *self = executor_new();
}
