set(SOURCES client.cpp)

add_executable(client ${SOURCES})

target_link_libraries(client
    asio
    Threads::Threads
    ${CMAKE_DL_LIBS})

if (RT_FOUND)
	target_link_libraries(client ${RT_LIBRARIES})
endif()
