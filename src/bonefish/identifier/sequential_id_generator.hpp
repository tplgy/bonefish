#ifndef BONEFISH_IDENTIFIER_SEQUENTIAL_ID_GENERATOR_HPP
#define BONEFISH_IDENTIFIER_SEQUENTIAL_ID_GENERATOR_HPP

#include <cstdint>
#include <stdexcept>

namespace bonefish {

class sequential_id_generator
{
public:
    explicit sequential_id_generator(uint64_t range_start, uint64_t range_end)
            throw(std::invalid_argument);
    sequential_id_generator(sequential_id_generator const&) = delete;
    sequential_id_generator& operator=(sequential_id_generator const&) = delete;
    uint64_t generate();

private:
    const uint64_t m_range_start;
    const uint64_t m_range_end;
    uint64_t m_next_id;
};

inline sequential_id_generator::sequential_id_generator(
        uint64_t range_start, uint64_t range_end) throw(std::invalid_argument)
    : m_range_start(range_start)
    , m_range_end(range_end)
    , m_next_id(m_range_start)
{
    if (m_range_start >= m_range_end) {
        throw std::invalid_argument("range is invalid");
    }
}

inline uint64_t sequential_id_generator::generate()
{
    if (m_next_id < m_range_start ||
        m_next_id > m_range_end) {
        m_next_id = m_range_start;
    }

    return m_next_id++;
}

} // namespace bonefish

#endif // BONEFISH_IDENTIFIER_SEQUENTIAL_ID_GENERATOR_HPP
