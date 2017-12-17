set (SRC_DIR ${CMAKE_SOURCE_DIR}/src)

add_library(lora
    ${SRC_DIR}/db.cpp
    ${SRC_DIR}/driver.cpp
    ${SRC_DIR}/init.cpp
    ${SRC_DIR}/message.cpp
    ${SRC_DIR}/packet.cpp
    ${SRC_DIR}/utils.cpp
)

add_executable(run ${SRC_DIR}/main.cpp)

target_link_libraries(run lora SQLite3_LIBRARY_DEBUG sodium_LIBRARY_DEBUG)
