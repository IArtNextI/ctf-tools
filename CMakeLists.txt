cmake_minimum_required(VERSION 3.16)
project(CTFTools)

set(CMAKE_CXX_STANDARD 17)

enable_testing()

option(SANITIZERS "Build with sanitizers" ON)

if(SANITIZERS)
    message("Sanitizers are enabled. Building with -fsanitize=address -fsanitize=undefined -fsanitize=leak -fno-sanitize-recover")
    add_compile_options(-fsanitize=address -fsanitize=undefined -fsanitize=leak -fno-sanitize-recover -g)
    add_link_options(-fsanitize=address -fsanitize=undefined -fsanitize=leak -fno-sanitize-recover -g)
else()
    message("Sanitizers are disabled")
endif()

add_library(ctftools
    src/ctftools.cpp
)

target_include_directories(ctftools PUBLIC include)

target_link_libraries(ctftools PUBLIC
    curl
)

find_package(GTest CONFIG REQUIRED)

add_executable(test_ctftools
    tests/test_main.cpp
    tests/test_easy.cpp
)

target_link_libraries(test_ctftools
    gtest
    pthread
    ctftools
)

add_test(NAME TestCTFTools COMMAND test_ctftools)
