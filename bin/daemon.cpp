#include "daemon.hpp"
#include <bonefish/identifiers/wamp_session_id_generator.hpp>
#include <bonefish/serialization/wamp_serializers.hpp>
#include <bonefish/serialization/msgpack_serializer.hpp>
#include <bonefish/router/wamp_router.hpp>
#include <bonefish/router/wamp_routers.hpp>
#include <bonefish/tcp/tcp_server.hpp>
#include <bonefish/websocket/websocket_server.hpp>

#include <signal.h>
#include <string.h>
#include <boost/bind.hpp>

namespace bonefish
{

daemon::daemon()
    : m_io_service()
    , m_work()
    , m_termination_signals(m_io_service, SIGTERM, SIGINT, SIGQUIT)
    , m_routers(new wamp_routers)
    , m_serializers(new wamp_serializers)
    , m_session_id_generator()
    , m_tcp_server()
    , m_websocket_server()
{
    // TODO: This should all come from configuration that is passed in to the daemon.
    //       For now we just hard code the necessary bits and pieces to give us
    //       somthing functinal to work with.
    std::shared_ptr<wamp_router> router =
            std::make_shared<wamp_router>(m_io_service, "default");
    m_routers->add_router(router);
    m_serializers->add_serializer(std::make_shared<msgpack_serializer>());
    m_session_id_generator = std::make_shared<wamp_session_id_generator>();
    m_tcp_server = std::make_shared<tcp_server>(
            m_io_service, m_routers, m_serializers, m_session_id_generator);
    m_websocket_server = std::make_shared<websocket_server>(
            m_io_service, m_routers, m_serializers, m_session_id_generator);
}

daemon::~daemon()
{
    m_termination_signals.cancel();
}

void daemon::run()
{
    m_work.reset(new boost::asio::io_service::work(m_io_service));
    m_termination_signals.async_wait(
            boost::bind(&daemon::termination_signal_handler, this, _1, _2));

    m_websocket_server->start();
    m_tcp_server->start();

    m_io_service.run();
}

void daemon::shutdown()
{
    if (m_work.get())
    {
        m_io_service.dispatch(boost::bind(&daemon::shutdown_handler, this));
    }
}

boost::asio::io_service& daemon::io_service()
{
    return m_io_service;
}

void daemon::shutdown_handler()
{
    if (m_work.get())
    {
        m_termination_signals.cancel();
        m_websocket_server->shutdown();
        m_tcp_server->shutdown();
        m_work.reset();
    }
}

void daemon::termination_signal_handler(
        const boost::system::error_code& error_code, int signal_number)
{
    if (error_code == boost::asio::error::operation_aborted)
    {
        return;
    }

    if (signal_number == SIGINT || signal_number == SIGTERM || signal_number == SIGQUIT)
    {
        shutdown();
    }
    else
    {
        // We should never encounter a case where we are invoked for a
        // signal for which we are not intentionally checking for. We
        // keep the assert for debugging purposes and re-register the
        // handler for production purposes so that we are still able to
        // terminate upon receipt of one of the registered signals.
        assert(0);
        m_termination_signals.async_wait(
                boost::bind(&daemon::termination_signal_handler, this, _1, _2));
    }
}

} // namespace bonefish
