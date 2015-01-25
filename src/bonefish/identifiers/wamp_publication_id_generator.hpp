#ifndef BONEFISH_IDENTIFIERS_WAMP_PUBLICATION_ID_GENERATOR_HPP
#define BONEFISH_IDENTIFIERS_WAMP_PUBLICATION_ID_GENERATOR_HPP

#include <bonefish/identifiers/wamp_publication_id.hpp>
#include <bonefish/identifiers/wamp_random_id_generator.hpp>
#include <cstdint>

namespace bonefish {

class wamp_publication_id_generator
{
public:
    wamp_publication_id_generator();
    explicit wamp_publication_id_generator(uint64_t seed);
    wamp_publication_id_generator(wamp_publication_id_generator const&) = delete;
    wamp_publication_id_generator& operator=(wamp_publication_id_generator const&) = delete;
    wamp_publication_id generate();

private:
    wamp_random_id_generator m_random_id_generator;
};

inline wamp_publication_id_generator::wamp_publication_id_generator()
    : m_random_id_generator(wamp_publication_id::MIN, wamp_publication_id::MAX)
{
}

inline wamp_publication_id_generator::wamp_publication_id_generator(uint64_t seed)
    : m_random_id_generator(wamp_publication_id::MIN, wamp_publication_id::MAX, seed)
{
}

inline wamp_publication_id wamp_publication_id_generator::generate()
{
    return wamp_publication_id(m_random_id_generator.generate());
}

} // namespace bonefish

#endif // BONEFISH_IDENTIFIERS_WAMP_PUBLICATION_ID_GENERATOR_HPP
