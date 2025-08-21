# Qwen Code Context

This document outlines the requirements and plan for the test task.

## Requirements Summary

- **Task**: Develop a C scripting language interpreter for testing shared libraries (.so files).
- **Language**: C (C23 standard).
- **Platform**: GNU/Linux (x86), POSIX.1-2001 API.
- **Build System**: CMake. Source files must be placed under `src/`.
- **Build Command**: `cmake -B build` to use `build/` as the build directory.
- **Third-Party Libraries**: Allowed for implementing required logic.
- **Testing**: Do not write tests at this stage.

## Detailed Task Description

### Application Functionality

1.  **Script Execution**: Run a script file provided as a command-line argument (`sotest test.sc`).
2.  **Script Syntax**:
    - One command per line.
    - `use <so_name>`: Load a shared library. Handle file not found and not a library errors.
    - `call <func_name>`: Call a function from the loaded library. Handle function not found and library not loaded errors.
    - Syntax errors should produce warnings but not stop execution. Execution errors terminate the script.
    - Optional: Support comments (`#`, `;`) and line continuations (`\`).
    - Optional: Interactive mode for direct command entry.
3.  **Library Requirements**: Test libraries are C programs exporting functions with the prototype `void example(void);`.
4.  **Error Handling**: Distinguish between syntax errors (warnings) and execution errors (termination).

### Deliverables

- Application source code with compilation instructions (CMake).
- Self-testing results including console output, the test script, and test library sources.