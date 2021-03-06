#include <catch.hpp>

#define GSL_THROW_ON_CONTRACT_VIOLATION
#include <multiformats/multibase.h>

#include <iostream>
#include <string>
#include <ctime>

using namespace std::string_literals; // enables s-suffix for std::string literals 

using namespace multiformats;


auto zstring = "Bonjour";
const auto ascii = "Hello"s;
const auto utf8 = u8"Привет"s;
const auto data = std::vector<unsigned char>{ 0xDE, 0xAD, 0xBE, 0xEF };

struct TestItem {
    base_t code;
    const char* zstring;
    const char* ascii;
    const char* utf8;
    const char* data;
};
constexpr TestItem _TestList[] = {
    { base2, 
        "01000010011011110110111001101010011011110111010101110010",
        "0100100001100101011011000110110001101111",
        "110100001001111111010001100000001101000010111000110100001011001011010000101101011101000110000010",
        "11011110101011011011111011101111",
    },
    { base8, 
        "1023366715233672562",
        "04414533066157" ,
        "64117721401502706413132055350602",
        "33653337357",
    },
    { base10, 
        "18699868485678450",
        "310939249775" ,
        "64566090598976661946233115010",
        "3735928559",
    },
    { base16, 
        "426f6e6a6f7572",
        "48656c6c6f" ,
        "d09fd180d0b8d0b2d0b5d182",
        "deadbeef",
    },
    { base32, 
        "ijxw42tpovza",
        "jbswy3dp" ,
        "2cp5dagqxdilfufv2gba",
        "32w353y",
    },
    { base32pad, 
        "ijxw42tpovza====",
        "jbswy3dp" ,
        "2cp5dagqxdilfufv2gba====",
        "32w353y=",
    },
    { base32hex, 
        "89nmsqjfelp0",
        "91imor3f" ,
        "q2ft306gn38b5k5lq610",
        "rqmrtro",
    },
    { base32hexpad, 
        "89nmsqjfelp0====",
        "91imor3f" ,
        "q2ft306gn38b5k5lq610====",
        "rqmrtro=",
    },
    { base32z, 
        "ejzsh4uxqi3y",
        "jb1sa5dx" ,
        "4nx7dygozdemfwfi4gby",
        "54s575a",
    },
    { base58flickr, 
        "3w2cQtovFG",
        "9aJCVZR" ,
        "4WkNkSZ5rdPgaoPru",
        "6G8Bpn",
    },
    { base58btc, 
        "3X2CqUPWgh",
        "9Ajdvzr", 
        "4wLoLsz5SDpGAPpSV",
        "6h8cQN",
    },
    { base64, 
        "Qm9uam91cg",
        "SGVsbG8", 
        "0J/RgNC40LLQtdGC",
        "3q2+7w",
    },
    { base64pad, 
        "Qm9uam91cg==",
        "SGVsbG8=", 
        "0J/RgNC40LLQtdGC",
        "3q2+7w==",
    },
    { base64url,  
        "Qm9uam91cg",
        "SGVsbG8", 
        "0J_RgNC40LLQtdGC",
        "3q2-7w",
    },
    { base64urlpad, 
        "Qm9uam91cg==",
        "SGVsbG8=" ,
        "0J_RgNC40LLQtdGC",
        "3q2-7w==",
    }
};
constexpr auto _TestSize = _countof(_TestList);

