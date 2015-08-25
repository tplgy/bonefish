set(SOURCES
    bonefish/broker/wamp_broker.cpp
    bonefish/common/wamp_message_processor.cpp
    bonefish/dealer/wamp_dealer.cpp
    bonefish/identifiers/wamp_session_id_factory.cpp
    bonefish/messages/wamp_call_options.cpp
    bonefish/messages/wamp_hello_details.cpp
    bonefish/messages/wamp_message_defaults.cpp
    bonefish/messages/wamp_message_factory.cpp
    bonefish/messages/wamp_message_type.cpp
    bonefish/messages/wamp_welcome_details.cpp
    bonefish/rawsocket/tcp_connection.cpp
    bonefish/rawsocket/tcp_listener.cpp
    bonefish/rawsocket/rawsocket_server.cpp
    bonefish/rawsocket/rawsocket_server_impl.cpp
    bonefish/rawsocket/rawsocket_transport.cpp
    bonefish/roles/wamp_role_type.cpp
    bonefish/router/wamp_router.cpp
    bonefish/router/wamp_router_impl.cpp
    bonefish/serialization/json_serializer.cpp
    bonefish/serialization/msgpack_serializer.cpp
    bonefish/session/wamp_session_state.cpp
    bonefish/trace/trace.cpp
    bonefish/utility/wamp_uri.cpp
    bonefish/websocket/websocket_protocol.cpp
    bonefish/websocket/websocket_server.cpp
    bonefish/websocket/websocket_server_impl.cpp
    bonefish/websocket/websocket_transport.cpp)

if (asio_source)
    list(APPEND SOURCES bonefish/boost/asio.cpp)
endif ()

set(PUBLIC_HEADERS
    bonefish/rawsocket/rawsocket_listener.hpp
    bonefish/rawsocket/rawsocket_server.hpp
    bonefish/rawsocket/tcp_listener.hpp
    bonefish/router/wamp_router.hpp
    bonefish/router/wamp_routers.hpp
    bonefish/serialization/expandable_buffer.hpp
    bonefish/serialization/json_serializer.hpp
    bonefish/serialization/msgpack_serializer.hpp
    bonefish/serialization/wamp_serializer_type.hpp
    bonefish/serialization/wamp_serializer.hpp
    bonefish/serialization/wamp_serializers.hpp
    bonefish/trace/trace.hpp
    bonefish/websocket/websocket_server.hpp)

set(PRIVATE_HEADERS
    bonefish/broker/wamp_broker.hpp
    bonefish/broker/wamp_broker_subscription.hpp
    bonefish/broker/wamp_broker_topic.hpp
    bonefish/common/wamp_connection_base.hpp
    bonefish/common/wamp_message_processor.hpp
    bonefish/dealer/wamp_dealer.hpp
    bonefish/dealer/wamp_dealer_invocation.hpp
    bonefish/dealer/wamp_dealer_registration.hpp
    bonefish/identifiers/wamp_publication_id.hpp
    bonefish/identifiers/wamp_publication_id_generator.hpp
    bonefish/identifiers/wamp_random_id_generator.hpp
    bonefish/identifiers/wamp_random_id.hpp
    bonefish/identifiers/wamp_registration_id.hpp
    bonefish/identifiers/wamp_registration_id_generator.hpp
    bonefish/identifiers/wamp_request_id.hpp
    bonefish/identifiers/wamp_request_id_generator.hpp
    bonefish/identifiers/wamp_sequential_id.hpp
    bonefish/identifiers/wamp_sequential_id_generator.hpp
    bonefish/identifiers/wamp_session_id.hpp
    bonefish/identifiers/wamp_session_id_factory.hpp
    bonefish/identifiers/wamp_session_id_generator.hpp
    bonefish/identifiers/wamp_subscription_id.hpp
    bonefish/identifiers/wamp_subscription_id_generator.hpp
    bonefish/logging/logging.hpp
    bonefish/messages/wamp_abort_message.hpp
    bonefish/messages/wamp_call_message.hpp
    bonefish/messages/wamp_call_options.hpp
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
    bonefish/messages/wamp_result_message.hpp
    bonefish/messages/wamp_subscribed_message.hpp
    bonefish/messages/wamp_subscribe_message.hpp
    bonefish/messages/wamp_unregistered_message.hpp
    bonefish/messages/wamp_unregister_message.hpp
    bonefish/messages/wamp_unsubscribed_message.hpp
    bonefish/messages/wamp_unsubscribe_message.hpp
    bonefish/messages/wamp_welcome_details.hpp
    bonefish/messages/wamp_welcome_message.hpp
    bonefish/messages/wamp_yield_message.hpp
    bonefish/rawsocket/rawsocket_connection.hpp
    bonefish/rawsocket/rawsocket_server_impl.hpp
    bonefish/rawsocket/rawsocket_transport.hpp
    bonefish/rawsocket/tcp_connection.hpp
    bonefish/rawsocket/tcp_listener.hpp
    bonefish/roles/wamp_role.hpp
    bonefish/roles/wamp_role_features.hpp
    bonefish/roles/wamp_role_type.hpp
    bonefish/router/wamp_router_impl.hpp
    bonefish/serialization/base64.hpp
    bonefish/serialization/json_msgpack_sax.hpp
    bonefish/session/wamp_session.hpp
    bonefish/session/wamp_session_state.hpp
    bonefish/transport/wamp_transport.hpp
    bonefish/utility/wamp_uri.hpp
    bonefish/websocket/websocket_config.hpp
    bonefish/websocket/websocket_protocol.hpp
    bonefish/websocket/websocket_server_impl.hpp
    bonefish/websocket/websocket_transport.hpp)

add_library(bonefish STATIC ${SOURCES} ${PUBLIC_HEADERS} ${PRIVATE_HEADERS})

target_link_libraries(bonefish
    ${Boost_LIBRARIES})

foreach(h ${PUBLIC_HEADERS})
    get_filename_component(HEADER_INCLUDE_DIRECTORY include/${h} PATH) # use DIRECTORY instead of PATH once requiring CMake 3.0
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${h} DESTINATION ${HEADER_INCLUDE_DIRECTORY} COMPONENT "headers")
endforeach()

install(TARGETS bonefish DESTINATION lib)
