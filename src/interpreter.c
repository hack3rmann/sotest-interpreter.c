#include "interpreter.h"
#include "str.h"

#include <dlfcn.h>

ExecutorResult executor_load_library(Executor* self, Str path) {
    string_clear(&self->current_library_path);
    string_append(&self->current_library_path, path);

    if (nullptr != self->library_handle) {
        string_clear(&self->current_function_name);
        self->function = nullptr;
    }

    self->library_handle =
        dlopen(self->current_library_path.str.ptr, RTLD_LAZY);

    if (nullptr == self->library_handle) {
        return (ExecutorResult) {
            .status = EXECUTOR_LOAD_FAILED,
            .dl_error = str_from_ptr(dlerror()),
        };
    }

    return (ExecutorResult) {
        .status = EXECUTOR_SUCCESS,
    };
}

ExecutorResult executor_call_function(Executor* self, Str function_name) {
    if (nullptr == self->library_handle) {
        return (ExecutorResult) {
            .status = EXECUTOR_LIBRARY_NOT_LOADED,
        };
    }

    if (nullptr == self->function || !str_eq(self->current_function_name.str, function_name)) {
        string_clear(&self->current_function_name);
        string_append(&self->current_function_name, function_name);

        self->function = (ExecutorFunction) dlsym(
            self->library_handle, self->current_function_name.str.ptr
        );

        if (nullptr == self->function) {
            string_clear(&self->current_function_name);
            
            return (ExecutorResult) {
                .status = EXECUTOR_FIND_SYMBOL_FAILED,
                .dl_error = str_from_ptr(dlerror()),
            };
        }
    }

    self->function();

    return (ExecutorResult) {
        .status = EXECUTOR_SUCCESS,
    };
}

void executor_free(Executor* self) {
    string_free(&self->current_library_path);
    string_free(&self->current_function_name);

    if (nullptr != self->library_handle) {
        dlclose(self->library_handle);
    }

    *self = EXECUTOR_EMPTY;
}
