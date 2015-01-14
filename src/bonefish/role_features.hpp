#ifndef BONEFISH_ROLE_FEATURES_HPP
#define BONEFISH_ROLE_FEATURES_HPP

#include <unordered_map>

namespace bonefish {

class role_features
{
public:
    role_features();
    ~role_features();

    void set_feature(const std::string& feature, bool supported);
    bool get_feature(const std::string& feature) const;

private:
    std::unordered_map<std::string, bool> m_features;
};

inline role_features::role_features()
    : m_features()
{
}

inline role_features::~role_features()
{
}

inline void role_features::set_feature(const std::string& feature, bool supported)
{
    m_features[feature] = supported;
}

inline bool role_features::get_feature(const std::string& feature) const
{
    auto itr = m_features.find(feature);
    return itr != m_features.end() ? itr->second : false;
}

} // namespace bonefish

#endif // BONEFISH_ROLE_FEATURES_HPP
