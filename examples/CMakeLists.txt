find_package(Boost REQUIRED COMPONENTS system thread)
find_package(Threads)

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/third-party/websocketpp
    ${Boost_INCLUDE_DIRS})

set(SOURCES integration.cpp)

add_executable(integration ${SOURCES})

add_dependencies(integration bonefish)

target_link_libraries(integration
    bonefish
    ${Boost_LIBRARIES}
    ${CMAKE_DL_LIBS}
    ${CMAKE_THREAD_LIBS_INIT})


