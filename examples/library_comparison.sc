# Library comparison script
# Compare functions with the same name in different libraries

use build/libtest1.so # Load first library
call foo # Call foo from test1
call bar # Call bar from test1

use build/libtest2.so # Load second library
call foo # Call foo again (should still use test1's version)
call bar # Call bar again (should still use test1's version)