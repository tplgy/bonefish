find_package(Boost REQUIRED COMPONENTS system thread)
find_package(Msgpack REQUIRED)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
    ${Boost_INCLUDE_DIRS})

set(SOURCES
    bonefish/message_code.cpp
    #bonefish/role.cpp
    bonefish/router.cpp
    bonefish/session.cpp
    bonefish/uri.cpp
    bonefish/websocket_protocol.cpp
    bonefish/websocket_server.cpp)

set(PUBLIC_HEADERS
    bonefish/identifier/publication_id.hpp
    bonefish/identifier/publication_id_generator.hpp
    bonefish/identifier/random_id.hpp
    bonefish/identifier/random_id_generator.hpp
    bonefish/identifier/registration_id.hpp
    bonefish/identifier/registration_id_generator.hpp
    bonefish/identifier/request_id.hpp
    bonefish/identifier/request_id_generator.hpp
    bonefish/identifier/sequential_id.hpp
    bonefish/identifier/sequential_id_generator.hpp
    bonefish/identifier/session_id.hpp
    bonefish/identifier/session_id_generator.hpp
    bonefish/identifier/subscription_id.hpp
    bonefish/identifier/subscription_id_generator.hpp
    bonefish/message_code.hpp
    bonefish/role.hpp
    bonefish/router.hpp
    bonefish/session.hpp
    bonefish/uri.hpp
    bonefish/websocket_config.hpp
    bonefish/websocket_data.hpp
    bonefish/websocket_protocol.hpp
    bonefish/websocket_server.hpp)

set(PRIVATE_HEADERS
    "")

add_library(bonefish STATIC ${SOURCES} ${PUBLIC_HEADERS} ${PRIVATE_HEADERS})
install(FILES ${PUBLIC_HEADERS} DESTINATION include/bonefish)
install(TARGETS bonefish DESTINATION lib)
