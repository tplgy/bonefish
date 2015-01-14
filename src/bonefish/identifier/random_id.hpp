#ifndef BONEFISH_IDENTIFIER_RANDOM_ID_HPP
#define BONEFISH_IDENTIFIER_RANDOM_ID_HPP

#include <cstdint>
#include <limits>
#include <sstream>
#include <stdexcept>

namespace bonefish {

class random_id
{
public:
    static const uint64_t MIN = 0;
    static const uint64_t MAX = 1ULL << 53;
    static const uint64_t INVALID = std::numeric_limits<uint64_t>::max();

public:
    random_id();
    explicit random_id(uint64_t id) throw(std::invalid_argument);
    bool is_valid() const;
    uint64_t id() const;

    bool operator==(const random_id& other) const;

private:
    uint64_t m_id;
};

inline random_id::random_id()
    : m_id(INVALID)
{
}

inline random_id::random_id(uint64_t id) throw(std::invalid_argument)
    : m_id(id)
{
    if (m_id > MAX) {
        std::stringstream ss;
        ss << "random id " << m_id << " is out of range";
        throw std::invalid_argument(ss.str());
    }
}

inline bool random_id::is_valid() const
{
    return m_id != INVALID;
}

inline uint64_t random_id::id() const
{
    return m_id;
}

inline bool random_id::operator==(const random_id& other) const
{
    return m_id == other.m_id;
}

inline std::ostream& operator<<(std::ostream& os, const random_id& rid)
{
    if (rid.is_valid()) {
        os << rid.id();
    } else {
        os << "<<invalid>>";
    }

    return os;
}

} // namespace bonefish

namespace std {

template<> struct hash<bonefish::random_id>
{
    size_t operator()(const bonefish::random_id& rid) const
    {
        return hash<uint64_t>()(rid.id());
    }
};

} // namespace std

#endif // BONEFISH_IDENTIFIER_RANDOM_ID_HPP
