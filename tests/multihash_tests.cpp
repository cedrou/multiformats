#include <catch.hpp>

#define GSL_THROW_ON_CONTRACT_VIOLATION
#include <multiformats/multihash.h>

using namespace multiformats;

struct TestItem {
    hash_t code;
    const char* digest;
};
constexpr TestItem _TestList[] = {
    { sha1, 
        "f7ff9e8b7bb2e09b70935a5d785e0cc5d9d0abf0",
    },
    { sha2_256, 
        "185f8db32271fe25f561a6fc938b2e264306ec304eda518007d1764826381969",
    },
};
constexpr auto _TestSize = _countof(_TestList);



TEST_CASE("Test multihash implementation details")
{
    SECTION("`details::find_hashimpl_by_key` should return a non-null index if hash is implemented") 
        REQUIRE(details::find_hashimpl_by_key(sha1) > 0);

    SECTION("`details::find_hashimpl_by_key` should return a null index if hash is not implemented") 
        REQUIRE(details::find_hashimpl_by_key(hash_t('$')) == 0);

    SECTION("`details::find_hashimpl_by_key` should return a null index if hash is undefined")       
        REQUIRE(details::find_hashimpl_by_key(dynamic_hash) == 0);



    SECTION("`details::hashcode_type` with a valid hash code should return the right hash code") {
        REQUIRE(details::hashcode_type<sha1>{sha1}.hash() == sha1);
        REQUIRE(details::hashcode_type<>{ sha1 }.hash() == sha1);
    }

    SECTION("`details::hashcode_type` with an invalid/undef/mismatched hash code must fail") {
#ifdef SHOULD_NOT_COMPILE
        REQUIRE_THROWS(details::hashcode_type<hash_t('$')>{hash_t('$')});
#endif
        REQUIRE_THROWS(details::hashcode_type<>{ hash_t('$') });
        REQUIRE_THROWS(details::hashcode_type<>{ dynamic_hash });
        REQUIRE_THROWS(details::hashcode_type<sha1>{sha2_256});
    }
}


