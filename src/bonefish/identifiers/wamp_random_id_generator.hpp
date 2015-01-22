#ifndef BONEFISH_IDENTIFIERS_WAMP_RANDOM_ID_GENERATOR_HPP
#define BONEFISH_IDENTIFIERS_WAMP_RANDOM_ID_GENERATOR_HPP

#include <cstdint>
#include <random>
#include <stdexcept>

namespace bonefish {

class wamp_random_id_generator
{
public:
    explicit wamp_random_id_generator(uint64_t range_start, uint64_t range_end,
            uint64_t seed = std::mt19937_64::default_seed);
    wamp_random_id_generator(wamp_random_id_generator const&) = delete;
    wamp_random_id_generator& operator=(wamp_random_id_generator const&) = delete;
    uint64_t generate();

private:
    std::mt19937_64 m_generator;
    std::uniform_int_distribution<uint64_t> m_distribution;
};

inline wamp_random_id_generator::wamp_random_id_generator(
    uint64_t range_start, uint64_t range_end, uint64_t seed)
    : m_generator(seed)
    , m_distribution(range_start, range_end)
{
    if (range_start >= range_end) {
        throw std::invalid_argument("range is invalid");
    }
}

inline uint64_t wamp_random_id_generator::generate()
{
    return m_distribution(m_generator);
}

} // namespace bonefish

#endif // BONEFISH_IDENTIFIERS_WAMP_RANDOM_ID_GENERATOR_HPP
