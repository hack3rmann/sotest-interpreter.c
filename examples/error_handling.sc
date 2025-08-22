# Error handling example
# This script demonstrates what happens with invalid commands

# Valid commands
use build/libtest1.so
call foo

# These will fail but execution continues
use nonexistent.so # This will fail to load
call nonexistent_function # This will fail to find the function

# More valid commands to show execution continues
call bar # This should still work