#ifndef BONEFISH_IDENTIFIER_REGISTRATION_ID_GENERATOR_HPP
#define BONEFISH_IDENTIFIER_REGISTRATION_ID_GENERATOR_HPP

#include <bonefish/identifier/registration_id.hpp>
#include <bonefish/identifier/sequential_id_generator.hpp>

namespace bonefish {

class registration_id_generator
{
public:
    registration_id_generator();
    registration_id_generator(registration_id_generator const&) = delete;
    registration_id_generator& operator=(registration_id_generator const&) = delete;
    registration_id generate();

private:
    sequential_id_generator m_sequential_id_generator;
};

inline registration_id_generator::registration_id_generator()
    : m_sequential_id_generator(registration_id::MIN, registration_id::MAX)
{
}

inline registration_id registration_id_generator::generate()
{
    return registration_id(m_sequential_id_generator.generate());
}

} // namespace bonefish

#endif // BONEFISH_IDENTIFIER_REGISTRATION_ID_GENERATOR_HPP
