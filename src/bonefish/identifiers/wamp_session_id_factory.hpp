#ifndef BONEFISH_WAMP_SESSION_ID_FACTORY_HPP
#define BONEFISH_WAMP_SESSION_ID_FACTORY_HPP

#include <string>

namespace bonefish {
namespace wamp_session_id_factory {

const std::shared_ptr<wamp_session_id_generator>& create(const std::string& realm);

} // namespace wamp_session_id_factory
} // namespace bonefish

#endif // BONEFISH_WAMP_SESSION_ID_FACTORY_HPP
