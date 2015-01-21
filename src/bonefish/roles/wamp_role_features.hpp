#ifndef BONEFISH_ROLES_WAMP_ROLE_FEATURES_HPP
#define BONEFISH_ROLES_WAMP_ROLE_FEATURES_HPP

#include <string>
#include <unordered_map>

namespace bonefish {

class wamp_role_features
{
public:
    wamp_role_features();
    ~wamp_role_features();

    void set_option(const std::string& option, bool supported);
    bool get_option(const std::string& option) const;
    const std::unordered_map<std::string, bool>& get_options() const;
    bool operator==(const wamp_role_features& features) const;

private:
    std::unordered_map<std::string, bool> m_options;
};

inline wamp_role_features::wamp_role_features()
    : m_options()
{
}

inline wamp_role_features::~wamp_role_features()
{
}

inline void wamp_role_features::set_option(const std::string& option, bool supported)
{
    m_options[option] = supported;
}

inline bool wamp_role_features::get_option(const std::string& option) const
{
    auto itr = m_options.find(option);
    return itr != m_options.end() ? itr->second : false;
}

inline const std::unordered_map<std::string, bool>& wamp_role_features::get_options() const
{
    return m_options;
}

inline bool wamp_role_features::operator==(const wamp_role_features& other) const
{
    if (this == &other) {
        return true;
    }

    return m_options == other.m_options;
}

} // namespace bonefish

#endif // BONEFISH_ROLES_WAMP_ROLE_FEATURES_HPP
