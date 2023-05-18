#include <curl/curl.h>
#include <gtest/gtest.h>

#include "ctftools/ctftools.hpp"

#include <iostream>

TEST(Easy, GetGoogle) {
    auto ret = ctf::easy::Get("https://www.google.com");
    ASSERT_EQ(ret.status_code, 200);
}

TEST(Easy, Caesar) {
    ASSERT_EQ(ctf::easy::crypto::Caesar("ABCDEFGHIJKLMNOPQRSTUVWXYZ"), "DEFGHIJKLMNOPQRSTUVWXYZABC");
    ASSERT_EQ(ctf::easy::crypto::Caesar("ABCDEFGHIJKLMNOPQRSTUVWXYZ", 23), "XYZABCDEFGHIJKLMNOPQRSTUVW");
    ASSERT_EQ(ctf::easy::crypto::Caesar("ABCDEFGHIJKLMNOPQRSTUVWXYZ", -7), ctf::easy::crypto::Caesar("ABCDEFGHIJKLMNOPQRSTUVWXYZ", 19));
    ASSERT_EQ(ctf::easy::crypto::Caesar("abcdefGHIJKLMNOP123UVwxYZ", 1), "bcdefgHIJKLMNOPQ123VWxyZA");
    ASSERT_EQ(ctf::easy::crypto::Caesar("abcdefGHIJKLMNOP123UVwxYZ", 27), "");
}

TEST(Easy, Atbash) {
    ASSERT_EQ(ctf::easy::crypto::Atbash("ABCDEFGHIJKLMNOPQRSTUVWXYZ"), "ZYXWVUTSRQPONMLKJIHGFEDCBA");
    ASSERT_EQ(ctf::easy::crypto::Atbash("abcdefghijklmnopqrstuvwxyz"), "zyxwvutsrqponmlkjihgfedcba");
    ASSERT_EQ(ctf::easy::crypto::Atbash(ctf::easy::crypto::Atbash("abcdEFGhijklm45pqrSTUvwxyz")), "abcdEFGhijklm45pqrSTUvwxyz");
}

TEST(Easy, Vigenere) {
    ASSERT_EQ(ctf::easy::crypto::Vigenere("ABCDEFGHIJKLMNOPQRSTUVWXYZ", "B"), "BCDEFGHIJKLMNOPQRSTUVWXYZA");
    ASSERT_EQ(ctf::easy::crypto::Vigenere("abcdefghijklmnopqrstuvwxyz", "AbC"), "acedfhgikjlnmoqprtsuwvxzya");
    ASSERT_EQ(ctf::easy::crypto::Vigenere("abcdEFGhijklm45pqrSTUvwxyz", "bc"), "bddfFHHjjllnn45rrtTVVxxzzb");
}
