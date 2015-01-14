#ifndef BONEFISH_IDENTIFIER_SESSION_ID_GENERATOR_HPP
#define BONEFISH_IDENTIFIER_SESSION_ID_GENERATOR_HPP

#include <bonefish/identifier/session_id.hpp>
#include <bonefish/identifier/random_id_generator.hpp>
#include <cstdint>

namespace bonefish {

class session_id_generator
{
public:
    session_id_generator();
    explicit session_id_generator(uint64_t seed);
    session_id_generator(session_id_generator const&) = delete;
    session_id_generator& operator=(session_id_generator const&) = delete;
    session_id generate();

private:
    random_id_generator m_random_id_generator;
};

inline session_id_generator::session_id_generator()
    : m_random_id_generator(session_id::MIN, session_id::MAX)
{
}

inline session_id_generator::session_id_generator(uint64_t seed)
    : m_random_id_generator(session_id::MIN, session_id::MAX, seed)
{
}

inline session_id session_id_generator::generate()
{
    return session_id(m_random_id_generator.generate());
}

} // namespace bonefish

#endif // BONEFISH_IDENTIFIER_SESSION_ID_GENERATOR_HPP
