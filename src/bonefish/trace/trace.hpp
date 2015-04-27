#ifndef BONEFISH_WAMP_TRACE_HPP
#define BONEFISH_WAMP_TRACE_HPP

#include <boost/preprocessor/variadic/size.hpp>

namespace bonefish {
namespace trace {

extern bool _enabled;

inline bool is_enabled()
{
    return _enabled;
}

inline void set_enabled(bool is_enabled)
{
    _enabled = is_enabled;
}

} // namespace trace
} // namespace bonefish

// Macro for facilitating
#define BONEFISH_TRACE(fmt, ...) \
    if (bonefish::trace::is_enabled()) { \
        if (BOOST_PP_VARIADIC_SIZE("dummy", ##__VA_ARGS__) == 1) { \
            std::cerr << boost::format(fmt) << std::endl; \
        } else { \
            std::cerr << (boost::format(fmt) % #__VA_ARGS__) << std::endl; \
        } \
    }

#endif // BONEFISH_WAMP_TRACE_HPP
