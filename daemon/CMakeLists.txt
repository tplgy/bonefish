set(SOURCES bonefish.cpp daemon.cpp daemon_options.cpp)

add_executable(bonefish_bin ${SOURCES})

set_target_properties(bonefish_bin PROPERTIES OUTPUT_NAME bonefish)

add_dependencies(bonefish_bin bonefish)

target_link_libraries(bonefish_bin
    bonefish
    ${Boost_LIBRARIES}
    ${CMAKE_DL_LIBS}
    ${CMAKE_THREAD_LIBS_INIT})

if (RT_FOUND)
    target_link_libraries(bonefish_bin ${RT_LIBRARIES})
endif()
