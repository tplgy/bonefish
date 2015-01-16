#ifndef BONEFISH_ROLES_ROLE_HPP
#define BONEFISH_ROLES_ROLE_HPP

#include <bonefish/roles/role_features.hpp>
#include <bonefish/roles/role_type.hpp>

namespace bonefish {

class role
{
public:
    role(role_type type);
    ~role();

    role_type get_type() const;
    role_features& get_features();
    const role_features& get_features() const;
    void set_features(const role_features& features);

    bool operator==(const role& other) const;

private:
    role_type m_type;
    role_features m_features;
};

role::role(role_type type)
    : m_type(type)
{
}

role::~role()
{
}

role_type role::get_type() const
{
    return m_type;
}

role_features& role::get_features()
{
    return m_features;
}

const role_features& role::get_features() const
{
    return m_features;
}

void role::set_features(const role_features& features)
{
    m_features = features;
}

bool role::operator==(const role& other) const
{
    if (this == &other) {
        return true;
    }

    return m_type == other.m_type &&
            m_features == other.m_features;
}

} // namespace bonefish

namespace std {

template<> struct hash<bonefish::role>
{
    size_t operator()(const bonefish::role& role) const
    {
        return hash<bonefish::role_type>()(role.get_type());
    }
};

} // namespace std

#endif // BONEFISH_ROLES_ROLE_HPP
