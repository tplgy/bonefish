#include "bonefish_test.hpp"

#include <bonefish/trace/trace.hpp>

#include <string>

using namespace bonefish;

int main()
{
    // %N% placeholder semantics (1-based, repeated, out-of-range verbatim).
    CHECK_EQ(trace::format("plain"), std::string("plain"));
    CHECK_EQ(trace::format("%1%", 1), std::string("1"));
    CHECK_EQ(trace::format("%1%, %2%", "a", 2), std::string("a, 2"));
    CHECK_EQ(trace::format("%2% before %1%", "one", "two"), std::string("two before one"));
    CHECK_EQ(trace::format("%1% %1%", "x"), std::string("x x"));
    CHECK_EQ(trace::format("out of range: %3%", 1, 2), std::string("out of range: %3%"));
    CHECK_EQ(trace::format("literal %% sign"), std::string("literal %% sign"));
    CHECK_EQ(trace::format("percent %x here", 1), std::string("percent %x here"));
    CHECK_EQ(trace::format("trailing %"), std::string("trailing %"));
    CHECK_EQ(trace::format("colon %1%:%2%", std::string("host"), 1888),
             std::string("colon host:1888"));

    return test_summary("trace");
}