template <int I = 0, int N = _TestSize>
void test_loop(bool do_encode, bool do_multi) {
    constexpr auto _Index = details::find_baseimpl(_TestList[I].code);
    constexpr auto _Impl = details::_BaseTable[_Index];
    SECTION("Base '"s + _Impl.name + "'") {
        if (do_encode)
        {
            if (do_multi)
            {
                SECTION("multi-encode zstring") {
                    REQUIRE(encode_multibase<_TestList[I].code>(zstring) == _Impl.code + std::string{ _TestList[I].zstring });
                    REQUIRE(encode_multibase(_TestList[I].code, zstring) == _Impl.code + std::string{ _TestList[I].zstring });
                }
                SECTION("multi-encode ascii") {
                    REQUIRE(encode_multibase<_TestList[I].code>(ascii) == _Impl.code + std::string{ _TestList[I].ascii });
                    REQUIRE(encode_multibase(_TestList[I].code, ascii) == _Impl.code + std::string{ _TestList[I].ascii });
                }
                SECTION("multi-encode utf8") {
                    REQUIRE(encode_multibase<_TestList[I].code>(utf8) == _Impl.code + std::string{ _TestList[I].utf8 });
                    REQUIRE(encode_multibase(_TestList[I].code, utf8) == _Impl.code + std::string{ _TestList[I].utf8 });
                }
                SECTION("multi-encode data") {
                    REQUIRE(encode_multibase<_TestList[I].code>(data) == _Impl.code + std::string{ _TestList[I].data });
                    REQUIRE(encode_multibase(_TestList[I].code, data) == _Impl.code + std::string{ _TestList[I].data });
                }
            }
            else
            {
                SECTION("base-encode zstring") {
                    REQUIRE(encode<_TestList[I].code>(zstring) == _TestList[I].zstring);
                    REQUIRE(encode(_TestList[I].code, zstring) == _TestList[I].zstring);
                }
                SECTION("base-encode ascii") {
                    REQUIRE(encode<_TestList[I].code>(ascii) == _TestList[I].ascii);
                    REQUIRE(encode(_TestList[I].code, ascii) == _TestList[I].ascii);
                }
                SECTION("base-encode utf8") {
                    REQUIRE(encode<_TestList[I].code>(utf8) == _TestList[I].utf8);
                    REQUIRE(encode(_TestList[I].code, utf8) == _TestList[I].utf8);
                }
                SECTION("base-encode data") {
                    REQUIRE(encode<_TestList[I].code>(data) == _TestList[I].data);
                    REQUIRE(encode(_TestList[I].code, data) == _TestList[I].data);
                }
            }
        }
        else
        {
            if (do_multi)
            {
                SECTION("multi-decode zstring") {
                    REQUIRE(encode_multibase<_TestList[I].code>(zstring) == _Impl.code + std::string{ _TestList[I].zstring });
                    REQUIRE(encode_multibase(_TestList[I].code, zstring) == _Impl.code + std::string{ _TestList[I].zstring });
                }
                SECTION("multi-decode ascii") {
                    REQUIRE(encode_multibase<_TestList[I].code>(ascii) == _Impl.code + std::string{ _TestList[I].ascii });
                    REQUIRE(encode_multibase(_TestList[I].code, ascii) == _Impl.code + std::string{ _TestList[I].ascii });
                }
                SECTION("multi-decode utf8") {
                    REQUIRE(encode_multibase<_TestList[I].code>(utf8) == _Impl.code + std::string{ _TestList[I].utf8 });
                    REQUIRE(encode_multibase(_TestList[I].code, utf8) == _Impl.code + std::string{ _TestList[I].utf8 });
                }
                SECTION("multi-decode data") {
                    REQUIRE(encode_multibase<_TestList[I].code>(data) == _Impl.code + std::string{ _TestList[I].data });
                    REQUIRE(encode_multibase(_TestList[I].code, data) == _Impl.code + std::string{ _TestList[I].data });
                }
            }
            else
            {
                SECTION("base-decode zstring") {
                    REQUIRE(decode(encoded_string<_TestList[I].code>{ _TestList[I].zstring }) == to_buffer(zstring));
                    REQUIRE(decode({ _TestList[I].code, _TestList[I].zstring }) == to_buffer(zstring));
                    REQUIRE(decode(_TestList[I].code, _TestList[I].zstring) == to_buffer(zstring));
                }
                SECTION("base-decode ascii") {
                    REQUIRE(decode(encoded_string<_TestList[I].code>(_TestList[I].ascii)) == to_buffer(ascii));
                    REQUIRE(decode({ _TestList[I].code, _TestList[I].ascii }) == to_buffer(ascii));
                    REQUIRE(decode(_TestList[I].code, _TestList[I].ascii) == to_buffer(ascii));
                }
                SECTION("base-decode utf8") {
                    REQUIRE(decode(encoded_string<_TestList[I].code>(_TestList[I].utf8)) == to_buffer(utf8));
                    REQUIRE(decode({ _TestList[I].code, _TestList[I].utf8 }) == to_buffer(utf8));
                    REQUIRE(decode(_TestList[I].code, _TestList[I].utf8) == to_buffer(utf8));
                }
                SECTION("base-decode data") {
                    REQUIRE(decode(encoded_string<_TestList[I].code>(_TestList[I].data)) == data);
                    REQUIRE(decode({ _TestList[I].code, _TestList[I].data }) == data);
                    REQUIRE(decode(_TestList[I].code, _TestList[I].data) == data);
                }
            }
        }
    }
    test_loop<I + 1>(do_encode, do_multi);
}
template <>
void test_loop<_TestSize>(bool /*do_encode*/, bool /*do_multi*/) {
}

