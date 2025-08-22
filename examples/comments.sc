# Comment heavy script
# This script demonstrates heavy comment usage

# Load libraries
# We're going to load two libraries for testing
use build/libtest1.so # First library

# Call some functions
# From the first library
call foo # First function
call bar # Second function

# Load second library
# This library has some unique functions
use build/libtest2.so # Second library

# Call more functions
# Including one unique to test2
call baz # Common function
call qux # Unique function

# End of script
# Thanks for running this example!