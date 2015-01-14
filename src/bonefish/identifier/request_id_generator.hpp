#ifndef BONEFISH_IDENTIFIER_REQUEST_ID_GENERATOR_HPP
#define BONEFISH_IDENTIFIER_REQUEST_ID_GENERATOR_HPP

#include <bonefish/identifier/request_id.hpp>
#include <bonefish/identifier/sequential_id_generator.hpp>

namespace bonefish {

class request_id_generator
{
public:
    request_id_generator();
    request_id_generator(request_id_generator const&) = delete;
    request_id_generator& operator=(request_id_generator const&) = delete;
    request_id generate();

private:
    sequential_id_generator m_sequential_id_generator;
};

inline request_id_generator::request_id_generator()
    : m_sequential_id_generator(request_id::MIN, request_id::MAX)
{
}

inline request_id request_id_generator::generate()
{
    return request_id(m_sequential_id_generator.generate());
}

} // namespace bonefish

#endif // BONEFISH_IDENTIFIER_REQUEST_ID_GENERATOR_HPP