TEST_CASE("`digest_string<>` class")
{
    SECTION("empty `digest_string<H,B>` with valid A and H is allowed and `hash()==H` and `base()==B`") {
        {
            auto s = digest_string<sha1, base16>{};
            REQUIRE(s.hash() == sha1);
            REQUIRE(s.base() == base16);
        }
        {
            auto s = digest_string<sha1>{base16};
            REQUIRE(s.hash() == sha1);
            REQUIRE(s.base() == base16);
        }
        {
            auto s = digest_string<dynamic_hash,base16>{ sha1 };
            REQUIRE(s.hash() == sha1);
            REQUIRE(s.base() == base16);
        }
        {
            auto s = digest_string<>{ sha1, base16 };
            REQUIRE(s.hash() == sha1);
            REQUIRE(s.base() == base16);
        }
    }
    SECTION("empty `digest_string<H,B>` with invalid/undef/mismatched H,B must fail") {
        REQUIRE_THROWS(digest_string<sha1, base16>{sha2_256});
        REQUIRE_THROWS(digest_string<sha1, base16>{base32});
        REQUIRE_THROWS(digest_string<sha1, base16>{sha1, base32});
        REQUIRE_THROWS(digest_string<sha1, base16>{sha2_256, base16});
        REQUIRE_THROWS(digest_string<sha1, base16>{sha2_256, base32});

#ifdef SHOULD_NOT_COMPILE
        REQUIRE_THROWS(digest_string<hash_t('$'), base16>{});
        REQUIRE_THROWS(digest_string<sha1, base_t('!')>{});
        REQUIRE_THROWS(digest_string<hash_t('$'), base_t('!')>{});
#endif
        REQUIRE_THROWS(digest_string<dynamic_hash,base16>{hash_t('$')});
        REQUIRE_THROWS(digest_string<sha1>{base_t('!')});
        REQUIRE_THROWS(digest_string<>{hash_t('$'), base_t('!')});

        REQUIRE_THROWS(digest_string<dynamic_hash, base16>{});
        REQUIRE_THROWS(digest_string<dynamic_hash, base16>{dynamic_hash});
        REQUIRE_THROWS(digest_string<dynamic_hash, base16>{dynamic_hash, base16});
        REQUIRE_THROWS(digest_string<sha1>{});
        REQUIRE_THROWS(digest_string<sha1>{dynamic_base});
        REQUIRE_THROWS(digest_string<sha1>{sha1, dynamic_base});
        REQUIRE_THROWS(digest_string<>{});
        REQUIRE_THROWS(digest_string<>{dynamic_base});
        REQUIRE_THROWS(digest_string<>{dynamic_hash});
        REQUIRE_THROWS(digest_string<>{dynamic_hash, dynamic_base});
    }

    SECTION("`digest_string<H,B>` with valid H/B, initialized with a std::string is allowed and `hash()==H` and `base()==B`") {
        {
            auto s = digest_string<sha1, base16>{ _TestList[0].digest };
            REQUIRE(s.hash() == sha1);
            REQUIRE(s.base() == base16);
        }
        {
            auto s = digest_string<sha1>{ base16, _TestList[0].digest };
            REQUIRE(s.hash() == sha1);
            REQUIRE(s.base() == base16);
        }
        {
            auto s = digest_string<dynamic_hash, base16>{ sha1, _TestList[0].digest };
            REQUIRE(s.hash() == sha1);
            REQUIRE(s.base() == base16);
        }
        {
            auto s = digest_string<>{ sha1, base16, _TestList[0].digest };
            REQUIRE(s.hash() == sha1);
            REQUIRE(s.base() == base16);
        }
    }
    SECTION("`digest_string<H,B>` with invalid/undef/mismatched H/B, initialized with a std::string must fail") {
        REQUIRE_THROWS(digest_string<sha1, base16>{sha2_256, _TestList[0].digest});
        REQUIRE_THROWS(digest_string<sha1, base16>{base32, _TestList[0].digest});
        REQUIRE_THROWS(digest_string<sha1, base16>{sha1, base32, _TestList[0].digest});
        REQUIRE_THROWS(digest_string<sha1, base16>{sha2_256, base16, _TestList[0].digest});
        REQUIRE_THROWS(digest_string<sha1, base16>{sha2_256, base32, _TestList[0].digest});

#ifdef SHOULD_NOT_COMPILE
        REQUIRE_THROWS(digest_string<hash_t('$'), base16>{ _TestList[0].digest});
        REQUIRE_THROWS(digest_string<sha1, base_t('!')>{ _TestList[0].digest});
        REQUIRE_THROWS(digest_string<hash_t('$'), base_t('!')>{ _TestList[0].digest});
#endif
        REQUIRE_THROWS(digest_string<dynamic_hash, base16>{hash_t('$'), _TestList[0].digest});
        REQUIRE_THROWS(digest_string<sha1>{base_t('!'), _TestList[0].digest});
        REQUIRE_THROWS(digest_string<>{hash_t('$'), base_t('!'), _TestList[0].digest});

#ifdef SHOULD_NOT_COMPILE
        REQUIRE_THROWS(digest_string<dynamic_hash, base16>{ _TestList[0].digest });
        REQUIRE_THROWS(digest_string<>{ _TestList[0].digest });
        REQUIRE_THROWS(digest_string<>{ dynamic_base, _TestList[0].digest });
#endif
        REQUIRE_THROWS(digest_string<dynamic_hash, base16>{dynamic_hash, _TestList[0].digest});
        REQUIRE_THROWS(digest_string<dynamic_hash, base16>{dynamic_hash, base16, _TestList[0].digest});
        REQUIRE_THROWS(digest_string<sha1>{ _TestList[0].digest});
        REQUIRE_THROWS(digest_string<sha1>{dynamic_base, _TestList[0].digest});
        REQUIRE_THROWS(digest_string<sha1>{sha1, dynamic_base, _TestList[0].digest});
        REQUIRE_THROWS(digest_string<>{dynamic_hash, _TestList[0].digest});
        REQUIRE_THROWS(digest_string<>{dynamic_hash, dynamic_base, _TestList[0].digest});
    }
    SECTION("`digest_string<H,B>` with valid H/B initialized with a std::string of the wrong size must fail") {
        auto string = std::string(_TestList[0].digest) + "ab";

        REQUIRE_THROWS( digest_string<sha1, base16>{ string } );
        REQUIRE_THROWS( digest_string<sha1>{ base16, string } );
        REQUIRE_THROWS( digest_string<dynamic_hash, base16>{ sha1, string } );
        REQUIRE_THROWS( digest_string<>{ sha1, base16, string } );
    }

    SECTION("`digest_string<H,B>` with valid H/B, initialized with a encoded_string<B> is allowed and `hash()==H` and `base()==B`") {
        auto a = encoded_string<base16>{ _TestList[0].digest };
        auto b = encoded_string<>{ base16, _TestList[0].digest };
        {
            auto s = digest_string<sha1, base16>{ a };
            REQUIRE(s.hash() == sha1);
            REQUIRE(s.base() == base16);
        }
        {
            auto s = digest_string<sha1, base16>{ b };
            REQUIRE(s.hash() == sha1);
            REQUIRE(s.base() == base16);
        }
        {
            auto s = digest_string<dynamic_hash, base16>{ sha1, a };
            REQUIRE(s.hash() == sha1);
            REQUIRE(s.base() == base16);
        }
        {
            auto s = digest_string<dynamic_hash, base16>{ sha1, b };
            REQUIRE(s.hash() == sha1);
            REQUIRE(s.base() == base16);
        }
    }
    SECTION("`digest_string<H,B>` with invalid/undef/mismatched H/B, initialized with a encoded_string<> must fail") {
        auto a = encoded_string<base16>{ _TestList[0].digest };
        auto b = encoded_string<>{ base16, _TestList[0].digest };

        REQUIRE_THROWS(digest_string<sha1, base16>{sha2_256, a});
        REQUIRE_THROWS(digest_string<sha1, base16>{sha2_256, b});

#ifdef SHOULD_NOT_COMPILE
        REQUIRE_THROWS(digest_string<sha1, base32>{ a });
        REQUIRE_THROWS(digest_string<sha1, base32>{ sha1, a });
        REQUIRE_THROWS(digest_string<sha1, base32>{ sha2_256, a });
#endif
        REQUIRE_THROWS(digest_string<sha1, base32>{ b });
        REQUIRE_THROWS(digest_string<sha1, base32>{ sha1, b});
        REQUIRE_THROWS(digest_string<sha1, base32>{ sha2_256, b});

#ifdef SHOULD_NOT_COMPILE
        REQUIRE_THROWS(digest_string<hash_t('$'), base16>{ a });
        REQUIRE_THROWS(digest_string<hash_t('$'), base16>{ b });

        REQUIRE_THROWS(digest_string<sha1, base_t('!')>{ a });
        REQUIRE_THROWS(digest_string<sha1, base_t('!')>{ b });

        REQUIRE_THROWS(digest_string<hash_t('$'), base_t('!')>{ a });
        REQUIRE_THROWS(digest_string<hash_t('$'), base_t('!')>{ b });
#endif
        REQUIRE_THROWS(digest_string<dynamic_hash, base16>{ hash_t('$'), a });
        REQUIRE_THROWS(digest_string<dynamic_hash, base16>{ hash_t('$'), b });

#ifdef SHOULD_NOT_COMPILE
        REQUIRE_THROWS(digest_string<dynamic_hash, base16>{ a });
        REQUIRE_THROWS(digest_string<dynamic_hash, base16>{ b });

        REQUIRE_THROWS(digest_string<>{ a });
        REQUIRE_THROWS(digest_string<>{ b });
#endif

        REQUIRE_THROWS(digest_string<dynamic_hash, base16>{dynamic_hash, a});
        REQUIRE_THROWS(digest_string<dynamic_hash, base16>{dynamic_hash, b});
        REQUIRE_THROWS(digest_string<>{dynamic_hash, a });
        REQUIRE_THROWS(digest_string<>{dynamic_hash, b });
    }
    SECTION("`digest_string<H,B>` with valid H/B initialized with a encoded_string<B> of the wrong size must fail") {
        auto a = encoded_string<base16>{ _TestList[0].digest } + "ab";
        auto b = encoded_string<>{ base16, _TestList[0].digest } + "ab";


        REQUIRE_THROWS(digest_string<sha1, base16>{ a });
        REQUIRE_THROWS(digest_string<sha1, base16>{ b });
        REQUIRE_THROWS(digest_string<sha1>{ base16, a });
        REQUIRE_THROWS(digest_string<sha1>{ base16, b });
        REQUIRE_THROWS(digest_string<dynamic_hash, base16>{ sha1, a });
        REQUIRE_THROWS(digest_string<dynamic_hash, base16>{ sha1, b });
        REQUIRE_THROWS(digest_string<>{ sha1, base16, a });
        REQUIRE_THROWS(digest_string<>{ sha1, base16, b });
    }

    SECTION("`digest_string<H1,B1>` can be initialized with `digest_string<H2,B2>` if compatible") {
        {
            auto a = digest_string<sha1,base16>{ _TestList[0].digest };
            REQUIRE(digest_string<sha1, base16>{ a } == a);
            REQUIRE(digest_string<sha1, dynamic_base>{ a } == a);
            REQUIRE(digest_string<dynamic_hash, base16>{ a } == a);
            REQUIRE(digest_string<dynamic_hash, dynamic_base>{ a } == a);
        }
        {
            auto a = digest_string<sha1, dynamic_base>{ base16, _TestList[0].digest };
            REQUIRE(digest_string<sha1, base16>{ a } == a);
            REQUIRE(digest_string<sha1, dynamic_base>{ a } == a);
            REQUIRE(digest_string<dynamic_hash, base16>{ a } == a);
            REQUIRE(digest_string<dynamic_hash, dynamic_base>{ a } == a);
        }
        {
            auto a = digest_string<dynamic_hash, base16>{ sha1, _TestList[0].digest };
            REQUIRE(digest_string<sha1, base16>{ a } == a);
            REQUIRE(digest_string<sha1, dynamic_base>{ a } == a);
            REQUIRE(digest_string<dynamic_hash, base16>{ a } == a);
            REQUIRE(digest_string<dynamic_hash, dynamic_base>{ a } == a);
        }
        {
            auto a = digest_string<dynamic_hash, dynamic_base>{ sha1, base16, _TestList[0].digest };
            REQUIRE(digest_string<sha1, base16>{ a } == a);
            REQUIRE(digest_string<sha1, dynamic_base>{ a } == a);
            REQUIRE(digest_string<dynamic_hash, base16>{ a } == a);
            REQUIRE(digest_string<dynamic_hash, dynamic_base>{ a } == a);
        }
    }
    SECTION("`digest_string<H1,B1>` initialized with `digest_string<H2,B2>` must fail if incompatible") {
        {
            auto a = digest_string<sha1, base16>{ _TestList[0].digest };

#ifdef SHOULD_NOT_COMPILE
            REQUIRE_THROWS(digest_string<sha2_256, base16>{ a } == a);
            REQUIRE_THROWS(digest_string<sha1, base32>{ a } == a);
#endif
        }
    }

    SECTION("b<H,B> = std::string is allowed if b is empty and a is not empty and size-compatible") {
        {
            auto a = _TestList[0].digest;
            {
                auto b = digest_string<sha1, base16>{};
                b = a;
                REQUIRE(b == a);
            }
            {
                auto b = digest_string<sha1, dynamic_base>{ base16 };
                b = a;
                REQUIRE(b == a);
            }
            {
                auto b = digest_string<dynamic_hash, base16>{ sha1 };
                b = a;
                REQUIRE(b == a);
            }
            {
                auto b = digest_string<dynamic_hash, dynamic_base>{ sha1, base16 };
                b = a;
                REQUIRE(b == a);
            }
        }
    }
    SECTION("b<H,B> = std::string must fail if b is not empty") {
        {
            auto a = _TestList[0].digest;
            {
                auto b = digest_string<sha1, base16>{ a };
                REQUIRE_THROWS(b = a);
            }
            {
                auto b = digest_string<sha1, dynamic_base>{ base16, a };
                REQUIRE_THROWS(b = a);
            }
            {
                auto b = digest_string<dynamic_hash, base16>{ sha1, a };
                REQUIRE_THROWS(b = a);
            }
            {
                auto b = digest_string<dynamic_hash, dynamic_base>{ sha1, base16, a };
                REQUIRE_THROWS(b = a);
            }
        }
    }
    SECTION("b<H,B> = std::string must fail if a is empty") {
        {
            auto a = "";
            {
                auto b = digest_string<sha1, base16>{ };
                REQUIRE_THROWS(b = a);
            }
            {
                auto b = digest_string<sha1, dynamic_base>{ base16 };
                REQUIRE_THROWS(b = a);
            }
            {
                auto b = digest_string<dynamic_hash, base16>{ sha1 };
                REQUIRE_THROWS(b = a);
            }
            {
                auto b = digest_string<dynamic_hash, dynamic_base>{ sha1, base16 };
                REQUIRE_THROWS(b = a);
            }
        }
    }
    SECTION("b<H,B> = std::string must fail if a has the wrong size") {
        {
            auto a = std::string(_TestList[0].digest) + "ab";
            {
                auto b = digest_string<sha1, base16>{};
                REQUIRE_THROWS(b = a);
            }
            {
                auto b = digest_string<sha1, dynamic_base>{ base16 };
                REQUIRE_THROWS(b = a);
            }
            {
                auto b = digest_string<dynamic_hash, base16>{ sha1 };
                REQUIRE_THROWS(b = a);
            }
            {
                auto b = digest_string<dynamic_hash, dynamic_base>{ sha1, base16 };
                REQUIRE_THROWS(b = a);
            }
        }
    }

    SECTION("b<H,B> = encoded_string<B> is allowed if b is empty and a is not empty and size-compatible") {
        {
            auto a = encoded_string<base16>(_TestList[0].digest);
            {
                auto b = digest_string<sha1, base16>{};
                b = a;
                REQUIRE(b == a);
            }
            {
                auto b = digest_string<sha1, dynamic_base>{ base16 };
                b = a;
                REQUIRE(b == a);
            }
            {
                auto b = digest_string<dynamic_hash, base16>{ sha1 };
                b = a;
                REQUIRE(b == a);
            }
            {
                auto b = digest_string<dynamic_hash, dynamic_base>{ sha1, base16 };
                b = a;
                REQUIRE(b == a);
            }
        }
    }
    SECTION("b<H,B> = encoded_string<B> must fail if b is not empty") {
        auto a = encoded_string<base16>(_TestList[0].digest);
        {
            auto b = digest_string<sha1, base16>{ a };
            REQUIRE_THROWS(b = a);
        }
        {
            auto b = digest_string<sha1, dynamic_base>{ a };
            REQUIRE_THROWS(b = a);
        }
        {
            auto b = digest_string<dynamic_hash, base16>{ sha1, a };
            REQUIRE_THROWS(b = a);
        }
        {
            auto b = digest_string<dynamic_hash, dynamic_base>{ sha1, a };
            REQUIRE_THROWS(b = a);
        }
    }
    SECTION("b<H,B> = encoded_string<B> must fail if a is empty") {
        {
            auto a = encoded_string<base16>();
            {
                auto b = digest_string<sha1, base16>{};
                REQUIRE_THROWS(b = a);
            }
            {
                auto b = digest_string<sha1, dynamic_base>{ base16 };
                REQUIRE_THROWS(b = a);
            }
            {
                auto b = digest_string<dynamic_hash, base16>{ sha1 };
                REQUIRE_THROWS(b = a);
            }
            {
                auto b = digest_string<dynamic_hash, dynamic_base>{ sha1, base16 };
                REQUIRE_THROWS(b = a);
            }
        }
    }
    SECTION("b<H,B> = encoded_string<B> must fail if a has the wrong size") {
        {
            auto a = encoded_string<base16>(_TestList[0].digest) + "ab";
            {
                auto b = digest_string<sha1, base16>{};
                REQUIRE_THROWS(b = a);
            }
            {
                auto b = digest_string<sha1, dynamic_base>{ base16 };
                REQUIRE_THROWS(b = a);
            }
            {
                auto b = digest_string<dynamic_hash, base16>{ sha1 };
                REQUIRE_THROWS(b = a);
            }
            {
                auto b = digest_string<dynamic_hash, dynamic_base>{ sha1, base16 };
                REQUIRE_THROWS(b = a);
            }
        }
    }
    SECTION("b<H,B> = encoded_string<C> must fail") {
        {
            auto a = encoded_string<base16>(_TestList[0].digest);
#ifdef SHOULD_NOT_COMPILE
            {
                auto b = digest_string<sha1, base64>{};
                REQUIRE_THROWS(b = a);
            }
#endif
            {
                auto b = digest_string<sha1, dynamic_base>{ base64 };
                REQUIRE_THROWS(b = a);
            }
#ifdef SHOULD_NOT_COMPILE
            {
                auto b = digest_string<dynamic_hash, base64>{ sha1 };
                REQUIRE_THROWS(b = a);
            }
#endif
            {
                auto b = digest_string<dynamic_hash, dynamic_base>{ sha1, base64 };
                REQUIRE_THROWS(b = a);
            }
        }
    }

    SECTION("b<H,B> = a<H,B> is allowed if b is empty and a is not empty and compatible") {
        {
            auto a = digest_string<sha1, base16>{ _TestList[0].digest };
            {
                auto b = digest_string<sha1, base16>{};
                b = a;
                REQUIRE(b == a);
            }
            {
                auto b = digest_string<sha1, dynamic_base>{ base16 };
                b = a;
                REQUIRE(b == a);
            }
            {
                auto b = digest_string<dynamic_hash, base16>{ sha1 };
                b = a;
                REQUIRE(b == a);
            }
            {
                auto b = digest_string<dynamic_hash, dynamic_base>{ sha1, base16 };
                b = a;
                REQUIRE(b == a);
            }
        }
        {
            auto a = digest_string<sha1, dynamic_base>{ base16, _TestList[0].digest };
            {
                auto b = digest_string<sha1, base16>{};
                b = a;
                REQUIRE(b == a);
            }
            {
                auto b = digest_string<sha1, dynamic_base>{ base16 };
                b = a;
                REQUIRE(b == a);
            }
            {
                auto b = digest_string<dynamic_hash, base16>{ sha1 };
                b = a;
                REQUIRE(b == a);
            }
            {
                auto b = digest_string<dynamic_hash, dynamic_base>{ sha1, base16 };
                b = a;
                REQUIRE(b == a);
            }
        }
        {
            auto a = digest_string<dynamic_hash, base16>{ sha1, _TestList[0].digest };
            {
                auto b = digest_string<sha1, base16>{};
                b = a;
                REQUIRE(b == a);
            }
            {
                auto b = digest_string<sha1, dynamic_base>{ base16 };
                b = a;
                REQUIRE(b == a);
            }
            {
                auto b = digest_string<dynamic_hash, base16>{ sha1 };
                b = a;
                REQUIRE(b == a);
            }
            {
                auto b = digest_string<dynamic_hash, dynamic_base>{ sha1, base16 };
                b = a;
                REQUIRE(b == a);
            }
        }
        {
            auto a = digest_string<dynamic_hash, dynamic_base>{ sha1, base16, _TestList[0].digest };
            {
                auto b = digest_string<sha1, base16>{};
                b = a;
                REQUIRE(b == a);
            }
            {
                auto b = digest_string<sha1, dynamic_base>{ base16 };
                b = a;
                REQUIRE(b == a);
            }
            {
                auto b = digest_string<dynamic_hash, base16>{ sha1 };
                b = a;
                REQUIRE(b == a);
            }
            {
                auto b = digest_string<dynamic_hash, dynamic_base>{ sha1, base16 };
                b = a;
                REQUIRE(b == a);
            }
        }
    }
    SECTION("b<H,B> = a<H,B> must fail if b is not empty") {
        auto a = digest_string<sha1, base16>{ _TestList[0].digest };
        {
            auto b = digest_string<sha1, base16>{ a };
            REQUIRE_THROWS(b = a);
        }
        {
            auto b = digest_string<sha1, dynamic_base>{ a };
            REQUIRE_THROWS(b = a);
        }
        {
            auto b = digest_string<dynamic_hash, base16>{ sha1, a };
            REQUIRE_THROWS(b = a);
        }
        {
            auto b = digest_string<dynamic_hash, dynamic_base>{ sha1, a };
            REQUIRE_THROWS(b = a);
        }
    }
    SECTION("b<H,B> = a<H,B> must fail if a is empty") {
        auto a = digest_string<sha1, base16>{ };
        {
            auto b = digest_string<sha1, base16>{};
            REQUIRE_THROWS(b = a);
        }
        {
            auto b = digest_string<sha1, dynamic_base>{ base16 };
            REQUIRE_THROWS(b = a);
        }
        {
            auto b = digest_string<dynamic_hash, base16>{ sha1 };
            REQUIRE_THROWS(b = a);
        }
        {
            auto b = digest_string<dynamic_hash, dynamic_base>{ sha1, base16 };
            REQUIRE_THROWS(b = a);
        }
    }
    SECTION("b<H1,B1> = a<H2,B2> must fail") {
        {
            auto a = digest_string<sha1, base16>{_TestList[0].digest};
#ifdef SHOULD_NOT_COMPILE
            {
                auto b = digest_string<sha1, base64>{};
                REQUIRE_THROWS(b = a);
            }
#endif
            {
                auto b = digest_string<sha1, dynamic_base>{ base64 };
                REQUIRE_THROWS(b = a);
            }
#ifdef SHOULD_NOT_COMPILE
            {
                auto b = digest_string<dynamic_hash, base64>{ sha1 };
                REQUIRE_THROWS(b = a);
            }
#endif
            {
                auto b = digest_string<dynamic_hash, dynamic_base>{ sha1, base64 };
                REQUIRE_THROWS(b = a);
            }
        }
    }
}


