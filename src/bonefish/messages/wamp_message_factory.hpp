#ifndef BONEFISH_MESSAGES_WAMP_MESSAGE_FACTORY_HPP
#define BONEFISH_MESSAGES_WAMP_MESSAGE_FACTORY_HPP

#include <bonefish/messages/wamp_message_type.hpp>

namespace bonefish {

class wamp_message;

namespace wamp_message_factory
{

wamp_message* create_message(wamp_message_type type);

};

} // namespace bonefish

#endif // BONEFISH_MESSAGES_WAMP_MESSAGE_FACTORY_HPP
