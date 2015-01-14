#include <bonefish/websocket_server.hpp>
#include <bonefish/identifier/session_id.hpp>
#include <bonefish/message_code.hpp>
#include <bonefish/router.hpp>
#include <bonefish/session.hpp>
#include <bonefish/wamp_message.hpp>
#include <bonefish/websocket_protocol.hpp>
#include <msgpack.hpp>
#include <websocketpp/common/connection_hdl.hpp>
#include <websocketpp/utilities.hpp>

namespace bonefish {

websocket_server::websocket_server(const std::shared_ptr<router>& r)
    : m_server(new websocketpp::server<websocket_config>())
    , m_router(r)
    , m_session_id_generator()
{
}

websocket_server::~websocket_server()
{
}

void websocket_server::run()
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

    m_server->init_asio();
    m_server->listen(9005);
    m_server->start_accept();
    m_server->run();
}

void websocket_server::shutdown()
{
}

void websocket_server::on_open(websocketpp::connection_hdl handle)
{
    std::cerr << "open handler called: " << handle.lock().get() << std::endl;
    session_id id;
    do {
        id = m_session_id_generator.generate();
    } while(m_router->has_session(id));

    websocketpp::server<websocket_config>::connection_ptr connection =
            m_server->get_con_from_hdl(handle);
    connection->set_session_id(id);

    m_router->attach_session(std::make_shared<session>(id, handle, m_server));
}

void websocket_server::on_close(websocketpp::connection_hdl handle)
{
    std::cerr << "close handler called: " << handle.lock().get() << std::endl;
    websocketpp::server<websocket_config>::connection_ptr connection =
            m_server->get_con_from_hdl(handle);
    m_router->detach_session(connection->get_session_id());
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
        wamp_message wmessage;

        try {
            msgpack::unpacked item;
            msgpack::unpacker unpacker;
            unpacker.reserve_buffer(message->get_payload().size());
            memcpy(unpacker.buffer(), message->get_payload().c_str(), message->get_payload().size());
            unpacker.buffer_consumed(message->get_payload().size());

            try {
                unpacker.next(&item);
                item.get().convert(&wmessage);

                message_code code = static_cast<message_code>(wmessage[0].as<int>());
                std::cerr << "received message: " << to_string(code) << std::endl;
                switch (code)
                {
                    case message_code::Hello:
                        break;
                    case message_code::Authenticate:
                        break;
                    case message_code::Goodbye:
                        break;
                    case message_code::Error:
                        break;
                    case message_code::Publish:
                        break;
                    case message_code::Subscribe:
                        break;
                    case message_code::Unsubscribe:
                        break;
                    case message_code::Call:
                        break;
                    case message_code::Cancel:
                        break;
                    case message_code::Register:
                        break;
                    case message_code::Unregister:
                        break;
                    case message_code::Yield:
                        break;
                }
            } catch (const msgpack::unpack_error& e) {
                std::cerr << "failed to unpack message: " << e.what();
            } catch (const msgpack::type_error& e) {
                std::cerr << "failed to parse message: " << e.what();
            } catch (const std::exception& e) {
                std::cerr << "unhandled exception: " << e.what() << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "unhandled exception: " << e.what() << std::endl;
        }
    }
}

} // namespace bonefish
