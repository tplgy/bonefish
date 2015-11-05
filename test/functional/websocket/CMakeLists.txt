set(SOURCES client.cpp)

add_executable(client ${SOURCES})

target_link_libraries(client
    ${Boost_LIBRARIES}
    ${CMAKE_DL_LIBS}
    ${CMAKE_THREAD_LIBS_INIT})

if (RT_FOUND)
	target_link_libraries(client ${RT_LIBRARIES})
endif()
