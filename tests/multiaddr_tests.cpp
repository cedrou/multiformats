#include <catch.hpp> // for AssertionHandler, StringRef, CHECK, TEST_...

#define GSL_THROW_ON_CONTRACT_VIOLATION
#include <multiformats/multiaddr.h>
#include <multiformats/multibase.h>

#include <iostream>
#include <string>
using namespace std::string_literals; // enables s-suffix for std::string literals 

using namespace multiformats;

//#define SHOULD_NOT_COMPILE

TEST_CASE("Construction")
{
    //auto addr = multiaddr{ "/ip4/127.0.0.1/udp/1234" };
}

TEST_CASE("addr_buffer")
{
    SECTION("Empty addr_buffer<A> must fail") {
#ifdef SHOULD_NOT_COMPILE
        REQUIRE_THROWS(addr_buffer<ip4>{});
        REQUIRE_THROWS(addr_buffer<>{ ip4 });
#endif
    }

    SECTION("addr_buffer<A> initialized with buffer_t of valid size is allowed and addr()==A") {
        auto a = buffer_t{ 0x7f, 0x00, 0x00, 0x01 };
        REQUIRE(addr_buffer<ip4>{a}.addr() == ip4);
        REQUIRE(addr_buffer<>{ ip4, a }.addr() == ip4);
    }
    SECTION("addr_buffer<A> initialized with buffer_t of wrong size must fail") {
        auto a = buffer_t{ 0x7f, 0x00, 0x00 };
        REQUIRE_THROWS(addr_buffer<ip4>{a});
        REQUIRE_THROWS(addr_buffer<>{ ip4, a });
    }

    SECTION("addr_buffer<A> initialized with a valid string is allowed and addr()==A") {
        auto a = "127.0.0.1";
        REQUIRE(addr_buffer<ip4>{ a }.addr() == ip4);
        REQUIRE(addr_buffer<>{ ip4, a }.addr() == ip4);
    }
    SECTION("addr_buffer<A> initialized with an invalid string must fail") {
        auto a = "1234.2345.3456.4567";
        REQUIRE_THROWS(addr_buffer<ip4>{a});
        REQUIRE_THROWS(addr_buffer<>{ ip4, a });
    }

    SECTION("addr_buffer<A> initialized with addr_buffer<A> is allowed and addr()==A") {
        {
            auto a = addr_buffer<ip4>{ "127.0.0.1" };
            REQUIRE(addr_buffer<ip4>{ a }.addr() == ip4);
            REQUIRE(addr_buffer<>{ a }.addr() == ip4);
        }
        {
            auto a = addr_buffer<>{ ip4, "127.0.0.1" };
            REQUIRE(addr_buffer<ip4>{ a }.addr() == ip4);
            REQUIRE(addr_buffer<>{ a }.addr() == ip4);
        }
    }
    SECTION("addr_buffer<A1> initialized with addr_buffer<A2> must fail") {
        {
#ifdef SHOULD_NOT_COMPILE
            auto a = addr_buffer<udp>{ "1234" };
            REQUIRE_THROWS(addr_buffer<ip4>{ a });
#endif
        }
        {
            auto a = addr_buffer<>{ udp, "1234" };
            REQUIRE_THROWS(addr_buffer<ip4>{ a });
        }
    }

}

