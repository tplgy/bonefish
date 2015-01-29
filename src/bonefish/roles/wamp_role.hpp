#ifndef BONEFISH_ROLES_WAMP_ROLE_HPP
#define BONEFISH_ROLES_WAMP_ROLE_HPP

#include <bonefish/roles/wamp_role_features.hpp>
#include <bonefish/roles/wamp_role_type.hpp>

namespace bonefish {

class wamp_role
{
public:
    wamp_role(wamp_role_type type);
    ~wamp_role();

    wamp_role(wamp_role&& other);
    wamp_role& operator=(wamp_role&& other);
    wamp_role(const wamp_role& other);
    wamp_role& operator=(const wamp_role& other);

    wamp_role_type get_type() const;
    wamp_role_features& get_features();
    const wamp_role_features& get_features() const;
    void set_features(wamp_role_features&& features);

    bool operator==(const wamp_role& other) const;

private:
    wamp_role_type m_type;
    wamp_role_features m_features;
};

inline wamp_role::wamp_role(wamp_role_type type)
    : m_type(type)
{
}

inline wamp_role::~wamp_role()
{
}

inline wamp_role::wamp_role(wamp_role&& other)
    : m_type(other.m_type)
    , m_features(std::move(other.m_features))
{
}

inline wamp_role& wamp_role::operator=(wamp_role&& other)
{
    if (this != &other) {
        m_type = other.m_type;
        m_features = std::move(other.m_features);
    }

    return *this;
}

inline wamp_role::wamp_role(const wamp_role& other)
    : m_type(other.m_type)
    , m_features(other.m_features)
{
}

inline wamp_role& wamp_role::operator=(const wamp_role& other)
{
    if (this != &other) {
        m_type = other.m_type;
        m_features = other.m_features;
    }

    return *this;
}

inline wamp_role_type wamp_role::get_type() const
{
    return m_type;
}

inline wamp_role_features& wamp_role::get_features()
{
    return m_features;
}

inline const wamp_role_features& wamp_role::get_features() const
{
    return m_features;
}

inline void wamp_role::set_features(wamp_role_features&& features)
{
    m_features = std::move(features);
}

inline bool wamp_role::operator==(const wamp_role& other) const
{
    if (this == &other) {
        return true;
    }

    return m_type == other.m_type &&
            m_features == other.m_features;
}

} // namespace bonefish

namespace std {

template<> struct hash<bonefish::wamp_role>
{

size_t operator()(const bonefish::wamp_role& role) const
{
    return hash<bonefish::wamp_role_type>()(role.get_type());
}

};

} // namespace std

#endif // BONEFISH_ROLES_WAMP_ROLE_HPP
