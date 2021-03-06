#include <catch.hpp> // for AssertionHandler, StringRef, CHECK, TEST_...
#include <multiformats/uvarint.h>
#include <array>
#include <ctime>

using namespace multiformats;

TEST_CASE("FuzzTest")
{
    std::srand(static_cast<uint32_t>(std::time(nullptr)));
    for (auto i = 0; i < 100; i++)
    {
        const auto nBytes = 1 + int(7 * std::rand() / RAND_MAX);
        auto value = uint64_t{ 0 };
        for (auto j = 0; j < nBytes; j++)
        {
            value <<= 8;
            value += static_cast<unsigned char>(std::rand());
        }
        value &= 0x7FFF'FFFF'FFFF'FFFF;
        auto e = uvarint::encode(value);
        auto result = uvarint::decode(e);
        REQUIRE(value == result);
    }
}

TEST_CASE("Test1ByteEncoding")
{
    {
        auto v = uvarint::encode(uint8_t(0x00));
        CHECK(v.size() == 1);
        CHECK(v[0] == 0x00);
    }
    {
        auto v = uvarint::encode(uint8_t(0x7F));
        CHECK(v.size() == 1);
        CHECK(v[0] == 0x7F);
    }
    {
        auto v = uvarint::encode(uint8_t(0xFF));
        CHECK(v.size() == 2);
        CHECK(v[0] == 0x81);
        CHECK(v[1] == 0x7F);
    }
}
TEST_CASE("Test2BytesEncoding")
{
    {
        auto v = uvarint::encode(uint16_t(0x0100));
        CHECK(v.size() == 2);
        CHECK(v[0] == 0x82);
        CHECK(v[1] == 0x00);
    }
    {
        auto v = uvarint::encode(uint16_t(0x3FFF));
        CHECK(v.size() == 2);
        CHECK(v[0] == 0xFF);
        CHECK(v[1] == 0x7F);
    }
    {
        auto v = uvarint::encode(uint16_t(0xFFFF));
        CHECK(v.size() == 3);
        CHECK(v[0] == 0x83);
        CHECK(v[1] == 0xFF);
        CHECK(v[2] == 0x7F);
    }
}
TEST_CASE("Test4BytesEncoding")
{
    {
        auto v = uvarint::encode(uint32_t(0x0001'0000));
        CHECK(v.size() == 3);
        CHECK(v[0] == 0x84);
        CHECK(v[1] == 0x80);
        CHECK(v[2] == 0x00);
    }
    {
        auto v = uvarint::encode(uint32_t(0x001F'FFFF));
        CHECK(v.size() == 3);
        CHECK(v[0] == 0xFF);
        CHECK(v[1] == 0xFF);
        CHECK(v[2] == 0x7F);
    }
    {
        auto v = uvarint::encode(uint32_t(0x0FFF'FFFF));
        CHECK(v.size() == 4);
        CHECK(v[0] == 0xFF);
        CHECK(v[1] == 0xFF);
        CHECK(v[2] == 0xFF);
        CHECK(v[3] == 0x7F);
    }
    {
        auto v = uvarint::encode(uint32_t(0xFFFF'FFFF));
        CHECK(v.size() == 5);
        CHECK(v[0] == 0x8F);
        CHECK(v[1] == 0xFF);
        CHECK(v[2] == 0xFF);
        CHECK(v[3] == 0xFF);
        CHECK(v[4] == 0x7F);
    }
}
TEST_CASE("Test8BytesEncoding")
{
    {
        auto v = uvarint::encode(uint64_t(0x0000'0001'0000'0000));
        CHECK(v.size() == 5);
        CHECK(v[0] == 0x90);
        CHECK(v[1] == 0x80);
        CHECK(v[2] == 0x80);
        CHECK(v[3] == 0x80);
        CHECK(v[4] == 0x00);
    }
    {
        auto v = uvarint::encode(uint64_t(0x0000'0007'FFFF'FFFF));
        CHECK(v.size() == 5);
        CHECK(v[0] == 0xFF);
        CHECK(v[1] == 0xFF);
        CHECK(v[2] == 0xFF);
        CHECK(v[3] == 0xFF);
        CHECK(v[4] == 0x7F);
    }
    {
        auto v = uvarint::encode(uint64_t(0x0000'0008'0000'0000));
        CHECK(v.size() == 6);
        CHECK(v[0] == 0x81);
        CHECK(v[1] == 0x80);
        CHECK(v[2] == 0x80);
        CHECK(v[3] == 0x80);
        CHECK(v[4] == 0x80);
        CHECK(v[5] == 0x00);
    }
    {
        auto v = uvarint::encode(uint64_t(0x0000'03FF'FFFF'FFFF));
        CHECK(v.size() == 6);
        CHECK(v[0] == 0xFF);
        CHECK(v[1] == 0xFF);
        CHECK(v[2] == 0xFF);
        CHECK(v[3] == 0xFF);
        CHECK(v[4] == 0xFF);
        CHECK(v[5] == 0x7F);
    }
    {
        auto v = uvarint::encode(uint64_t(0x0001'FFFF'FFFF'FFFF));
        CHECK(v.size() == 7);
        CHECK(v[0] == 0xFF);
        CHECK(v[1] == 0xFF);
        CHECK(v[2] == 0xFF);
        CHECK(v[3] == 0xFF);
        CHECK(v[4] == 0xFF);
        CHECK(v[5] == 0xFF);
        CHECK(v[6] == 0x7F);
    }
    {
        auto v = uvarint::encode(uint64_t(0x00FF'FFFF'FFFF'FFFF));
        CHECK(v.size() == 8);
        CHECK(v[0] == 0xFF);
        CHECK(v[1] == 0xFF);
        CHECK(v[2] == 0xFF);
        CHECK(v[3] == 0xFF);
        CHECK(v[4] == 0xFF);
        CHECK(v[5] == 0xFF);
        CHECK(v[6] == 0xFF);
        CHECK(v[7] == 0x7F);
    }
    {
        auto v = uvarint::encode(uint64_t(0x7FFF'FFFF'FFFF'FFFF));
        CHECK(v.size() == 9);
        CHECK(v[0] == 0xFF);
        CHECK(v[1] == 0xFF);
        CHECK(v[2] == 0xFF);
        CHECK(v[3] == 0xFF);
        CHECK(v[4] == 0xFF);
        CHECK(v[5] == 0xFF);
        CHECK(v[6] == 0xFF);
        CHECK(v[7] == 0xFF);
        CHECK(v[8] == 0x7F);
    }
}
TEST_CASE("TestOutOfRange")
{
    CHECK_THROWS_AS(uvarint::encode(uint64_t(0x8000'0000'0000'0000)), std::out_of_range);
}

TEST_CASE("TestEncodeWithPointer")
{
    uint8_t buf[9] = {0};
    auto endIt = uvarint::encode(uint64_t(0x7FED'CBA9'8765'4321), buf);
    CHECK(endIt == buf + 9);
    CHECK(buf[0] == 0xFF);
    CHECK(buf[1] == 0xF6);
    CHECK(buf[2] == 0xF2);
    CHECK(buf[3] == 0xF5);
    CHECK(buf[4] == 0x98);
    CHECK(buf[5] == 0xBB);
    CHECK(buf[6] == 0x95);
    CHECK(buf[7] == 0x86);
    CHECK(buf[8] == 0x21);
}
//TEST_CASE("TestEncodeInCArrayOfInt_CompilationError")
//{
//    int buf[9] = { 0 };
//    auto endIt = uvarint::encode(uint64_t(0x7FED'CBA9'8765'4321), buf);
//    CHECK(endIt == buf + 9);
//    CHECK(buf[0] == 0xFF);
//    CHECK(buf[1] == 0xF6);
//    CHECK(buf[2] == 0xF2);
//    CHECK(buf[3] == 0xF5);
//    CHECK(buf[4] == 0x98);
//    CHECK(buf[5] == 0xBB);
//    CHECK(buf[6] == 0x95);
//    CHECK(buf[7] == 0x86);
//    CHECK(buf[8] == 0x21);
//}
TEST_CASE("TestEncodeWithIterator")
{
    auto buf = std::array<unsigned char, 9>{};
    auto endIt = uvarint::encode(uint64_t(0x7FED'CBA9'8765'4321), std::begin(buf));
    CHECK(endIt == buf.end());
    CHECK(buf[0] == 0xFF);
    CHECK(buf[1] == 0xF6);
    CHECK(buf[2] == 0xF2);
    CHECK(buf[3] == 0xF5);
    CHECK(buf[4] == 0x98);
    CHECK(buf[5] == 0xBB);
    CHECK(buf[6] == 0x95);
    CHECK(buf[7] == 0x86);
    CHECK(buf[8] == 0x21);
}
TEST_CASE("TestEncodeWithInserter")
{
    auto buf = std::vector<unsigned char>{};
    auto endIt = uvarint::encode(uint64_t(0x7FED'CBA9'8765'4321), std::back_inserter(buf));
    CHECK(buf.size() == 9);
    CHECK(buf[0] == 0xFF);
    CHECK(buf[1] == 0xF6);
    CHECK(buf[2] == 0xF2);
    CHECK(buf[3] == 0xF5);
    CHECK(buf[4] == 0x98);
    CHECK(buf[5] == 0xBB);
    CHECK(buf[6] == 0x95);
    CHECK(buf[7] == 0x86);
    CHECK(buf[8] == 0x21);
}