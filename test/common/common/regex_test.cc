#include "envoy/common/exception.h"

#include "common/common/regex.h"

#include "test/test_common/utility.h"

#include "gtest/gtest.h"

namespace Envoy {
namespace Regex {
namespace {

TEST(Utility, parseStdRegex) {
  EXPECT_THROW_WITH_REGEX(Regex::Utility::parseStdRegex("(+invalid)"), EnvoyException,
                          "Invalid regex '\\(\\+invalid\\)': .+");

  {
    std::regex regex = Regex::Utility::parseStdRegex("x*");
    EXPECT_NE(0, regex.flags() & std::regex::optimize);
  }

  {
    std::regex regex = Regex::Utility::parseStdRegex("x*", std::regex::icase);
    EXPECT_NE(0, regex.flags() & std::regex::icase);
    EXPECT_EQ(0, regex.flags() & std::regex::optimize);
  }
}

} // namespace
} // namespace Regex
} // namespace Envoy
