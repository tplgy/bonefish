#ifndef BONEFISH_IDENTIFIERS_WAMP_RANDOM_ID_HPP
#define BONEFISH_IDENTIFIERS_WAMP_RANDOM_ID_HPP

#include <cstdint>
#include <limits>
#include <sstream>
#include <stdexcept>

namespace bonefish {

class wamp_random_id
{
public:
    static const uint64_t MIN = 0;
    static const uint64_t MAX = 1ULL << 53;
    static const uint64_t INVALID = std::numeric_limits<uint64_t>::max();

public:
    wamp_random_id();
    explicit wamp_random_id(uint64_t id);
    bool is_valid() const;
    uint64_t id() const;

    bool operator==(const wamp_random_id& other) const;

private:
    uint64_t m_id;
};

inline wamp_random_id::wamp_random_id()
    : m_id(INVALID)
{
}

inline wamp_random_id::wamp_random_id(uint64_t id)
    : m_id(id)
{
    if (m_id > MAX) {
        std::stringstream ss;
        ss << "random id " << m_id << " is out of range";
        throw std::invalid_argument(ss.str());
    }
}

inline bool wamp_random_id::is_valid() const
{
    return m_id != INVALID;
}

inline uint64_t wamp_random_id::id() const
{
    return m_id;
}

inline bool wamp_random_id::operator==(const wamp_random_id& other) const
{
    return m_id == other.m_id;
}

inline std::ostream& operator<<(std::ostream& os, const wamp_random_id& rid)
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

template<> struct hash<bonefish::wamp_random_id>
{
    size_t operator()(const bonefish::wamp_random_id& id) const
    {
        return hash<uint64_t>()(id.id());
    }
};

} // namespace std

#endif // BONEFISH_IDENTIFIERS_WAMP_RANDOM_ID_HPP
