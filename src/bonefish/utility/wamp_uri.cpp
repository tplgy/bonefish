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

#include <bonefish/utility/wamp_uri.hpp>

#ifdef USE_BOOST_REGEX

#include <boost/regex.hpp>
#define REGEX_RETURN(uri, regex_name, pcre_regex) { \
    static boost::regex regex_name(pcre_regex, \
            boost::regex_constants::nosubs | boost::regex_constants::optimize); \
    return boost::regex_match(uri, regex_name); \
}

#else // std::regex is supported (gcc >= 4.9, clang)

#include <regex>
#define REGEX_RETURN(uri, regex_name, pcre_regex) { \
    static std::regex regex_name(pcre_regex, \
            std::regex_constants::nosubs | std::regex_constants::optimize); \
    return std::regex_match(uri, regex_name); \
}

#endif

namespace bonefish {

namespace uri_flags {
unsigned int allow_empty_components = 1;
unsigned int strict = 1 << 1;
} // namespace uri_flags

bool is_valid_uri(const std::string& uri, int flags)
{
    if (flags & uri_flags::allow_empty_components) {
        if (flags & uri_flags::strict) {
            REGEX_RETURN(uri, strict_uri_regex_empty, "^(([0-9a-z_]+\\.)|\\.)*([0-9a-z_]+)?$");
        }
        REGEX_RETURN(uri, relaxed_uri_regex_empty, "^(([^\\s\\.#]+\\.)|\\.)*([^\\s\\.#]+)?$");
    }

    if (flags & uri_flags::strict) {
        REGEX_RETURN(uri, strict_uri_regex, "^([0-9a-z_]+\\.)*([0-9a-z_]+)$");
    }
    REGEX_RETURN(uri, relaxed_uri_regex, "^([^\\s\\.#]+\\.)*([^\\s\\.#]+)$");
}

} // namespace bonefish
