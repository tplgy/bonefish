find_package(Boost REQUIRED COMPONENTS system thread)
find_package(Msgpack REQUIRED)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
    ${Boost_INCLUDE_DIRS})

set(SOURCES
    bonefish/messages/message_type.cpp
    bonefish/roles/wamp_role_type.cpp
    bonefish/serialization/msgpack_serializer.cpp
    bonefish/serialization/msgpack_hello_message_serializer.cpp
    bonefish/serialization/msgpack_welcome_message_serializer.cpp
    bonefish/uri.cpp
    bonefish/wamp_router.cpp
    bonefish/wamp_session.cpp
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
    bonefish/messages/hello_message.hpp
    bonefish/messages/message_type.hpp
    bonefish/messages/welcome_message.hpp
    bonefish/roles/wamp_role.hpp
    bonefish/roles/wamp_role_features.hpp
    bonefish/roles/wamp_role_type.hpp
    bonefish/serialization/msgpack_serializer.hpp
    bonefish/serialization/msgpack_hello_message_serializer.hpp
    bonefish/serialization/msgpack_welcome_message_serializer.hpp
    bonefish/serialization/serializer_type.hpp
    bonefish/serialization/wamp_serializer.hpp
    bonefish/uri.hpp
    bonefish/wamp_router.hpp
    bonefish/wamp_routers.hpp
    bonefish/wamp_session.hpp
    bonefish/wamp_transport.hpp
    bonefish/websocket_config.hpp
    bonefish/websocket_data.hpp
    bonefish/websocket_protocol.hpp
    bonefish/websocket_server.hpp
    bonefish/websocket_transport.hpp)

add_library(bonefish STATIC ${SOURCES} ${PUBLIC_HEADERS})
install(FILES ${PUBLIC_HEADERS} DESTINATION include/bonefish)
install(TARGETS bonefish DESTINATION lib)
