#ifndef BONEFISH_WAMP_SESSION_ID_FACTORY_HPP
#define BONEFISH_WAMP_SESSION_ID_FACTORY_HPP

#include <memory>
#include <string>

namespace bonefish {

class wamp_session_id_generator;

namespace wamp_session_id_factory {

std::shared_ptr<wamp_session_id_generator> create(const std::string& realm);

} // namespace wamp_session_id_factory
} // namespace bonefish

#endif // BONEFISH_WAMP_SESSION_ID_FACTORY_HPP
