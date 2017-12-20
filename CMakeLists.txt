cmake_minimum_required (VERSION 3.2)
project (lora C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules")
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_FLAGS "--std=c++17 -stdlib=libc++ -g -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-zero-as-null-pointer-constant -Wno-padded -Wno-exit-time-destructors -Wno-missing-prototypes -Wno-shorten-64-to-32")

find_package(sodium REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(Threads REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/)
include_directories(${CMAKE_SOURCE_DIR}/lib)
include_directories(${SQLite3_INCLUDE_DIR})
include_directories(${sobium_INCLUDE_DIRS})

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
