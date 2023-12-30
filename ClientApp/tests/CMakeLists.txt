# hoor_buch/ClientApp/tests/CMakeLists.txt

# Find and include the CTest module
include(CTest)

# Define your test executable and source files
# add_executable(testClientApp test1.cpp test2.cpp) # List your test files here
add_executable(testClientApp test1.cpp) # List your test files here

# Link the test executable against the libraries or objects from your main application if needed
# target_link_libraries(testClientApp PRIVATE {YourMainAppLibrariesOrObjects})
target_link_libraries(testClientApp gtest gtest_main)

# Add tests to the project
add_test(NAME TestClientApp COMMAND testClientApp)