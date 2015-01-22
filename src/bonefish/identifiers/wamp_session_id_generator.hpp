#ifndef BONEFISH_IDENTIFIERS_WAMP_SESSION_ID_GENERATOR_HPP
#define BONEFISH_IDENTIFIERS_WAMP_SESSION_ID_GENERATOR_HPP

#include <bonefish/identifiers/wamp_session_id.hpp>
#include <bonefish/identifiers/wamp_random_id_generator.hpp>
#include <cstdint>

namespace bonefish {

class wamp_session_id_generator
{
public:
    wamp_session_id_generator();
    explicit wamp_session_id_generator(uint64_t seed);
    wamp_session_id_generator(wamp_session_id_generator const&) = delete;
    wamp_session_id_generator& operator=(wamp_session_id_generator const&) = delete;
    wamp_session_id generate();

private:
    wamp_random_id_generator m_random_id_generator;
};

inline wamp_session_id_generator::wamp_session_id_generator()
    : m_random_id_generator(wamp_session_id::MIN, wamp_session_id::MAX)
{
}

inline wamp_session_id_generator::wamp_session_id_generator(uint64_t seed)
    : m_random_id_generator(wamp_session_id::MIN, wamp_session_id::MAX, seed)
{
}

inline wamp_session_id wamp_session_id_generator::generate()
{
    return wamp_session_id(m_random_id_generator.generate());
}

} // namespace bonefish

#endif // BONEFISH_IDENTIFIERS_WAMP_SESSION_ID_GENERATOR_HPP
