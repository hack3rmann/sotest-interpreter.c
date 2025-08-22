# Comprehensive error testing script
# This script tests various error conditions

# Valid commands first
use build/libtest1.so
call foo

# Test loading non-existent library
use build/nonexistent.so
call nonexistent_function

# Test calling non-existent function
call another_nonexistent_function

# Load another valid library
use build/libtest2.so
call qux

# Try to load a directory as a library
use build/

# Try to load with empty path
use 

# Valid command at the end to show execution continues
call bar