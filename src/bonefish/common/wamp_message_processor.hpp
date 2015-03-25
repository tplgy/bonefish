#ifndef BONEFISH_WAMP_MESSAGE_PROCESSOR_HPP
#define BONEFISH_WAMP_MESSAGE_PROCESSOR_HPP

#include <bonefish/identifiers/wamp_session_id.hpp>

#include <memory>

namespace bonefish {

class wamp_connection_base;
class wamp_message;
class wamp_routers;
class wamp_session_id_generator;
class wamp_transport;

class wamp_message_processor
{
public:
    wamp_message_processor(
            const std::shared_ptr<wamp_routers>& routers,
            const std::shared_ptr<wamp_session_id_generator>& generator);

    void process_message(
            const std::unique_ptr<wamp_message>& message,
            std::unique_ptr<wamp_transport>&& transport,
            wamp_connection_base* connection_base);

private:
    std::shared_ptr<wamp_routers> m_routers;
    std::shared_ptr<wamp_session_id_generator> m_session_id_generator;
};

inline wamp_message_processor::wamp_message_processor(
        const std::shared_ptr<wamp_routers>& routers,
        const std::shared_ptr<wamp_session_id_generator>& generator)
    : m_routers(routers)
    , m_session_id_generator(generator)
{
}

} // namespace bonefish

#endif // BONEFISH_WAMP_MESSAGE_PROCESSOR_HPP
