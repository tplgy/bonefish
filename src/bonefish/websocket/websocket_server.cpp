#include <bonefish/websocket/websocket_server.hpp>
#include <bonefish/identifiers/wamp_session_id.hpp>
#include <bonefish/messages/wamp_abort_message.hpp>
#include <bonefish/messages/wamp_call_message.hpp>
#include <bonefish/messages/wamp_error_message.hpp>
#include <bonefish/messages/wamp_goodbye_message.hpp>
#include <bonefish/messages/wamp_hello_message.hpp>
#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/wamp_publish_message.hpp>
#include <bonefish/messages/wamp_register_message.hpp>
#include <bonefish/messages/wamp_subscribe_message.hpp>
#include <bonefish/messages/wamp_unregister_message.hpp>
#include <bonefish/messages/wamp_unsubscribe_message.hpp>
#include <bonefish/messages/wamp_yield_message.hpp>
#include <bonefish/router/wamp_router.hpp>
#include <bonefish/router/wamp_routers.hpp>
#include <bonefish/serialization/msgpack_serializer.hpp>
#include <bonefish/serialization/wamp_serializer.hpp>
#include <bonefish/serialization/wamp_serializers.hpp>
#include <bonefish/session/wamp_session.hpp>
#include <bonefish/transport/wamp_transport.hpp>
#include <bonefish/websocket/websocket_transport.hpp>
#include <bonefish/websocket/websocket_protocol.hpp>

#include <boost/asio/io_service.hpp>
#include <msgpack.hpp>
#include <websocketpp/common/connection_hdl.hpp>
#include <websocketpp/utilities.hpp>

