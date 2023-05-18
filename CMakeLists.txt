cmake_minimum_required(VERSION 3.16)
project(CTFTools)

set(CMAKE_CXX_STANDARD 17)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

enable_testing()

option(SANITIZERS "Build with sanitizers" OFF)

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

target_include_directories(ctftools PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_link_libraries(ctftools PUBLIC
    curl
    pthread
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

install(TARGETS ctftools
        EXPORT CTFToolsConfig
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(FILES include/ctftools/ctftools.hpp DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT CTFToolsConfig
        FILE CTFToolsConfig.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/CTFTools
)

# install(TARGETS ctftools
#     EXPORT CTFToolsConfig DESTINATION lib/cmake/CTFTools
#     LIBRARY DESTINATION /usr/lib
#     ARCHIVE DESTINATION /usr/lib
# )

# install(FILES include/ctftools/ctftools.hpp
#     DESTINATION /usr/include)

# install(FILES "${CMAKE_CURRENT_BINARY_DIR}/CTFToolsConfig.cmake" DESTINATION lib/cmake/CTFTools)
