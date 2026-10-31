/**
 *  Copyright (C) 2015 Topology LP
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef BONEFISH_WAMP_TRACE_HPP
#define BONEFISH_WAMP_TRACE_HPP

#include <cctype>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

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

/*!
 * Formats a printf-free format string using boost-format style "%N%"
 * placeholders (1-based), where each argument is rendered through its
 * operator<<. Unknown or out-of-range placeholders are emitted verbatim.
 */
inline std::string format(const std::string& fmt)
{
    return fmt;
}

template <typename T>
inline std::string to_string(const T& value)
{
    std::ostringstream out;
    out << value;
    return out.str();
}

template <typename... Args>
inline std::string format(const std::string& fmt, const Args&... args)
{
    std::vector<std::string> values { to_string(args)... };

    std::string result;
    result.reserve(fmt.size());

    size_t i = 0;
    while (i < fmt.size()) {
        char c = fmt[i];
        if (c != '%') {
            result += c;
            ++i;
            continue;
        }

        size_t j = i + 1;
        size_t n = 0;
        bool have_digits = false;
        while (j < fmt.size() && isdigit(static_cast<unsigned char>(fmt[j]))) {
            n = n * 10 + static_cast<size_t>(fmt[j] - '0');
            ++j;
            have_digits = true;
        }

        if (have_digits && j < fmt.size() && fmt[j] == '%') {
            if (n >= 1 && n <= values.size()) {
                result += values[n - 1];
            } else {
                result += fmt.substr(i, j + 1);
            }
            i = j + 1;
        } else {
            result += c;
            ++i;
        }
    }

    return result;
}

} // namespace trace
} // namespace bonefish

// Macro for facilitating debug trace logging.
#define BONEFISH_TRACE(...) \
    if (bonefish::trace::is_enabled()) { \
        std::cerr << "[" << bonefish::trace::base_file_name(__FILE__) << ":" << __LINE__ << "][" << __FUNCTION__ << "] " \
                << bonefish::trace::format(__VA_ARGS__) << std::endl; \
    }

#endif // BONEFISH_WAMP_TRACE_HPP
