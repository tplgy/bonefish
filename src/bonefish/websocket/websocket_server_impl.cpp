#include <bonefish/websocket/websocket_server_impl.hpp>
#include <bonefish/identifiers/wamp_session_id.hpp>
#include <bonefish/identifiers/wamp_session_id_generator.hpp>
#include <bonefish/router/wamp_router.hpp>
#include <bonefish/router/wamp_routers.hpp>
#include <bonefish/serialization/wamp_serializer.hpp>
#include <bonefish/serialization/wamp_serializers.hpp>
#include <bonefish/transport/wamp_transport.hpp>
#include <bonefish/websocket/websocket_protocol.hpp>
#include <bonefish/websocket/websocket_transport.hpp>

#include <boost/asio/io_service.hpp>
#include <websocketpp/common/connection_hdl.hpp>
#include <websocketpp/utilities.hpp>

namespace bonefish {

websocket_server_impl::websocket_server_impl(
        boost::asio::io_service& io_service,
        const std::shared_ptr<wamp_routers>& routers,
        const std::shared_ptr<wamp_serializers>& serializers)
    : m_io_service(io_service)
    , m_server(new websocketpp::server<websocket_config>())
    , m_routers(routers)
    , m_serializers(serializers)
    , m_message_processor(m_routers)
{
}

websocket_server_impl::~websocket_server_impl()
{
}

void websocket_server_impl::start(const boost::asio::ip::address& ip_address, uint16_t port)
{
    boost::asio::ip::tcp::endpoint endpoint(ip_address, port);

    m_server->set_open_handler(
            websocketpp::lib::bind(&websocket_server_impl::on_open, this,
                    websocketpp::lib::placeholders::_1));

    m_server->set_close_handler(
            websocketpp::lib::bind(&websocket_server_impl::on_close, this,
                    websocketpp::lib::placeholders::_1));

    m_server->set_fail_handler(
            websocketpp::lib::bind(&websocket_server_impl::on_fail, this,
                    websocketpp::lib::placeholders::_1));

    m_server->set_validate_handler(
            websocketpp::lib::bind(&websocket_server_impl::on_validate, this,
                    websocketpp::lib::placeholders::_1));

    m_server->set_message_handler(
            websocketpp::lib::bind(&websocket_server_impl::on_message, this,
                    websocketpp::lib::placeholders::_1,
                    websocketpp::lib::placeholders::_2));

    // Set log settings
    m_server->set_access_channels(websocketpp::log::alevel::all);
    m_server->clear_access_channels(websocketpp::log::alevel::frame_payload);

    m_server->init_asio(&m_io_service);
    m_server->set_reuse_addr(true);
    m_server->listen(endpoint);
    m_server->start_accept();

    // The io_service is run from owning context.
    //m_server->run();
}

void websocket_server_impl::shutdown()
{
    m_server->stop();
}

void websocket_server_impl::on_open(websocketpp::connection_hdl handle)
{
    std::cerr << "open handler called: " << handle.lock().get() << std::endl;
}

void websocket_server_impl::on_close(websocketpp::connection_hdl handle)
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

void websocket_server_impl::on_fail(websocketpp::connection_hdl handle)
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

bool websocket_server_impl::on_validate(websocketpp::connection_hdl handle)
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
        } else if (subprotocol == WAMPV2_JSON_SUBPROTOCOL) {
            if (m_serializers->has_serializer(wamp_serializer_type::JSON)) {
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

void websocket_server_impl::on_message(websocketpp::connection_hdl handle,
        websocketpp::server<websocket_config>::message_ptr buffer)
{
    std::cerr << "received message" << std::endl;
    websocketpp::server<websocket_config>::connection_ptr connection =
            m_server->get_con_from_hdl(handle);
    std::shared_ptr<wamp_serializer> serializer;

    if (connection->get_subprotocol() == WAMPV2_MSGPACK_SUBPROTOCOL) {
        serializer = m_serializers->get_serializer(wamp_serializer_type::MSGPACK);
    } else if (connection->get_subprotocol() == WAMPV2_JSON_SUBPROTOCOL) {
        serializer = m_serializers->get_serializer(wamp_serializer_type::JSON);
    }

    try {
        std::unique_ptr<wamp_message> message(
                serializer->deserialize(buffer->get_payload().c_str(), buffer->get_payload().size()));
        std::unique_ptr<wamp_transport> transport(
                new websocket_transport(serializer, handle, m_server));

        if (message) {
            m_message_processor.process_message(message, std::move(transport), connection.get());
        } else {
            std::cerr << "error: unable to deserialize message" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "unhandled exception: " << e.what() << std::endl;
    }
}

} // namespace bonefish