TEST_CASE("`digest_buffer<>` class")
{
    SECTION("empty `digest_buffer<A>` with valid A is allowed and `hash()` returns A") {
        REQUIRE(digest_buffer<sha1>{}.hash() == sha1);
        REQUIRE(digest_buffer<>{sha1}.hash() == sha1);
    }

    SECTION("empty `digest_buffer<A>` with invalid/undef/mismatched A must fail") {
#ifdef SHOULD_NOT_COMPILE
        REQUIRE_THROWS(digest_buffer<hash_t('$')>{});
#endif
        REQUIRE_THROWS(digest_buffer<>{hash_t('$')});
        REQUIRE_THROWS(digest_buffer<>{dynamic_hash});
        REQUIRE_THROWS(digest_buffer<>{});
        REQUIRE_THROWS(digest_buffer<sha1>{sha2_256});
    }

    SECTION("`digest_buffer<A>` with valid A initialized with a buffer_t is allowed and `hash()` returns A") {
        auto buffer = decode(base16, _TestList[0].digest);
        REQUIRE(digest_buffer<_TestList[0].code>{buffer}.hash() == _TestList[0].code);
        REQUIRE(digest_buffer<>(_TestList[0].code, buffer).hash() == _TestList[0].code);
    }

    SECTION("`digest_buffer<A>` with invalid/undef/mismatched A initialized with a buffer_t must fail") {
        auto buffer = decode(base16, _TestList[0].digest);
#ifdef SHOULD_NOT_COMPILE
        REQUIRE_THROWS(digest_buffer<hash_t('$')>{buffer});
        REQUIRE_THROWS(digest_buffer<>{buffer});
#endif
        REQUIRE_THROWS(digest_buffer<>{hash_t('$'), buffer});
        REQUIRE_THROWS(digest_buffer<>{dynamic_hash, buffer});
        REQUIRE_THROWS(digest_buffer<sha1>{sha2_256, buffer});
    }

    SECTION("`digest_buffer<A>` with valid A initialized with a buffer_t of the wrong size must fail") {
        auto buffer = decode(base16, _TestList[0].digest);
        buffer.push_back('?');

        REQUIRE_THROWS(digest_buffer<_TestList[0].code>{buffer});
        REQUIRE_THROWS(digest_buffer<>(_TestList[0].code, buffer));
    }


    SECTION("`digest_buffer<A>` can be initialized with `digest_buffer<B>` if A is dynamic_hash or A.hash==B.hash") {
        auto buffer = decode(base16, _TestList[0].digest);
        {
            auto a = digest_buffer<sha1>{ buffer };
            auto b = digest_buffer<sha1>{ a };
            REQUIRE(a == b);
        }
        {
            auto a = digest_buffer<>{ sha1, buffer };
            auto b = digest_buffer<sha1>{ a };
            REQUIRE(a == b);
        }
        {
            auto a = digest_buffer<sha1>{ buffer };
            auto b = digest_buffer<>{ a };
            REQUIRE(a == b);
        }
        {
            auto a = digest_buffer<>{ sha1, buffer };
            auto b = digest_buffer<>{ a };
            REQUIRE(a == b);
        }
    }

    SECTION("`digest_buffer<A>` must throw if initialized with `digest_buffer<B>` and A!=B") {
        auto buffer = decode(base16, _TestList[0].digest);
        {
            auto a = digest_buffer<sha1>{ buffer };
            REQUIRE_THROWS(digest_buffer<sha2_256>{ a });
        }
        {
            auto a = digest_buffer<>{ sha1, buffer };
            REQUIRE_THROWS(digest_buffer<sha2_256>{ a });
        }
    }

    SECTION("a<A> = b<B> is allowed if a is empty and b is not empty and A==B") {
        auto buffer = decode(base16, _TestList[0].digest);
        {
            auto a = digest_buffer<sha1>{};
            auto b = digest_buffer<sha1>{ buffer };
            a = b;
            REQUIRE(a == b);
        }
        {
            auto a = digest_buffer<sha1>{};
            auto b = digest_buffer<>{ sha1, buffer };
            a = b;
            REQUIRE(a == b);
        }
        {
            auto a = digest_buffer<>{ sha1 };
            auto b = digest_buffer<sha1>{ buffer };
            a = b;
            REQUIRE(a == b);
        }
        {
            auto a = digest_buffer<>{ sha1 };
            auto b = digest_buffer<>{ sha1, buffer };
            a = b;
            REQUIRE(a == b);
        }
    }

    SECTION("a<A> = b<B> must fail if a is not empty") {
        auto buffer = decode(base16, _TestList[0].digest);
        {
            auto a = digest_buffer<sha1>{ buffer };
            auto b = digest_buffer<sha1>{ buffer };
            REQUIRE_THROWS(a = b);
        }
        {
            auto a = digest_buffer<>{ sha1, buffer };
            auto b = digest_buffer<sha1>{ buffer };
            REQUIRE_THROWS(a = b);
        }
        {
            auto a = digest_buffer<sha1>{ buffer };
            auto b = digest_buffer<>{ sha1, buffer };
            REQUIRE_THROWS(a = b);
        }
        {
            auto a = digest_buffer<>{ sha1, buffer };
            auto b = digest_buffer<>{ sha1, buffer };
            REQUIRE_THROWS(a = b);
        }
    }
    SECTION("a<A> = b<B> must fail if b is empty") {
        {
            auto a = digest_buffer<sha1>{};
            auto b = digest_buffer<sha1>{};
            REQUIRE_THROWS(a = b);
        }
        {
            auto a = digest_buffer<>{ sha1 };
            auto b = digest_buffer<sha1>{};
            REQUIRE_THROWS(a = b);
        }
        {
            auto a = digest_buffer<sha1>{};
            auto b = digest_buffer<>{ sha1 };
            REQUIRE_THROWS(a = b);
        }
        {
            auto a = digest_buffer<>{ sha1 };
            auto b = digest_buffer<>{ sha1 };
            REQUIRE_THROWS(a = b);
        }
    }
    SECTION("a<A> = b<B> must fail if A!=B") {
        auto buffer = decode(base16, _TestList[1].digest);
#ifdef SHOULD_NOT_COMPILE
        {
            auto a = digest_buffer<sha1>{};
            auto b = digest_buffer<sha2_256>{ buffer };
            REQUIRE_THROWS(a = b);
        }
#endif
        {
            auto a = digest_buffer<sha1>{};
            auto b = digest_buffer<>{ sha2_256, buffer };
            REQUIRE_THROWS(a = b);
        }
        {
            auto a = digest_buffer<>{ sha1 };
            auto b = digest_buffer<sha2_256>{ buffer };
            REQUIRE_THROWS(a = b);
        }
        {
            auto a = digest_buffer<>{ sha1 };
            auto b = digest_buffer<>{ sha2_256, buffer };
            REQUIRE_THROWS(a = b);
        }
    }
}

