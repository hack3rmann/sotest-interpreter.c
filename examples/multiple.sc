# Multiple library example
# Load both test libraries and call functions from each

use build/libtest1.so # Load the first library
call foo # Call foo from test1
call bar # Call bar from test1

use build/libtest2.so # Load the second library
call foo # Call foo from test2 (should still use test1's foo since it was loaded first)
call qux # Call qux which is unique to test2