TEST_CASE("Test multibase implementation details")
{
    SECTION("`details::find_baseimpl` should return a non-null index if base is implemented") 
        REQUIRE(details::find_baseimpl(base2) > 0);

    SECTION("`details::find_baseimpl` should return a null index if base is not implemented") 
        REQUIRE(details::find_baseimpl(base_t('$')) == 0);

    SECTION("`details::find_baseimpl` should return a null index if base is undefined")       
        REQUIRE(details::find_baseimpl(dynamic_base) == 0);


    SECTION("`details::basecode_type` with a valid base code should return the right base code") {
        REQUIRE(details::basecode_type<base58btc>{base58btc}.base() == base58btc);
        REQUIRE(details::basecode_type<>{ base58btc }.base() == base58btc);
    }

    SECTION("`details::basecode_type` with an invalid/undef/mismatched base code must fail") {
#ifdef SHOULD_NOT_COMPILE
        REQUIRE(details::basecode_type<basecode('$')>{basecode('$')}.base() == base58btc);
#endif
        REQUIRE_THROWS(details::basecode_type<>{ base_t('$') }.base() == base58btc);
        REQUIRE_THROWS(details::basecode_type<>{ dynamic_base }.base() == dynamic_base);
        REQUIRE_THROWS(details::basecode_type<base58btc>{base64url}.base() == base58btc);
    }
}


