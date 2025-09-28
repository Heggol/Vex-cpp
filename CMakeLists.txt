cmake_minimum_required(VERSION 3.21)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(vex VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(vex src/main.cpp src/commands/add.cpp src/commands/init.cpp src/commands/install.cpp src/commands/remove.cpp src/commands/help.cpp src/lockfile.cpp src/package.cpp src/arguments.cpp src/utils/utils.cpp)

find_package(ZLIB REQUIRED)
find_package(httplib REQUIRED)
find_package(OpenSSL REQUIRED)
if (OPENSSL_FOUND)
    set(HTTPLIB_IS_USING_OPENSSL TRUE)
endif()
find_package(nlohmann_json REQUIRED)
find_package(LibArchive REQUIRED)
target_link_libraries(vex PRIVATE ZLIB::ZLIB httplib::httplib nlohmann_json::nlohmann_json LibArchive::LibArchive)
target_link_libraries(vex PUBLIC
        $<$<BOOL:${HTTPLIB_IS_USING_OPENSSL}>:OpenSSL::SSL>
        $<$<BOOL:${HTTPLIB_IS_USING_OPENSSL}>:OpenSSL::Crypto>)

target_compile_definitions(vex PUBLIC
        $<$<BOOL:${HTTPLIB_IS_USING_OPENSSL}>:CPPHTTPLIB_OPENSSL_SUPPORT>
        )

enable_testing()

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
find_package(GTest REQUIRED)
add_executable(tests
    tests/tests.cpp
    tests/test_add.cpp
    tests/test_remove.cpp
    tests/test_install.cpp
    tests/test_init.cpp
    tests/test_help.cpp
    tests/test_integration.cpp
    # source files
    src/commands/add.cpp
    src/commands/init.cpp
    src/commands/install.cpp
    src/commands/remove.cpp
    src/commands/help.cpp
    src/lockfile.cpp
    src/package.cpp
    src/arguments.cpp
    src/utils/utils.cpp
)
target_link_libraries(tests PRIVATE GTest::gtest_main LibArchive::LibArchive httplib::httplib nlohmann_json::nlohmann_json ZLIB::ZLIB)
target_link_libraries(tests PUBLIC
        $<$<BOOL:${HTTPLIB_IS_USING_OPENSSL}>:OpenSSL::SSL>
        $<$<BOOL:${HTTPLIB_IS_USING_OPENSSL}>:OpenSSL::Crypto>)

target_compile_definitions(tests PUBLIC
        $<$<BOOL:${HTTPLIB_IS_USING_OPENSSL}>:CPPHTTPLIB_OPENSSL_SUPPORT>
        )
include(GoogleTest)
gtest_discover_tests(tests)