#ifndef BONEFISH_IDENTIFIERS_WAMP_SUBSCRIPTION_ID_GENERATOR_HPP
#define BONEFISH_IDENTIFIERS_WAMP_SUBSCRIPTION_ID_GENERATOR_HPP

#include <bonefish/identifiers/wamp_sequential_id_generator.hpp>
#include <bonefish/identifiers/wamp_subscription_id.hpp>

namespace bonefish {

class wamp_subscription_id_generator
{
public:
    wamp_subscription_id_generator();
    wamp_subscription_id_generator(wamp_subscription_id_generator const&) = delete;
    wamp_subscription_id_generator& operator=(wamp_subscription_id_generator const&) = delete;
    wamp_subscription_id generate();

private:
    wamp_sequential_id_generator m_sequential_id_generator;
};

inline wamp_subscription_id_generator::wamp_subscription_id_generator()
    : m_sequential_id_generator(wamp_subscription_id::MIN, wamp_subscription_id::MAX)
{
}

inline wamp_subscription_id wamp_subscription_id_generator::generate()
{
    return wamp_subscription_id(m_sequential_id_generator.generate());
}

} // namespace bonefish

#endif // BONEFISH_IDENTIFIERS_WAMP_SUBSCRIPTION_ID_GENERATOR_HPP
