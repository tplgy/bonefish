#include "bonefish_test.hpp"

#include <bonefish/utility/wamp_uri.hpp>

#include <string>

using namespace bonefish;

int main()
{
    // Strict URIs: [0-9a-z_]+ segments separated by dots.
    CHECK(is_valid_uri("a.b.c", uri_flags::strict));
    CHECK(is_valid_uri("com.example.procedure_1", uri_flags::strict));
    CHECK(!is_valid_uri("A.b", uri_flags::strict));
    CHECK(!is_valid_uri("a..b", uri_flags::strict));
    CHECK(!is_valid_uri("a.b#", uri_flags::strict));
    CHECK(!is_valid_uri("", uri_flags::strict));

    // Relaxed URIs: any non-whitespace except '#' and empty segments
    // when allowed.
    CHECK(is_valid_uri("some.topic", 0));
    CHECK(!is_valid_uri("some topic", 0));
    CHECK(!is_valid_uri("some#topic", 0));

    // Empty-component handling.
    CHECK(is_valid_uri("a..b", uri_flags::allow_empty_components));
    CHECK(is_valid_uri(".a.", uri_flags::allow_empty_components));
    CHECK(is_valid_uri("", uri_flags::allow_empty_components));
    CHECK(!is_valid_uri("a..b", 0));

    // Strict + empty components.
    CHECK(is_valid_uri("com..example", uri_flags::allow_empty_components | uri_flags::strict));
    CHECK(!is_valid_uri("Com.Example", uri_flags::allow_empty_components | uri_flags::strict));

    return test_summary("wamp_uri");
}
