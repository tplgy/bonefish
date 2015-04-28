#ifndef BONEFISH_WAMP_TRACE_HPP
#define BONEFISH_WAMP_TRACE_HPP

#include <boost/format.hpp>
#include <boost/preprocessor/arithmetic/sub.hpp>
#include <boost/preprocessor/control/if.hpp>
#include <boost/preprocessor/variadic/size.hpp>
#include <string.h>

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

inline const char* base_file_name(const char* file_path)
{
    const char* file_name = strrchr(file_path, '/');
    return file_name == nullptr ? file_path : file_name + 1;
}

} // namespace trace
} // namespace bonefish

// Macro for facilitating debug trace logging.
#define BONEFISH_TRACE(fmt, ...) \
    BOOST_PP_IF(BOOST_PP_SUB(BOOST_PP_VARIADIC_SIZE("dummy", ##__VA_ARGS__), 1), BONFISH_TRACE_ARGS(fmt, __VA_ARGS__), BONFISH_TRACE_NOARGS(fmt, __VA_ARGS__))

#define BONFISH_TRACE_NOARGS(fmt, ...) \
    std::cerr << "[" << bonefish::trace::base_file_name(__FILE__) << ":" << __LINE__ << "][" << __FUNCTION__ << "] " \
            << boost::format(fmt) << std::endl;

#define BONFISH_TRACE_ARGS(fmt, ...) \
    std::cerr << "[" << bonefish::trace::base_file_name(__FILE__) << ":" << __LINE__ << "][" << __FUNCTION__ << "] " \
            << (boost::format(fmt) % __VA_ARGS__) << std::endl;

#endif // BONEFISH_WAMP_TRACE_HPP