TEST_CASE("multiaddr")
{
    auto from_string = "/ip4/127.0.0.1/udp/1234";
    auto from_buffer = buffer_t{ 0x04, 0x7f, 0x00, 0x00, 0x01, 0x11, 0x04, 0xd2 };
    auto from_addrip4 = addr_buffer<ip4>{ "127.0.0.1" };
    auto from_addrudp = addr_buffer<udp>{ "1234" };


    // construction from string
    auto ma_fromstring = multiaddr{ from_string };
    REQUIRE(ma_fromstring.str() == from_string);

    // construction from buffer
    auto ma_frombuffer = multiaddr{ from_buffer };
    REQUIRE(ma_frombuffer.str() == from_string);

    // construction from addr_buffer
    auto ma_fromip4 = multiaddr{ from_addrip4 };
    auto ma_fromudp = multiaddr{ from_addrudp };

    // construction from multiaddr
    auto ma_fromma = multiaddr(ma_fromstring);
    REQUIRE(ma_fromma.str() == from_string);

    // conversion to string (tested above)
    // conversion to buffer
    REQUIRE(ma_fromstring.data() == from_buffer);

    // encapsulate another multiaddr
    auto ma_enc = ma_fromip4.encapsulate(ma_fromudp);
    REQUIRE(ma_enc.str() == from_string);

    // decapsulate
    auto ma_dec1 = ma_enc.decapsulate(from_addrudp);
    REQUIRE(ma_dec1.str() == ma_fromip4.str());
    auto ma_dec2 = ma_dec1.decapsulate(from_addrip4);
    REQUIRE(ma_dec2.empty());

    auto ma_dec3 = ma_enc.decapsulate(from_addrip4);
    REQUIRE(ma_dec3.empty());
}

TEST_CASE("multiaddr manipulation with basic protocols")
{
    auto udpAddrStr = "/ip4/127.0.0.1/udp/1234";
    auto udpAddrBuf = decode("047f0000011104d2"_16);
    auto udpAddr = multiaddr(udpAddrStr);

    REQUIRE(udpAddr.str() == udpAddrStr);
    REQUIRE(udpAddr.data() == udpAddrBuf);
    REQUIRE(udpAddr.protocols().size() == 2);
    REQUIRE(udpAddr.protocols()[0].addr() == ip4);
    REQUIRE(udpAddr.protocols()[1].addr() == udp);

    auto udpAddrBuf2 = udpAddr.encapsulate("/udp/5678");

    REQUIRE(udpAddrBuf2.str() == "/ip4/127.0.0.1/udp/1234/udp/5678");
    REQUIRE(udpAddrBuf2.decapsulate(udp).str() == udpAddrStr);
    REQUIRE(udpAddrBuf2.decapsulate(ip4).str() == "/");
    REQUIRE(multiaddr().encapsulate(udpAddr).str() == udpAddr.str());
}

TEST_CASE("multiaddr manipulation with ipfs")
{
    auto ipfsAddr = multiaddr("/ipfs/QmcgpsyWgH8Y8ajJz1Cu72KnS5uo2Aa2LpzU7kinSupNKC");
    auto ip6Addr  = multiaddr("/ip6/2001:8a0:7ac5:4201:3ac9:86ff:fe31:7095");
    auto tcpAddr  = multiaddr("/tcp/8000");
    auto webAddr  = multiaddr("/ws");

    REQUIRE(multiaddr().encapsulate(ip6Addr).encapsulate(tcpAddr).encapsulate(webAddr).encapsulate(ipfsAddr).str()
        == ip6Addr.str() + tcpAddr.str() + webAddr.str() + ipfsAddr.str()
    );
        
    REQUIRE(multiaddr().encapsulate(ip6Addr).encapsulate(tcpAddr).encapsulate(webAddr).encapsulate(ipfsAddr).decapsulate(ipfsAddr).str()
        == ip6Addr.str() + tcpAddr.str() + webAddr.str()
    );

    REQUIRE(multiaddr().encapsulate(ip6Addr).encapsulate(tcpAddr).encapsulate(ipfsAddr).encapsulate(webAddr).decapsulate(webAddr).str()
        == ip6Addr.str() + tcpAddr.str() + ipfsAddr.str()
    );

}

TEST_CASE("codecs")
{
    SECTION("ipv6") {
        auto ip6_1 = addr_buffer<ip6>{ "2001:0db8:85a3:0000:0000:8a2e:0370:7334" };
        auto ip6_2 = addr_buffer<ip6>{ "2001:db8:85a3:0:0:8a2e:370:7334" };
        auto ip6_3 = addr_buffer<ip6>{ "2001:db8:85a3::8a2e:370:7334" };
        REQUIRE(ip6_1 == ip6_2);
        REQUIRE(ip6_1 == ip6_3);

        auto loopback = addr_buffer<ip6>{ "::1" };
        REQUIRE(loopback.data() == decode("00000000000000000000000000000001"_16));

        auto unspecified = addr_buffer<ip6>{ "::" };
        REQUIRE(unspecified.data() == decode("00000000000000000000000000000000"_16));
    }
}

