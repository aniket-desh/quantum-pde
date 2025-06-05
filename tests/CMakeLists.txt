# Find Google Test
find_package(GTest REQUIRED)

# Add test executables
add_executable(test_solver test_solver.cc)
add_executable(test_tensor_network test_tensor_network.cc)
add_executable(test_eigen test_eigen.cc)

# Link libraries
target_link_libraries(test_solver PRIVATE qps GTest::GTest GTest::Main)
target_link_libraries(test_tensor_network PRIVATE qps GTest::GTest GTest::Main)
target_link_libraries(test_eigen PRIVATE qps GTest::GTest GTest::Main)

# Register tests with colored output
add_test(NAME test_solver COMMAND test_solver --gtest_color=yes)
add_test(NAME test_tensor_network COMMAND test_tensor_network --gtest_color=yes)
add_test(NAME test_eigen COMMAND test_eigen --gtest_color=yes) 