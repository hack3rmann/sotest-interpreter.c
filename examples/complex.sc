# Complex example with comments and empty lines

# This is a more complex script demonstrating various features
use build/libtest1.so # Load first library

call foo # Call first function
call bar # Call second function

# Now load the second library
use build/libtest2.so # Load second library

# Call functions from both libraries
call baz # This exists in both, should use test1's version
call qux # This exists only in test2

# End of script