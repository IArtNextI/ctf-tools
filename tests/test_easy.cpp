#include <curl/curl.h>
#include <gtest/gtest.h>

#include "ctftools/ctftools.hpp"

#include <iostream>

TEST(Easy, GetGoogle) {
    auto ret = ctf::easy::Get("https://www.google.com");
    ASSERT_EQ(ret.status_code, 200);
}