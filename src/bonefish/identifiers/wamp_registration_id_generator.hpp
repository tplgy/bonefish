#ifndef BONEFISH_IDENTIFIERS_WAMP_REGISTRATION_ID_GENERATOR_HPP
#define BONEFISH_IDENTIFIERS_WAMP_REGISTRATION_ID_GENERATOR_HPP

#include <bonefish/identifiers/wamp_registration_id.hpp>
#include <bonefish/identifiers/wamp_sequential_id_generator.hpp>

namespace bonefish {

class wamp_registration_id_generator
{
public:
    wamp_registration_id_generator();
    wamp_registration_id_generator(wamp_registration_id_generator const&) = delete;
    wamp_registration_id_generator& operator=(wamp_registration_id_generator const&) = delete;
    wamp_registration_id generate();

private:
    wamp_sequential_id_generator m_sequential_id_generator;
};

inline wamp_registration_id_generator::wamp_registration_id_generator()
    : m_sequential_id_generator(wamp_registration_id::MIN, wamp_registration_id::MAX)
{
}

inline wamp_registration_id wamp_registration_id_generator::generate()
{
    return wamp_registration_id(m_sequential_id_generator.generate());
}

} // namespace bonefish

#endif // BONEFISH_IDENTIFIERS_WAMP_REGISTRATION_ID_GENERATOR_HPP
