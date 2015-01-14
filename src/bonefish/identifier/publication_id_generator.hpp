#ifndef BONEFISH_IDENTIFIER_PUBLICATION_ID_GENERATOR_HPP
#define BONEFISH_IDENTIFIER_PUBLICATION_ID_GENERATOR_HPP

#include <bonefish/identifier/publication_id.hpp>
#include <bonefish/identifier/random_id_generator.hpp>
#include <cstdint>

namespace bonefish {

class publication_id_generator
{
public:
    publication_id_generator();
    explicit publication_id_generator(uint64_t seed);
    publication_id_generator(publication_id_generator const&) = delete;
    publication_id_generator& operator=(publication_id_generator const&) = delete;
    publication_id generate();

private:
    random_id_generator m_random_id_generator;
};

inline publication_id_generator::publication_id_generator()
    : m_random_id_generator(publication_id::MIN, publication_id::MAX)
{
}

inline publication_id_generator::publication_id_generator(uint64_t seed)
    : m_random_id_generator(publication_id::MIN, publication_id::MAX, seed)
{
}

inline publication_id publication_id_generator::generate()
{
    return publication_id(m_random_id_generator.generate());
}

} // namespace bonefish

#endif // BONEFISH_IDENTIFIER_PUBLICATION_ID_GENERATOR_HPP
