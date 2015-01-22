#ifndef BONEFISH_IDENTIFIERS_WAMP_REQUEST_ID_GENERATOR_HPP
#define BONEFISH_IDENTIFIERS_WAMP_REQUEST_ID_GENERATOR_HPP

#include <bonefish/identifier/wamp_request_id.hpp>
#include <bonefish/identifier/wamp_sequential_id_generator.hpp>

namespace bonefish {

class wamp_request_id_generator
{
public:
    wamp_request_id_generator();
    wamp_request_id_generator(wamp_request_id_generator const&) = delete;
    wamp_request_id_generator& operator=(wamp_request_id_generator const&) = delete;
    wamp_request_id generate();

private:
    sequential_id_generator m_sequential_id_generator;
};

inline wamp_request_id_generator::wamp_request_id_generator()
    : m_sequential_id_generator(request_id::MIN, request_id::MAX)
{
}

inline wamp_request_id wamp_request_id_generator::generate()
{
    return wamp_request_id(m_sequential_id_generator.generate());
}

} // namespace bonefish

#endif // BONEFISH_IDENTIFIERS_WAMP_REQUEST_ID_GENERATOR_HPP
