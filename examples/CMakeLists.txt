set(SOURCES integration.cpp)

add_executable(integration ${SOURCES})

add_dependencies(integration bonefish)

target_link_libraries(integration
    bonefish
    ${Boost_LIBRARIES}
    ${CMAKE_DL_LIBS}
    ${CMAKE_THREAD_LIBS_INIT})
