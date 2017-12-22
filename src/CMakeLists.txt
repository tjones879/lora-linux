set (SRC_DIR ${CMAKE_SOURCE_DIR}/src)

add_library(lora
    ${SRC_DIR}/db.cpp
    ${SRC_DIR}/driver.cpp
    ${SRC_DIR}/init.cpp
    ${SRC_DIR}/message.cpp
    ${SRC_DIR}/packet.cpp
    ${SRC_DIR}/utils.cpp
    ${SRC_DIR}/contact.cpp
    ${SRC_DIR}/compress.cpp
)

add_executable(run ${SRC_DIR}/main.cpp)

target_link_libraries(run lora
    ${SQLITE3_LIBRARY_DEBUG}
    ${sodium_LIBRARY_DEBUG}
    Threads::Threads
    ${ZLIB_LIBRARIES}
    shoco)
