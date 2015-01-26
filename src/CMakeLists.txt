find_package(Boost REQUIRED COMPONENTS system thread)
find_package(Msgpack REQUIRED)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
    ${Boost_INCLUDE_DIRS})

set(SOURCES
    bonefish/broker/wamp_broker.cpp
    bonefish/messages/wamp_message_type.cpp
    bonefish/roles/wamp_role_type.cpp
    bonefish/serialization/msgpack_serializer.cpp
    bonefish/serialization/msgpack_goodbye_message_serializer.cpp
    bonefish/serialization/msgpack_hello_message_serializer.cpp
    bonefish/serialization/msgpack_welcome_message_serializer.cpp
    bonefish/serialization/msgpack_subscribe_message_serializer.cpp
    bonefish/serialization/msgpack_subscribed_message_serializer.cpp
    bonefish/serialization/msgpack_unsubscribe_message_serializer.cpp
    bonefish/serialization/msgpack_unsubscribed_message_serializer.cpp
    bonefish/wamp_dealer.cpp
    bonefish/wamp_router.cpp
    bonefish/wamp_session_state.cpp
    bonefish/wamp_uri.cpp
    bonefish/websocket_protocol.cpp
    bonefish/websocket_server.cpp)

set(PUBLIC_HEADERS
    bonefish/broker/wamp_broker.hpp
    bonefish/broker/wamp_broker_subscription.hpp
    bonefish/broker/wamp_broker_topic.hpp
    bonefish/identifiers/wamp_publication_id.hpp
    bonefish/identifiers/wamp_publication_id_generator.hpp
    bonefish/identifiers/wamp_random_id.hpp
    bonefish/identifiers/wamp_random_id_generator.hpp
    bonefish/identifiers/wamp_registration_id.hpp
    bonefish/identifiers/wamp_registration_id_generator.hpp
    bonefish/identifiers/wamp_request_id.hpp
    bonefish/identifiers/wamp_request_id_generator.hpp
    bonefish/identifiers/wamp_sequential_id.hpp
    bonefish/identifiers/wamp_sequential_id_generator.hpp
    bonefish/identifiers/wamp_session_id.hpp
    bonefish/identifiers/wamp_session_id_generator.hpp
    bonefish/identifiers/wamp_subscription_id.hpp
    bonefish/identifiers/wamp_subscription_id_generator.hpp
    bonefish/messages/wamp_abort_message.hpp
    bonefish/messages/wamp_error_message.hpp
    bonefish/messages/wamp_goodbye_message.hpp
    bonefish/messages/wamp_hello_message.hpp
    bonefish/messages/wamp_invocation_message.hpp
    bonefish/messages/wamp_message.hpp
    bonefish/messages/wamp_message_type.hpp
    bonefish/messages/wamp_registered_message.hpp
    bonefish/messages/wamp_register_message.hpp
    bonefish/messages/wamp_subscribed_message.hpp
    bonefish/messages/wamp_subscribe_message.hpp
    bonefish/messages/wamp_unregistered_message.hpp
    bonefish/messages/wamp_unregister_message.hpp
    bonefish/messages/wamp_unsubscribed_message.hpp
    bonefish/messages/wamp_unsubscribe_message.hpp
    bonefish/messages/wamp_welcome_message.hpp
    bonefish/messages/wamp_yield_message.hpp
    bonefish/roles/wamp_role.hpp
    bonefish/roles/wamp_role_features.hpp
    bonefish/roles/wamp_role_type.hpp
    bonefish/serialization/msgpack_serializer.hpp
    bonefish/serialization/msgpack_goodbye_message_serializer.hpp
    bonefish/serialization/msgpack_hello_message_serializer.hpp
    bonefish/serialization/msgpack_welcome_message_serializer.hpp
    bonefish/serialization/msgpack_subscribe_message_serializer.hpp
    bonefish/serialization/msgpack_subscribed_message_serializer.hpp
    bonefish/serialization/msgpack_unsubscribe_message_serializer.hpp
    bonefish/serialization/msgpack_unsubscribed_message_serializer.hpp
    bonefish/serialization/wamp_serializer_type.hpp
    bonefish/serialization/wamp_serializer.hpp
    bonefish/wamp_dealer.hpp
    bonefish/wamp_router.hpp
    bonefish/wamp_routers.hpp
    bonefish/wamp_session.hpp
    bonefish/wamp_session_state.hpp
    bonefish/wamp_transport.hpp
    bonefish/wamp_uri.hpp
    bonefish/websocket_config.hpp
    bonefish/websocket_data.hpp
    bonefish/websocket_protocol.hpp
    bonefish/websocket_server.hpp
    bonefish/websocket_transport.hpp)

add_library(bonefish STATIC ${SOURCES} ${PUBLIC_HEADERS})
install(FILES ${PUBLIC_HEADERS} DESTINATION include/bonefish)
install(TARGETS bonefish DESTINATION lib)
