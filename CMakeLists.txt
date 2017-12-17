cmake_minimum_required (VERSION 3.2)
project (lora)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules")
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_FLAGS "--std=c++17 -stdlib=libc++ -g -Wall -Wextra -Wno-c++98-compat")

include_directories(${CMAKE_SOURCE_DIR}/)
include_directories(${CMAKE_SOURCE_DIR}/lib)

find_package(sodium REQUIRED)
find_package(SQLite3 REQUIRED)

add_subdirectory(src)
add_subdirectory(test)

file(GLOB_RECURSE ALL_SOURCE_FILES src/*.cpp)
file(GLOB_RECURSE ALL_HEADER_FILES inc/*.h inc/*.hpp)

find_program(FORMAT_PATH clang-format)
if(FORMAT_PATH)
    add_custom_target(format
        COMMAND ${FORMAT_PATH} -i ${ALL_SOURCE_FILES} ${ALL_HEADER_FILES})
endif()

find_program(TIDY_PATH clang-tidy)
if(TIDY_PATH)
    add_custom_target(tidy COMMAND ${TIDY_PATH} ${ALL_SOURCE_FILES} -fix -p ./)
endif()