namespace bonefish {

websocket_server::websocket_server(
        boost::asio::io_service& io_service,
        const std::shared_ptr<wamp_routers>& routers,
        const std::shared_ptr<wamp_serializers>& serializers)
    : m_io_service(io_service)
    , m_server(new websocketpp::server<websocket_config>())
    , m_routers(routers)
    , m_serializers(serializers)
    , m_session_id_generator()
{
}

websocket_server::~websocket_server()
{
}

void websocket_server::start()
{
    m_server->set_open_handler(
            websocketpp::lib::bind(&websocket_server::on_open, this,
                    websocketpp::lib::placeholders::_1));

    m_server->set_close_handler(
            websocketpp::lib::bind(&websocket_server::on_close, this,
                    websocketpp::lib::placeholders::_1));

    m_server->set_fail_handler(
            websocketpp::lib::bind(&websocket_server::on_fail, this,
                    websocketpp::lib::placeholders::_1));

    m_server->set_ping_handler(
            websocketpp::lib::bind(&websocket_server::on_ping, this,
                    websocketpp::lib::placeholders::_1,
                    websocketpp::lib::placeholders::_2));

    m_server->set_pong_handler(
            websocketpp::lib::bind(&websocket_server::on_pong, this,
                    websocketpp::lib::placeholders::_1,
                    websocketpp::lib::placeholders::_2));

    m_server->set_pong_timeout_handler(
            websocketpp::lib::bind(&websocket_server::on_pong_timeout, this,
                    websocketpp::lib::placeholders::_1,
                    websocketpp::lib::placeholders::_2));

    m_server->set_validate_handler(
            websocketpp::lib::bind(&websocket_server::on_validate, this,
                    websocketpp::lib::placeholders::_1));

    m_server->set_message_handler(
            websocketpp::lib::bind(&websocket_server::on_message, this,
                    websocketpp::lib::placeholders::_1,
                    websocketpp::lib::placeholders::_2));

    // Set log settings
    m_server->set_access_channels(websocketpp::log::alevel::all);
    m_server->clear_access_channels(websocketpp::log::alevel::frame_payload);

    m_server->init_asio(&m_io_service);
    m_server->set_reuse_addr(true);
    m_server->listen(9005);
    m_server->start_accept();
    // The io_service is run from owning context.
    //m_server->run();
}

void websocket_server::shutdown()
{
}

void websocket_server::on_open(websocketpp::connection_hdl handle)
{
    std::cerr << "open handler called: " << handle.lock().get() << std::endl;
}

void websocket_server::on_close(websocketpp::connection_hdl handle)
{
    std::cerr << "close handler called: " << handle.lock().get() << std::endl;
    websocketpp::server<websocket_config>::connection_ptr connection =
            m_server->get_con_from_hdl(handle);

    if (connection->has_session_id()) {
        std::shared_ptr<wamp_router> router =
                m_routers->get_router(connection->get_realm());
        if (router) {
            router->detach_session(connection->get_session_id());
        }
    }
}

void websocket_server::on_fail(websocketpp::connection_hdl handle)
{
    std::cerr << "fail handler called: " << handle.lock().get() << std::endl;
    websocketpp::server<websocket_config>::connection_ptr connection =
            m_server->get_con_from_hdl(handle);

    if (connection->has_session_id()) {
        std::shared_ptr<wamp_router> router =
                m_routers->get_router(connection->get_realm());
        if (router) {
            router->detach_session(connection->get_session_id());
        }
    }
}

bool websocket_server::on_ping(websocketpp::connection_hdl handle, std::string message)
{
    std::cerr << "ping handler called: " << handle.lock().get() << std::endl;
    return true;
}

void websocket_server::on_pong(websocketpp::connection_hdl handle, std::string message)
{
    std::cerr << "pong handler called: " << handle.lock().get() << std::endl;
}

void websocket_server::on_pong_timeout(websocketpp::connection_hdl handle, std::string message)
{
    std::cerr << "pong timeout handler called: " << handle.lock().get() << std::endl;
}

bool websocket_server::on_validate(websocketpp::connection_hdl handle)
{
    websocketpp::server<websocket_config>::connection_ptr connection =
            m_server->get_con_from_hdl(handle);

    std::cerr << "Cache-Control: " << connection->get_request_header("Cache-Control") << std::endl;

    const auto& subprotocols = connection->get_requested_subprotocols();
    for (const auto& subprotocol : subprotocols) {
        if (subprotocol == WAMPV2_MSGPACK_SUBPROTOCOL) {
            if (m_serializers->has_serializer(wamp_serializer_type::MSGPACK)) {
                std::cerr << "negotiated subprotocol " << subprotocol << std::endl;
                connection->select_subprotocol(subprotocol);
                return true;
            }
        } else {
            std::cerr << "requested subprotocol " << subprotocol << " is unsupported" << std::endl;
        }
    }

    std::cerr << "no supported subprotocol found" << std::endl;
    return false;
}

void websocket_server::on_message(websocketpp::connection_hdl handle,
        websocketpp::server<websocket_config>::message_ptr buffer)
{
    std::cerr << "received message" << std::endl;
    websocketpp::server<websocket_config>::connection_ptr connection =
            m_server->get_con_from_hdl(handle);

    if (connection->get_subprotocol() == WAMPV2_MSGPACK_SUBPROTOCOL) {
        try {
            std::shared_ptr<wamp_serializer> serializer =
                    m_serializers->get_serializer(wamp_serializer_type::MSGPACK);
            std::unique_ptr<wamp_message> message(
                    serializer->deserialize(buffer->get_payload().c_str(), buffer->get_payload().size()));
            std::unique_ptr<wamp_transport> transport(
                    new websocket_transport(serializer, handle, m_server));

            if (message) {
                std::cerr << "received message: " << message_type_to_string(message->get_type()) << std::endl;
                switch (message->get_type())
                {
                    case wamp_message_type::AUTHENTICATE:
                        break;
                    case wamp_message_type::CALL:
                        {
                            std::shared_ptr<wamp_router> router = m_routers->get_router(connection->get_realm());
                            if (router) {
                                wamp_call_message* call_message = static_cast<wamp_call_message*>(message.get());
                                router->process_call_message(connection->get_session_id(), call_message);
                            }
                        }
                        break;
                    case wamp_message_type::CANCEL:
                        break;
                    case wamp_message_type::ERROR:
                        {
                            std::shared_ptr<wamp_router> router = m_routers->get_router(connection->get_realm());
                            if (router) {
                                wamp_error_message* error_message = static_cast<wamp_error_message*>(message.get());
                                router->process_error_message(connection->get_session_id(), error_message);
                            }
                        }
                        break;
                    case wamp_message_type::GOODBYE:
                        {
                            std::shared_ptr<wamp_router> router = m_routers->get_router(connection->get_realm());
                            if (router) {
                                wamp_goodbye_message* goodbye_message = static_cast<wamp_goodbye_message*>(message.get());
                                router->process_goodbye_message(connection->get_session_id(), goodbye_message);
                                router->detach_session(connection->get_session_id());
                            }
                            connection->clear_data();
                        }
                        break;
                    case wamp_message_type::HELLO:
                        {
                            wamp_hello_message* hello_message = static_cast<wamp_hello_message*>(message.get());
                            std::shared_ptr<wamp_router> router = m_routers->get_router(hello_message->get_realm());
                            if (!router) {
                                std::unique_ptr<wamp_abort_message> abort_message(new wamp_abort_message);
                                abort_message->set_reason("wamp.error.no_such_realm");
                                transport->send_message(abort_message.get());
                                break;
                            }

                            wamp_session_id id;
                            do {
                                id = m_session_id_generator.generate();
                            } while(router->has_session(id));

                            connection->set_session_id(id);
                            connection->set_realm(hello_message->get_realm());

                            router->attach_session(std::make_shared<wamp_session>(id, std::move(transport)));
                            router->process_hello_message(id, hello_message);
                        }
                        break;
                    case wamp_message_type::PUBLISH:
                        {
                            std::shared_ptr<wamp_router> router = m_routers->get_router(connection->get_realm());
                            if (router) {
                                wamp_publish_message* publish_message = static_cast<wamp_publish_message*>(message.get());
                                router->process_publish_message(connection->get_session_id(), publish_message);
                            }
                        }
                        break;
                    case wamp_message_type::REGISTER:
                        {
                            std::shared_ptr<wamp_router> router = m_routers->get_router(connection->get_realm());
                            if (router) {
                                wamp_register_message* register_message = static_cast<wamp_register_message*>(message.get());
                                router->process_register_message(connection->get_session_id(), register_message);
                            }
                        }
                        break;
                    case wamp_message_type::SUBSCRIBE:
                        {
                            std::shared_ptr<wamp_router> router = m_routers->get_router(connection->get_realm());
                            if (router) {
                                wamp_subscribe_message* subscribe_message = static_cast<wamp_subscribe_message*>(message.get());
                                router->process_subscribe_message(connection->get_session_id(), subscribe_message);
                            }
                        }
                        break;
                    case wamp_message_type::UNREGISTER:
                        {
                            std::shared_ptr<wamp_router> router = m_routers->get_router(connection->get_realm());
                            if (router) {
                                wamp_unregister_message* unregister_message = static_cast<wamp_unregister_message*>(message.get());
                                router->process_unregister_message(connection->get_session_id(), unregister_message);
                            }
                        }
                        break;
                    case wamp_message_type::UNSUBSCRIBE:
                        {
                            std::shared_ptr<wamp_router> router = m_routers->get_router(connection->get_realm());
                            if (router) {
                                wamp_unsubscribe_message* unsubscribe_message = static_cast<wamp_unsubscribe_message*>(message.get());
                                router->process_unsubscribe_message(connection->get_session_id(), unsubscribe_message);
                            }
                        }
                        break;
                    case wamp_message_type::YIELD:
                        {
                            std::shared_ptr<wamp_router> router = m_routers->get_router(connection->get_realm());
                            if (router) {
                                wamp_yield_message* yield_message = static_cast<wamp_yield_message*>(message.get());
                                router->process_yield_message(connection->get_session_id(), yield_message);
                            }
                        }
                        break;
                    default:
                        break;
                }
            } else {
                std::cerr << "error: unable to deserialize message" << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "unhandled exception: " << e.what() << std::endl;
        }
    }
}

} // namespace bonefish