/*

TEST_CASE("encode")
{
    SECTION("encode<A>(...) with invalid/undef A must fail ") {
#ifdef SHOULD_NOT_COMPILE
        REQUIRE_THROWS(encode<hash_t('!')>(ascii));
        REQUIRE_THROWS(encode<hash_t('!')>(data));

        REQUIRE_THROWS(encode<dynamic_hash>(ascii));
        REQUIRE_THROWS(encode<dynamic_hash>(data));
#endif
        REQUIRE_THROWS(encode(hash_t('!'), ascii));
        REQUIRE_THROWS(encode(hash_t('!'), data));

        REQUIRE_THROWS(encode(dynamic_hash, ascii));
        REQUIRE_THROWS(encode(dynamic_hash, data));
    }

    SECTION("encode<A>(string) should throw on empty input") {
        REQUIRE_THROWS(encode<sha1>(buffer_t{}));
        REQUIRE_THROWS(encode<sha1>(""));

        REQUIRE_THROWS(encode(sha1, buffer_t{}));
        REQUIRE_THROWS(encode(sha1, ""));
    }

    SECTION("Encode a zstring") test_encode_zstring(false);
    SECTION("Encode a ascii string") test_encode_ascii(false);
    SECTION("Encode a utf8 string") test_encode_utf8(false);
    SECTION("Encode a data buffer") test_encode_data(false);
}

TEST_CASE("multiencode")
{
    SECTION("multiencode<A>(...) with invalid/undef A must fail ") {
#ifdef SHOULD_NOT_COMPILE
        REQUIRE_THROWS(multiencode<hash_t('!')>(ascii));
        REQUIRE_THROWS(multiencode<hash_t('!')>(data));

        REQUIRE_THROWS(multiencode<dynamic_hash>(ascii));
        REQUIRE_THROWS(multiencode<dynamic_hash>(data));
#endif
        REQUIRE_THROWS(multiencode(hash_t('!'), ascii));
        REQUIRE_THROWS(multiencode(hash_t('!'), data));

        REQUIRE_THROWS(multiencode(dynamic_hash, ascii));
        REQUIRE_THROWS(multiencode(dynamic_hash, data));
    }

    SECTION("encode<A>(string) should throw on empty input") {
        REQUIRE_THROWS(multiencode<sha1>(buffer_t{}));
        REQUIRE_THROWS(multiencode<sha1>(""));

        REQUIRE_THROWS(multiencode(sha1, buffer_t{}));
        REQUIRE_THROWS(multiencode(sha1, ""));
    }

    SECTION("Encode a zstring") test_encode_zstring(true);
    SECTION("Encode a ascii string") test_encode_ascii(true);
    SECTION("Encode a utf8 string") test_encode_utf8(true);
    SECTION("Encode a data buffer") test_encode_data(true);
}
*/