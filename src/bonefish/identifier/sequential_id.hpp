#ifndef BONEFISH_IDENTIFIER_SEQUENTIAL_ID_HPP
#define BONEFISH_IDENTIFIER_SEQUENTIAL_ID_HPP

#include <cstdint>
#include <limits>
#include <sstream>
#include <stdexcept>

namespace bonefish {

class sequential_id
{
public:
    static const uint64_t MIN = 1;
    static const uint64_t MAX = 1ULL << 53;
    static const uint64_t INVALID = std::numeric_limits<uint64_t>::max();

public:
    sequential_id();
    explicit sequential_id(uint64_t id) throw(std::invalid_argument);
    bool is_valid() const;
    uint64_t id() const;

private:
    uint64_t m_id;
};

inline sequential_id::sequential_id()
    : m_id(INVALID)
{
}

inline sequential_id::sequential_id(uint64_t id) throw(std::invalid_argument)
    : m_id(id)
{
    if (m_id < MIN || m_id > MAX) {
        std::stringstream ss;
        ss << "sequential id " << m_id << " is out of range";
        throw std::invalid_argument(ss.str());
    }
}

inline bool sequential_id::is_valid() const
{
    return m_id != INVALID;
}

inline uint64_t sequential_id::id() const
{
    return m_id;
}

} // namespace bonefish

#endif // BONEFISH_IDENTIFIER_SEQUENTIAL_ID_HPP
