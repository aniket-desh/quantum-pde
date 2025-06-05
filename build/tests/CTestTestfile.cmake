# CMake generated Testfile for 
# Source directory: /Users/aniket/Development/quantum-pde/tests
# Build directory: /Users/aniket/Development/quantum-pde/build/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(test_solver "/Users/aniket/Development/quantum-pde/build/tests/test_solver" "--gtest_color=yes")
set_tests_properties(test_solver PROPERTIES  _BACKTRACE_TRIPLES "/Users/aniket/Development/quantum-pde/tests/CMakeLists.txt;15;add_test;/Users/aniket/Development/quantum-pde/tests/CMakeLists.txt;0;")
add_test(test_tensor_network "/Users/aniket/Development/quantum-pde/build/tests/test_tensor_network" "--gtest_color=yes")
set_tests_properties(test_tensor_network PROPERTIES  _BACKTRACE_TRIPLES "/Users/aniket/Development/quantum-pde/tests/CMakeLists.txt;16;add_test;/Users/aniket/Development/quantum-pde/tests/CMakeLists.txt;0;")
add_test(test_eigen "/Users/aniket/Development/quantum-pde/build/tests/test_eigen" "--gtest_color=yes")
set_tests_properties(test_eigen PROPERTIES  _BACKTRACE_TRIPLES "/Users/aniket/Development/quantum-pde/tests/CMakeLists.txt;17;add_test;/Users/aniket/Development/quantum-pde/tests/CMakeLists.txt;0;")