TEST_CASE("Test `encoded_string<>` class")
{

    SECTION("empty `encoded_string<A>` with valid A is allowed and `base()` returns A") {
        REQUIRE(encoded_string<base58btc>{}.base() == base58btc);
        REQUIRE(encoded_string<>{base58btc}.base() == base58btc);
    }
    SECTION("empty `encoded_string<A>` with invalid/undef/mismatched A must fail") {
#ifdef SHOULD_NOT_COMPILE
        REQUIRE(encoded_string<basecode('$')>{}.base() == base58btc);
#endif
        REQUIRE_THROWS(encoded_string<>{base_t('$')}.base() == base58btc);
        REQUIRE_THROWS(encoded_string<>{}.base() == base58btc);
        REQUIRE_THROWS(encoded_string<base58btc>{base64url});
    }

    SECTION("`encoded_string<A>` with valid A initialized with a std::string is allowed and `base()` returns A") {
        auto a = "abcd"s;
        REQUIRE(encoded_string<base58btc>{a}.base() == base58btc);
        REQUIRE(encoded_string<>{base58btc, a}.base() == base58btc);
    }
    SECTION("`encoded_string<A>` with invalid/undef/mismatched A initialized with a std::string must fail") {
        auto a = "abcd"s;
#ifdef SHOULD_NOT_COMPILE
        REQUIRE(encoded_string<basecode('$')>{a});
#endif
        REQUIRE_THROWS(encoded_string<>{base_t('$'), a});
        REQUIRE_THROWS(encoded_string<>{a});
        REQUIRE_THROWS(encoded_string<base58btc>{base64url, a});
    }

    SECTION("`encoded_string<A>` with valid A initialized with a zstring is allowed and `base()` returns A") {
        REQUIRE(encoded_string<base58btc>{"abcd"}.base() == base58btc);
        REQUIRE(encoded_string<>{base58btc, "abcd"}.base() == base58btc);
    }
    SECTION("`encoded_string<A>` with invalid/undef/mismatched A initialized with a zstring must fail") {
#ifdef SHOULD_NOT_COMPILE
        REQUIRE(encoded_string<basecode('$')>{"abcd"});
#endif
        REQUIRE_THROWS(encoded_string<>{base_t('$'), "abcd"});
        REQUIRE_THROWS(encoded_string<>{"abcd"});
        REQUIRE_THROWS(encoded_string<base58btc>{base64url, "abcd"});
    }

    SECTION("`encoded_string<A>` with valid A initialized with a rvalue is allowed and `base()` returns A") {
        REQUIRE(encoded_string<base58btc>{"ab"s + "cd"}.base() == base58btc);
        REQUIRE(encoded_string<>{base58btc, "ab"s + "cd"}.base() == base58btc);
    }
    SECTION("`encoded_string<A>` with invalid/undef/mismatched A initialized with a rvalue must fail") {
#ifdef SHOULD_NOT_COMPILE
        REQUIRE(encoded_string<basecode('$')>{"ab"s + "cd"});
#endif
        REQUIRE_THROWS(encoded_string<>{base_t('$'), "ab"s + "cd"});
        REQUIRE_THROWS(encoded_string<>{"ab"s + "cd"});
        REQUIRE_THROWS(encoded_string<base58btc>{base64url, "ab"s + "cd"});
    }

    SECTION("`encoded_string<A>` can be compared to a string") {
        REQUIRE(encoded_string<base58btc>{"abcd"} == "abcd");
        REQUIRE("abcd" == encoded_string<base58btc>{"abcd"});

        REQUIRE(encoded_string<>{base58btc, "abcd"} == "abcd");
        REQUIRE("abcd" == encoded_string<>{base58btc, "abcd"});
    }

    SECTION("`encoded_string<A>` can be initialized with `encoded_string<B>` if A is dynamic_base or A.base==B.base") {
        {
            auto a = encoded_string<base58btc>{ "abcd" };
            auto b = encoded_string<base58btc>{ a };
            REQUIRE(a == b);
        }
        {
            auto a = encoded_string<>{ base58btc, "abcd" };
            auto b = encoded_string<base58btc>{ a };
            REQUIRE(a == b);
        }
        {
            auto a = encoded_string<base58btc>{ "abcd" };
            auto b = encoded_string<>{ a };
            REQUIRE(a == b);
        }
        {
            auto a = encoded_string<>{ base58btc, "abcd" };
            auto b = encoded_string<>{ a };
            REQUIRE(a == b);
        }
    }

    SECTION("`encoded_string<A>` must throw if initialized with `encoded_string<B>` and A!=B") {
        {
#ifdef SHOULD_NOT_COMPILE
            auto a = encoded_string<base58btc>{ "abcd" };
            REQUIRE_THROWS(encoded_string<base64url>{ a });
#endif
        }
        {
            auto a = encoded_string<>{ base58btc, "abcd" };
            REQUIRE_THROWS(encoded_string<base64url>{ a });
        }
    }

    SECTION("a<A> = b<B> is allowed if a is empty and b is not empty and A==B") {
        {
            auto a = encoded_string<base58btc>{};
            auto b = encoded_string<base58btc>{ "abcd" };
            a = b;
            REQUIRE(a == b);
        }
        {
            auto a = encoded_string<base58btc>{};
            auto b = encoded_string<>{ base58btc, "abcd" };
            a = b;
            REQUIRE(a == b);
        }
        {
            auto a = encoded_string<>{ base58btc };
            auto b = encoded_string<base58btc>{ "abcd" };
            a = b;
            REQUIRE(a == b);
        }
        {
            auto a = encoded_string<>{ base58btc };
            auto b = encoded_string<>{ base58btc, "abcd" };
            a = b;
            REQUIRE(a == b);
        }
    }

    SECTION("a<A> = b<B> must fail if a is not empty") {
        {
            auto a = encoded_string<base58btc>{ "abcd" };
            auto b = encoded_string<base58btc>{ "efgh" };
            REQUIRE_THROWS(a = b);
        }
        {
            auto a = encoded_string<>{ base58btc, "abcd" };
            auto b = encoded_string<base58btc>{ "efgh" };
            REQUIRE_THROWS(a = b);
        }
        {
            auto a = encoded_string<base58btc>{ "abcd" };
            auto b = encoded_string<>{ base58btc, "efgh" };
            REQUIRE_THROWS(a = b);
        }
        {
            auto a = encoded_string<>{ base58btc, "abcd" };
            auto b = encoded_string<>{ base58btc,"efgh" };
            REQUIRE_THROWS(a = b);
        }
    }
    SECTION("a<A> = b<B> must fail if b is empty") {
        {
            auto a = encoded_string<base58btc>{};
            auto b = encoded_string<base58btc>{};
            REQUIRE_THROWS(a = b);
        }
        {
            auto a = encoded_string<>{ base58btc };
            auto b = encoded_string<base58btc>{};
            REQUIRE_THROWS(a = b);
        }
        {
            auto a = encoded_string<base58btc>{};
            auto b = encoded_string<>{ base58btc };
            REQUIRE_THROWS(a = b);
        }
        {
            auto a = encoded_string<>{ base58btc };
            auto b = encoded_string<>{ base58btc };
            REQUIRE_THROWS(a = b);
        }
    }
    SECTION("a<A> = b<B> must fail if A!=B") {
#ifdef SHOULD_NOT_COMPILE
        {
            auto a = encoded_string<base58btc>{};
            auto b = encoded_string<base64url>{ "abcd" };
            REQUIRE_THROWS(a = b);
        }
#endif
        {
            auto a = encoded_string<base58btc>{};
            auto b = encoded_string<>{ base64url, "abcd" };
            REQUIRE_THROWS(a = b);
        }
        {
            auto a = encoded_string<>{ base58btc };
            auto b = encoded_string<base64url>{ "abcd" };
            REQUIRE_THROWS(a = b);
        }
        {
            auto a = encoded_string<>{ base58btc };
            auto b = encoded_string<>{ base64url, "abcd" };
            REQUIRE_THROWS(a = b);
        }
    }
}


