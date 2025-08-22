# SoTest - Shared Library Reader Language Interpreter

SoTest is a simple interpreter that can load shared libraries (.so files)
and call functions from them using a custom scripting language. It provides
an easy way to interact with dynamically loaded libraries without writing C code.

**Note: This software only works on Linux systems due to its use of Linux-specific shared library loading mechanisms.**

## Features

- Load shared libraries at runtime
- Call functions from loaded libraries
- Simple scripting language with `use` and `call` commands
- Interactive mode for experimentation
- Error handling that continues execution even on failures
- Comment support in scripts
- Support for reading from stdin/pipe
- Intelligent caching of loaded libraries and function symbols for performance

## Building the Program

To build the program, you need CMake and a C compiler that supports C23 standard.
This will download required libraries via `CPM.cmake` and build the interpreter with tests.

```bash
cmake -B build
cmake --build build
```

This will create the main executable `sotest` and the test libraries
`libtest1.so` and `libtest2.so` under the `build` directory.

## Running the Program

### Command Line Usage

Use the help message to better understand how to run the interpreter.

```bash
build/sotest --help
```

### Interactive Mode

If no script file is provided, SoTest will start in interactive mode:

```bash
build/sotest
```

In interactive mode, you can enter commands directly:

```
 >>> use build/libtest1.so
 >>> call foo
 >>> exit
```

Commands in interactive mode:

- `use <library_path>`: Load a shared library
- `call <function_name>`: Call a function from a loaded library
- `exit`: Exit the interpreter (unique for the interactive mode)

To exit interactive mode, you can also press Ctrl+C or Ctrl+D.

### Running Scripts

You can run a script file directly:

```bash
build/sotest examples/simple.sc
```

### Stdin/Pipe Behavior

`sotest` can also read scripts from stdin, allowing you to pipe commands:

```bash
cat examples/simple.sc | build/sotest
echo -e "use build/libtest1.so\ncall foo" | build/sotest
```

When reading from pipe, the interpreter will not show the `>>>` prompt.

## Script Language Syntax

### Commands

1. **use**: Load a shared library `use <library_path>`.
2. **call**: Call a function from a loaded library `call <function_name>`.

### Comments

Comments start with `#` and continue to the end of the line:

```
# This is a comment
use build/libtest1.so # This loads the library
```

### Empty Lines

Empty lines are ignored and can be used to separate sections of your script.

### Example Script

```
# Load a library
use build/libtest1.so

# Call functions
call foo
call bar

# Load another library
use build/libtest2.so
call qux
```

## Error Handling

The interpreter is designed to continue execution even when errors occur.
If a command fails (e.g., trying to load a non-existent library or call a
non-existent function), the interpreter will display an error message but
continue processing the next line.

## Caching Behavior

The interpreter implements intelligent caching for performance optimization:

- **Library Caching**: Once a library is loaded with the `use` command, it
    remains loaded in memory for the duration of the interpreter session.
    This eliminates the overhead of repeatedly loading the same library.
- **Function Caching**: When a function is called for the first time, the
    interpreter looks up the symbol in all loaded libraries and caches the
    function pointer. Subsequent calls to the same function name use the
    cached pointer, avoiding repeated symbol lookups.

This design choice trades a small amount of memory for significant performance
improvements, especially when calling functions repeatedly. Libraries are never
unloaded during the interpreter session, ensuring that all cached function
pointers remain valid.

## Testing

The project includes a comprehensive test suite. To run the tests:

```bash
build/test
```

This will run all tests including:

- Parser tests
- Executor tests
- Library loading and function calling tests
- Error handling tests

## Examples

Check out the `examples/` directory for sample scripts:

- `simple.sc`: Basic library loading and function calling
- `multiple.sc`: Using multiple libraries
- `complex.sc`: Complex script with comments
- `error_handling.sc`: Demonstrates error handling with failing commands
- `errors.sc`: Comprehensive error testing
- `comments.sc`: Heavy comment usage example
- `library_comparison.sc`: Compare functions with same names in different libraries

## Project Structure

- `src/`: Source code for the interpreter
- `tests/`: Test suite
- `examples/`: Example scripts
- `build/`: Build directory (created during build process)

## Libraries

The test libraries (`libtest1.so` and `libtest2.so`) provide sample functions:

- `foo()`: Prints "foo() from test1" or "foo() from test2"
- `bar()`: Prints "bar() from test1" or "bar() from test2"
- `baz()`: Prints "baz() from test1" or "baz() from test2"
- `qux()`: Prints "qux() is unique for test2" (only in test2)

## Platform Support

This software is designed specifically for Linux systems. It uses Linux-specific
mechanisms for shared library loading (dlopen, dlsym, dlclose) and will not work
on other operating systems like Windows or macOS without significant modifications.
