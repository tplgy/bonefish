#ifndef BONEFISH_IDENTIFIER_SUBSCRIPTION_ID_GENERATOR_HPP
#define BONEFISH_IDENTIFIER_SUBSCRIPTION_ID_GENERATOR_HPP

#include <bonefish/identifier/subscription_id.hpp>
#include <bonefish/identifier/sequential_id_generator.hpp>

namespace bonefish {

class subscription_id_generator
{
public:
    subscription_id_generator();
    subscription_id_generator(subscription_id_generator const&) = delete;
    subscription_id_generator& operator=(subscription_id_generator const&) = delete;
    subscription_id generate();

private:
    sequential_id_generator m_sequential_id_generator;
};

inline subscription_id_generator::subscription_id_generator()
    : m_sequential_id_generator(subscription_id::MIN, subscription_id::MAX)
{
}

inline subscription_id subscription_id_generator::generate()
{
    return subscription_id(m_sequential_id_generator.generate());
}

} // namespace bonefish

#endif // BONEFISH_IDENTIFIER_SUBSCRIPTION_ID_GENERATOR_HPP