TEST_CASE("encode<base_t>")
{
    SECTION("encode<A>(...) with invalid/undef A must fail ") {
#ifdef SHOULD_NOT_COMPILE
        REQUIRE_THROWS(encode<basecode('!')>(ascii));
        REQUIRE_THROWS(encode<basecode('!')>(data));

        REQUIRE_THROWS(encode<dynamic_base>(ascii));
        REQUIRE_THROWS(encode<dynamic_base>(data));
#endif
        REQUIRE_THROWS(encode(base_t('!'), ascii));
        REQUIRE_THROWS(encode(base_t('!'), data));

        REQUIRE_THROWS(encode(dynamic_base, ascii));
        REQUIRE_THROWS(encode(dynamic_base, data));
    }

    SECTION("encode<A>(string) should throw on empty input") {
        REQUIRE_THROWS(encode<base58btc>(buffer_t{}));
        REQUIRE_THROWS(encode<base58btc>(""));

        REQUIRE_THROWS(encode(base58btc, buffer_t{}));
        REQUIRE_THROWS(encode(base58btc, ""));
    }

    test_loop(true, false);
}

TEST_CASE("encode_multibase<base_t>")
{
    SECTION("encode_multibase<A>(...) with invalid/undef A must fail ") {
#ifdef SHOULD_NOT_COMPILE
        REQUIRE_THROWS(encode_multibase<basecode('!')>(ascii));
        REQUIRE_THROWS(encode_multibase<basecode('!')>(data));

        REQUIRE_THROWS(encode_multibase<dynamic_base>(ascii));
        REQUIRE_THROWS(encode_multibase<dynamic_base>(data));
#endif
        REQUIRE_THROWS(encode_multibase(base_t('!'), ascii));
        REQUIRE_THROWS(encode_multibase(base_t('!'), data));

        REQUIRE_THROWS(encode_multibase(dynamic_base, ascii));
        REQUIRE_THROWS(encode_multibase(dynamic_base, data));
    }

    SECTION("encode_multibase<A>(string) should throw on empty input") {
        REQUIRE_THROWS(encode_multibase<base58btc>(buffer_t{}));
        REQUIRE_THROWS(encode_multibase<base58btc>(""));

        REQUIRE_THROWS(encode_multibase(base58btc, buffer_t{}));
        REQUIRE_THROWS(encode_multibase(base58btc, ""));
    }

    test_loop(true, true);
}

TEST_CASE("decode<base_t>")
{
    SECTION("decode<A>(string) should throw on empty input") {
        REQUIRE_THROWS(decode(encoded_string<base58btc>{""}));
        REQUIRE_THROWS(decode({ base58btc, "" }));
        REQUIRE_THROWS(decode(base58btc, ""));
    }

    test_loop(false, false);
}

TEST_CASE("multidecode<base_t>")
{
    test_loop(false, true);
}

TEST_CASE("Fuzzy encode/decode<base_t>")
{
    std::srand(static_cast<uint32_t>(std::time(nullptr)));
    for (auto i = 0; i < 100; i++)
    {
        const auto nBytes = 1 + int(128 * std::rand() / RAND_MAX);
        auto buffer = buffer_t{ };
        for (auto j = 0; j < nBytes; j++)
        {
            buffer.push_back(gsl::narrow<byte_t>(std::rand()));
        }

        const auto base = static_cast<base_t>(base2 + int((base64urlpad + 1 - base2) * std::rand() / RAND_MAX));
        REQUIRE(decode(encode(base, buffer)) == buffer);
    }

}