TEST_CASE("Construction fails")
{
    auto cases = {
        "/ip4",
        "/ip4/::1",
        "/ip4/fdpsofodsajfdoisa",
        "/ip6",
        "/udp",
        "/tcp",
        "/sctp",
        "/udp/65536",
        "/tcp/65536",
        "/quic/65536",
        "/onion/9imaq4ygg2iegci7:80",
        "/onion/aaimaq4ygg2iegci7:80",
        "/onion/timaq4ygg2iegci7:0",
        "/onion/timaq4ygg2iegci7:-1",
        "/onion/timaq4ygg2iegci7",
        "/onion/timaq4ygg2iegci@:666",
        "/udp/1234/sctp",
        "/udp/1234/udt/1234",
        "/udp/1234/utp/1234",
        "/ip4/127.0.0.1/udp/jfodsajfidosajfoidsa",
        "/ip4/127.0.0.1/udp",
        "/ip4/127.0.0.1/tcp/jfodsajfidosajfoidsa",
        "/ip4/127.0.0.1/tcp",
        "/ip4/127.0.0.1/quic/1234",
        "/ip4/127.0.0.1/ipfs",
        "/ip4/127.0.0.1/ipfs/tcp",
        "/unix",
        "/ip4/1.2.3.4/tcp/80/unix",
    };

    for (auto s : cases)
        SECTION(s) CHECK_THROWS(multiaddr(s));

}

TEST_CASE("Construction succeeds")
{
    auto cases = {
        "/ip4/1.2.3.4",
        "/ip4/0.0.0.0",
        "/ip6/::1",
        "/ip6/2601:9:4f81:9700:803e:ca65:66e8:c21",
        "/ip6/2601:9:4f81:9700:803e:ca65:66e8:c21/udp/1234/quic",
        "/onion/timaq4ygg2iegci7:1234",
        "/onion/timaq4ygg2iegci7:80/http",
        "/udp/0",
        "/tcp/0",
        "/sctp/0",
        "/udp/1234",
        "/tcp/1234",
        "/sctp/1234",
        "/udp/65535",
        "/tcp/65535",
        "/ipfs/QmcgpsyWgH8Y8ajJz1Cu72KnS5uo2Aa2LpzU7kinSupNKC",
        "/udp/1234/sctp/1234",
        "/udp/1234/udt",
        "/udp/1234/utp",
        "/tcp/1234/http",
        "/tcp/1234/https",
        "/ipfs/QmcgpsyWgH8Y8ajJz1Cu72KnS5uo2Aa2LpzU7kinSupNKC/tcp/1234",
        "/ip4/127.0.0.1/udp/1234",
        "/ip4/127.0.0.1/udp/0",
        "/ip4/127.0.0.1/tcp/1234",
        "/ip4/127.0.0.1/tcp/1234/",
        "/ip4/127.0.0.1/udp/1234/quic",
        "/ip4/127.0.0.1/ipfs/QmcgpsyWgH8Y8ajJz1Cu72KnS5uo2Aa2LpzU7kinSupNKC",
        "/ip4/127.0.0.1/ipfs/QmcgpsyWgH8Y8ajJz1Cu72KnS5uo2Aa2LpzU7kinSupNKC/tcp/1234",
        "/unix/a/b/c/d/e",
        "/unix/stdio",
        "/ip4/1.2.3.4/tcp/80/unix/a/b/c/d/e/f",
        "/ip4/127.0.0.1/ipfs/QmcgpsyWgH8Y8ajJz1Cu72KnS5uo2Aa2LpzU7kinSupNKC/tcp/1234/unix/stdio",
    };

    for (auto s : cases)
        SECTION(s) CHECK_NOTHROW(multiaddr(s));

}