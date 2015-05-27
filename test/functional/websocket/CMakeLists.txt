find_package(Threads)
find_package(Boost REQUIRED COMPONENTS random system thread)

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/third-party/websocketpp
    ${Boost_INCLUDE_DIRS})

link_libraries(
    ${Boost_LIBRARIES}
    ${CMAKE_DL_LIBS}
    ${CMAKE_THREAD_LIBS_INIT}
)

set(SOURCES client.cpp)

add_executable(client ${SOURCES})
