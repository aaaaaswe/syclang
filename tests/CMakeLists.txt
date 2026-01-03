# Tests CMakeLists.txt

enable_testing()

# Test runner
add_executable(test_runner
    main_test.cpp
)

target_link_libraries(test_runner syclang_lib)

# Add tests
add_test(NAME LexerTests COMMAND test_runner)
add_test(NAME ParserTests COMMAND test_runner)
add_test(NAME IRTests COMMAND test_runner)
