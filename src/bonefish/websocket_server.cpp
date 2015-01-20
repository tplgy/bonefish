#include <bonefish/websocket_server.hpp>
#include <bonefish/identifier/session_id.hpp>
#include <bonefish/messages/hello_message.hpp>
#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/welcome_message.hpp>
#include <bonefish/realm_routers.hpp>
#include <bonefish/serialization/serializer.hpp>
#include <bonefish/serialization/msgpack_serializer.hpp>
#include <bonefish/session.hpp>
#include <bonefish/session_transport.hpp>
#include <bonefish/websocket_protocol.hpp>
#include <boost/asio/io_service.hpp>
#include <msgpack.hpp>
#include <websocketpp/common/connection_hdl.hpp>
#include <websocketpp/utilities.hpp>

namespace bonefish {

websocket_server::websocket_server(
        boost::asio::io_service& io_service,
        const std::shared_ptr<realm_routers>& routers)
    : m_io_service(io_service)
    , m_server(new websocketpp::server<websocket_config>())
    , m_realm_routers(routers)
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
        std::shared_ptr<router> realm_router =
                m_realm_routers->get_router(connection->get_realm());
        if (realm_router) {
            realm_router->detach_session(connection->get_session_id());
        }
    }
}

void websocket_server::on_fail(websocketpp::connection_hdl handle)
{
    std::cerr << "fail handler called: " << handle.lock().get() << std::endl;
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
            std::cerr << "negotiated subprotocol " << subprotocol << std::endl;
            connection->select_subprotocol(subprotocol);
            return true;
        } else {
            std::cerr << "requested subprotocol " << subprotocol << " is unsupported" << std::endl;
        }
    }

    return false;
}

void websocket_server::on_message(websocketpp::connection_hdl handle,
        websocketpp::server<websocket_config>::message_ptr message)
{
    websocketpp::server<websocket_config>::connection_ptr connection =
            m_server->get_con_from_hdl(handle);

    if (connection->get_subprotocol() == WAMPV2_MSGPACK_SUBPROTOCOL) {
        try {
            msgpack_serializer mps;
            std::unique_ptr<wamp_message> wmsg = mps.deserialize(
                    message->get_payload().c_str(), message->get_payload().size());

            if (wmsg) {
                std::cerr << "received message: " << to_string(wmsg->get_type()) << std::endl;
                switch (wmsg->get_type())
                {
                    case message_type::Hello:
                        {
                            hello_message* hello = static_cast<hello_message*>(wmsg.get());
                            std::shared_ptr<router> realm_router =
                                    m_realm_routers->get_router(hello->get_realm());
                            if (!realm_router) {
                                std::cerr << "error: no router found in realm " << hello->get_realm() << std::endl;
                                break;
                            }

                            session_id id;
                            do {
                                id = m_session_id_generator.generate();
                            } while(realm_router->has_session(id));

                            connection->set_session_id(id);
                            connection->set_realm(hello->get_realm());

                            std::unique_ptr<serializer> mps(new msgpack_serializer);
                            std::unique_ptr<session_transport> transport(
                                    new session_transport(std::move(mps), handle, m_server));
                            realm_router->attach_session(std::make_shared<session>(id, std::move(transport)));
                        }
                        break;
                    case message_type::Authenticate:
                        break;
                    case message_type::Goodbye:
                        break;
                    case message_type::Error:
                        break;
                    case message_type::Publish:
                        break;
                    case message_type::Subscribe:
                        break;
                    case message_type::Unsubscribe:
                        break;
                    case message_type::Call:
                        break;
                    case message_type::Cancel:
                        break;
                    case message_type::Register:
                        break;
                    case message_type::Unregister:
                        break;
                    case message_type::Yield:
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
