find_package(Boost REQUIRED COMPONENTS system thread)

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/third-party/websocketpp
    ${CMAKE_SOURCE_DIR}/third-party/msgpack-c/include
    ${Boost_INCLUDE_DIRS})

set(SOURCES
    bonefish/broker/wamp_broker.cpp
    bonefish/common/wamp_message_processor.cpp
    bonefish/dealer/wamp_dealer.cpp
    bonefish/messages/wamp_message_defaults.cpp
    bonefish/messages/wamp_message_factory.cpp
    bonefish/messages/wamp_message_type.cpp
    bonefish/messages/wamp_welcome_details.cpp
    bonefish/roles/wamp_role_type.cpp
    bonefish/router/wamp_router.cpp
    bonefish/serialization/msgpack_serializer.cpp
    bonefish/session/wamp_session_state.cpp
    bonefish/tcp/tcp_connection.cpp
    bonefish/tcp/tcp_listener.cpp
    bonefish/tcp/tcp_server.cpp
    bonefish/tcp/tcp_transport.cpp
    bonefish/utility/wamp_uri.cpp
    bonefish/websocket/websocket_protocol.cpp
    bonefish/websocket/websocket_server.cpp
    bonefish/websocket/websocket_transport.cpp)

if (asio_source)
    list(APPEND SOURCES bonefish/boost/asio.cpp)
endif ()

set(HEADERS
    bonefish/broker/wamp_broker.hpp
    bonefish/broker/wamp_broker_subscription.hpp
    bonefish/broker/wamp_broker_topic.hpp
    bonefish/common/wamp_connection_base.hpp
    bonefish/common/wamp_message_processor.hpp
    bonefish/dealer/wamp_dealer.hpp
    bonefish/dealer/wamp_dealer_registration.hpp
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
    bonefish/messages/wamp_event_message.hpp
    bonefish/messages/wamp_goodbye_message.hpp
    bonefish/messages/wamp_hello_details.hpp
    bonefish/messages/wamp_hello_message.hpp
    bonefish/messages/wamp_invocation_message.hpp
    bonefish/messages/wamp_message.hpp
    bonefish/messages/wamp_message_defaults.hpp
    bonefish/messages/wamp_message_factory.hpp
    bonefish/messages/wamp_message_type.hpp
    bonefish/messages/wamp_publish_message.hpp
    bonefish/messages/wamp_published_message.hpp
    bonefish/messages/wamp_registered_message.hpp
    bonefish/messages/wamp_register_message.hpp
    bonefish/messages/wamp_subscribed_message.hpp
    bonefish/messages/wamp_subscribe_message.hpp
    bonefish/messages/wamp_unregistered_message.hpp
    bonefish/messages/wamp_unregister_message.hpp
    bonefish/messages/wamp_unsubscribed_message.hpp
    bonefish/messages/wamp_unsubscribe_message.hpp
    bonefish/messages/wamp_welcome_details.hpp
    bonefish/messages/wamp_welcome_message.hpp
    bonefish/messages/wamp_yield_message.hpp
    bonefish/roles/wamp_role.hpp
    bonefish/roles/wamp_role_features.hpp
    bonefish/roles/wamp_role_type.hpp
    bonefish/router/wamp_router.hpp
    bonefish/router/wamp_routers.hpp
    bonefish/serialization/msgpack_serializer.hpp
    bonefish/serialization/wamp_serializer_type.hpp
    bonefish/serialization/wamp_serializer.hpp
    bonefish/session/wamp_session.hpp
    bonefish/session/wamp_session_state.hpp
    bonefish/tcp/tcp_connection.hpp
    bonefish/tcp/tcp_listener.hpp
    bonefish/tcp/tcp_server.hpp
    bonefish/tcp/tcp_transport.hpp
    bonefish/transport/wamp_transport.hpp
    bonefish/utility/wamp_uri.hpp
    bonefish/websocket/websocket_config.hpp
    bonefish/websocket/websocket_protocol.hpp
    bonefish/websocket/websocket_server.hpp
    bonefish/websocket/websocket_transport.hpp)

add_library(bonefish STATIC ${SOURCES} ${HEADERS})
install(DIRECTORY bonefish/ DESTINATION include/bonefish FILES_MATCHING PATTERN *.hpp)
install(TARGETS bonefish DESTINATION lib)
