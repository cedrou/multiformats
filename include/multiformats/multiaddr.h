#pragma once

#include "common.h"
#include "uvarint.h"



namespace multiformats {

    enum addr_t {
        dynamic_addr,
        ip4,
        tcp,
        udp,
        dccp,
        ip6,
        dns,
        dns4,
        dns6,
        sctp,
        udt,
        utp,
        unix,
        p2p,
        ipfs,
        onion,
        quic,
        http,
        https,
        ws,
        wss,
        p2p_websocket_star,
        p2p_webrtc_star,
        p2p_webrtc_direct,
        p2p_circuit
    };
namespace details {

    typedef stringview_t(*Serializer)(stringview_t, buffer_t&);
    typedef bufferview_t(*Deserializer)(bufferview_t, std::string&);
    typedef bufferview_t(*Reader)(bufferview_t, buffer_t&);

    inline stringview_t serialize_noimpl(stringview_t /*src*/, buffer_t& /*dst*/) { throw std::logic_error("Function not yet implemented"); }
    inline bufferview_t deserialize_noimpl(bufferview_t /*src*/, std::string& /*dst*/) { throw std::logic_error("Function not yet implemented"); }
    inline bufferview_t read_noimpl(bufferview_t /*src*/, buffer_t& /*dst*/) { throw std::logic_error("Function not yet implemented"); }

    inline stringview_t serialize_noval(stringview_t src, buffer_t& /*dst*/) { return src; }
    inline bufferview_t deserialize_noval(bufferview_t src, std::string& /*dst*/) { return src; }
    inline bufferview_t read_noval(bufferview_t src, buffer_t& /*dst*/) { return src; }

    stringview_t serialize_ipv4(stringview_t src, buffer_t& dst);
    bufferview_t deserialize_ipv4(bufferview_t src, std::string& dst);
    bufferview_t read_ipv4(bufferview_t src, buffer_t& dst);

    stringview_t serialize_ipv6(stringview_t src, buffer_t& dst);
    bufferview_t deserialize_ipv6(bufferview_t src, std::string& dst);
    bufferview_t read_ipv6(bufferview_t src, buffer_t& dst);

    stringview_t serialize_dns(stringview_t src, buffer_t& dst);
    bufferview_t deserialize_dns(bufferview_t src, std::string& dst);
    bufferview_t read_dns(bufferview_t src, buffer_t& dst);

    stringview_t serialize_ipfs(stringview_t src, buffer_t& dst);
    bufferview_t deserialize_ipfs(bufferview_t src, std::string& dst);
    bufferview_t read_ipfs(bufferview_t src, buffer_t& dst);

    stringview_t serialize_onion(stringview_t src, buffer_t& dst);
    bufferview_t deserialize_onion(bufferview_t src, std::string& dst);
    bufferview_t read_onion(bufferview_t src, buffer_t& dst);

    stringview_t serialize_unix(stringview_t src, buffer_t& dst);
    bufferview_t deserialize_unix(bufferview_t src, std::string& dst);
    bufferview_t read_unix(bufferview_t src, buffer_t& dst);

    stringview_t serialize_port(stringview_t src, buffer_t& dst);
    bufferview_t deserialize_port(bufferview_t src, std::string& dst);
    bufferview_t read_port(bufferview_t src, buffer_t& dst);


    struct addrimpl {
        addr_t       key;
        const char*  name;
        uint32_t     code;
        int32_t      len;
        Serializer   serialize;
        Deserializer deserialize;
        Reader       read;
    };

    // defined protocols from https://github.com/multiformats/multiaddr/blob/master/protocols.csv
    // (commit f067654 on Nov 28 2017)
    constexpr addrimpl _AddrTable[] = {
        { dynamic_addr,         "dynamic_addr",           0, -1, serialize_noimpl, deserialize_noimpl, read_noimpl },
        { ip4,                  "ip4",                    4,  4, serialize_ipv4  , deserialize_ipv4  , read_ipv4   },
        { tcp,                  "tcp",                    6,  2, serialize_port  , deserialize_port  , read_port   },
        { udp,                  "udp",                   17,  2, serialize_port  , deserialize_port  , read_port   },
        { dccp,                 "dccp",                  33,  2, serialize_port  , deserialize_port  , read_port   },
        { ip6,                  "ip6",                   41, 16, serialize_ipv6  , deserialize_ipv6  , read_ipv6   },
        { dns,                  "dnsaddr",               53, -1, serialize_dns   , deserialize_dns   , read_dns    },
        { dns4,                 "dns4",                  54, -1, serialize_dns   , deserialize_dns   , read_dns    },
        { dns6,                 "dns6",                  55, -1, serialize_dns   , deserialize_dns   , read_dns    },
        { sctp,                 "sctp",                 132,  2, serialize_port  , deserialize_port  , read_port   },
        { udt,                  "udt",                  301,  0, serialize_noval , deserialize_noval , read_noval  },
        { utp,                  "utp",                  302,  0, serialize_noval , deserialize_noval , read_noval  },
        { unix,                 "unix",                 400, -1, serialize_unix  , deserialize_unix  , read_unix   },
        { p2p,                  "p2p",                  420, -1, serialize_noimpl, deserialize_noimpl, read_noimpl },
        { ipfs,                 "ipfs",                 421, -1, serialize_ipfs  , deserialize_ipfs  , read_ipfs   },
        { onion,                "onion",                444, 12, serialize_onion , deserialize_onion , read_onion  },
        { quic,                 "quic",                 460,  0, serialize_noval , deserialize_noval , read_noval  },
        { http,                 "http",                 480,  0, serialize_noval , deserialize_noval , read_noval  },
        { https,                "https",                443,  0, serialize_noval , deserialize_noval , read_noval  },
        { ws,                   "ws",                   477,  0, serialize_noval , deserialize_noval , read_noval  },
        { wss,                  "wss",                  478,  0, serialize_noval , deserialize_noval , read_noval  },
        { p2p_websocket_star,   "p2p-websocket-star",   479,  0, serialize_noval , deserialize_noval , read_noval  },
        { p2p_webrtc_star,      "p2p-webrtc-star",      275,  0, serialize_noval , deserialize_noval , read_noval  },
        { p2p_webrtc_direct,    "p2p-webrtc-direct",    276,  0, serialize_noval , deserialize_noval , read_noval  },
        { p2p_circuit,          "p2p-circuit",          290,  0, serialize_noval , deserialize_noval , read_noval  }
    };

    constexpr int find_addrimpl_by_key(addr_t key) {
        for (auto i = 0; i < _countof(_AddrTable); i++)
            if (_AddrTable[i].key == key) return i;
        return 0;
    }
    
    inline int find_addrimpl_by_name(stringview_t name) {
        auto s = to_string(name);
        for (auto i = 0; i < _countof(_AddrTable); i++)
            if (_AddrTable[i].name == s) return i;
        return 0;
    }
    inline int find_addrimpl_by_code(uint32_t code) {
        for (auto i = 0; i < _countof(_AddrTable); i++)
            if (_AddrTable[i].code == code) return i;
        return 0;
    }



    template <addr_t _Addr = dynamic_addr, int _Index = find_addrimpl_by_key(_Addr)>
    class addr_type
    {
    public:
        static_assert(_Index > 0, "The base_t is not implemented");

        constexpr addr_type(addr_t key) { Expects(key == _Addr); }

        constexpr addr_t          key()         const { return _AddrTable[_Index].key; }
        constexpr const char*     name()        const { return _AddrTable[_Index].name; }
        constexpr uint32_t        code()        const { return _AddrTable[_Index].code; }
        constexpr int32_t         len()         const { return _AddrTable[_Index].len; }
        constexpr Serializer      serialize()   const { return _AddrTable[_Index].serialize; }
        constexpr Deserializer    deserialize() const { return _AddrTable[_Index].deserialize; }
        constexpr Reader          read()        const { return _AddrTable[_Index].read; }
        constexpr int             index()       const { return _Index; }
    };
    template <>
    class addr_type<dynamic_addr>
    {
    public:
        explicit constexpr addr_type(addr_t key) : _index(find_addrimpl_by_key(key)) { Expects(_index > 0); }

        constexpr addr_t          key()         const { return _AddrTable[_index].key; }
        constexpr const char*     name()        const { return _AddrTable[_index].name; }
        constexpr uint32_t        code()        const { return _AddrTable[_index].code; }
        constexpr int32_t         len()         const { return _AddrTable[_index].len; }
        constexpr Serializer      serialize()   const { return _AddrTable[_index].serialize; }
        constexpr Deserializer    deserialize() const { return _AddrTable[_index].deserialize; }
        constexpr Reader          read()        const { return _AddrTable[_index].read; }
        constexpr int             index()       const { return _index; }
    private:
        const int _index;
    };


}

    template <addr_t _Addr = dynamic_addr>
    class addr_buffer
    {
    public:
        // Construct by copying _Right
        //    - from bufferview_t
        addr_buffer(bufferview_t _Right) : _addr(_Addr)
        {
            static_assert(_Addr != dynamic_addr, "dynamic_addr is not allowed here");
            _addr.read()(_Right, _buffer);
            Expects(_addr.len() < 0 || _buffer.size() == _addr.len());
        }
        addr_buffer(addr_t addr, bufferview_t _Right) : _addr(addr)
        {
            _addr.read()(_Right, _buffer);
            Expects(_addr.len() < 0 || _buffer.size() == _addr.len());
        }

        //    - from std::string
        addr_buffer(stringview_t _Right) : _addr(_Addr)
        {
            static_assert(_Addr != dynamic_addr, "dynamic_addr is not allowed here");
            Expects(!_Right.empty());
            _addr.serialize()(_Right, _buffer);
            Expects(_addr.len() < 0 || _buffer.size() == _addr.len());
        }
        addr_buffer(addr_t addr, stringview_t _Right) : _addr(addr)
        {
            Expects(!_Right.empty());
            _addr.serialize()(_Right, _buffer);
            Expects(_addr.len() < 0 || _buffer.size() == _addr.len());
        }

        addr_buffer(const char* _Right) : addr_buffer(gsl::ensure_z(_Right))
        { }
        addr_buffer(addr_t addr, const char* _Right) : addr_buffer(addr, gsl::ensure_z(_Right))
        { }

        //    - from addr_buffer<>
        //addr_buffer(const addr_buffer<_Addr>& _Right) : _addr(_Right.addr()), _buffer(_Right._buffer)
        //{ }
        template <addr_t _RightAddr>
        addr_buffer(const addr_buffer<_RightAddr>& _Right) : _addr(_Right.addr()), _buffer(_Right._buffer)
        {
            static_assert(_RightAddr == _Addr || _RightAddr == dynamic_addr || _Addr == dynamic_addr, "Mismatch between codes.");
        }


        // Assign by copying _Right
        //    - from addr_buffer<>
        addr_buffer<_Addr>& operator=(const addr_buffer<_Addr>& _Right)
        {
            Expects(_Right.addr() == addr());
            Expects(_buffer.empty() && !_Right._buffer.empty());
            _buffer = _Right._buffer;
            return *this;
        }
        template <addr_t _RightAddr>
        addr_buffer<_Addr>& operator=(const addr_buffer<_RightAddr>& _Right)
        {
            static_assert(_RightAddr == _Addr || _RightAddr == dynamic_addr || _Addr == dynamic_addr, "Mismatch between codes.");
            Expects(_Right.addr() == addr());
            Expects(_buffer.empty() && !_Right._buffer.empty());
            _buffer = _Right._buffer;
            return *this;
        }


        // Accessors
        addr_t       addr() const { return _addr.key(); }
        bufferview_t data() const { return _buffer; }
        string_t     str()  const {
            auto s = string_t{};
            _addr.deserialize()(_buffer, s);
            return s;
        }

    private:
        const details::addr_type<_Addr> _addr;
        buffer_t _buffer;

        template <addr_t _RightAddr> friend class addr_buffer;
        friend class multiaddr;
    };

    // Comparison operators
    template <addr_t _LeftAddr, addr_t _RightAddr>
    bool operator==(const addr_buffer<_LeftAddr>& _Left, const addr_buffer<_RightAddr>& _Right)
    {
        return (_Left.addr() == _Right.addr()) && (_Left.data() == _Right.data());
    }

    template <addr_t _LeftAddr, addr_t _RightAddr>
    bool operator!=(const addr_buffer<_LeftAddr>& _Left, const addr_buffer<_RightAddr>& _Right)
    {
        return !(_Left == _Right);
    }
    
    template <addr_t _LeftAddr, addr_t _RightAddr>
    bool operator<(const addr_buffer<_LeftAddr>& _Left, const addr_buffer<_RightAddr>& _Right) {
        if (_Left.addr() < _Right.addr()) return true;
        if (_Left.addr() > _Right.addr()) return false;
        auto minsize = std::min(_Left.data().size(), _Right.data().size());
        for (auto i = 0; i < minsize; i++) {
            if (_Left.data()[i] < _Right.data()[i]) return true;
            if (_Left.data()[i] > _Right.data()[i]) return false;
        }
        return false;
    }



    class multiaddr
    {
        using store_t = std::vector<addr_buffer<>>;

    public:
        // Construct empty
        multiaddr() {}

        // Construct by parsing _Right
        //    - from stringview_t
        multiaddr(stringview_t _Right)
        {
            auto view = _Right;

            while (!view.empty()) {
                // extract separator
                if (view[0] != '/') throw std::invalid_argument("Invalid multiaddr format: a protocol must follow a '/'");
                view = view.last(view.size() - 1);

                // a trailing / is valid
                if (view.empty()) break;

                // find next separator
                auto pos = std::find(view.begin(), view.end(), '/') - view.begin();
                //if (pos == view.size()) throw std::invalid_argument("Invalid multiaddr format: No protocol found");

                // extract protocol name
                auto protocol = view.first(pos);
                view = view.last(view.size() - pos);

                auto index = details::find_addrimpl_by_name(protocol);
                if (index == 0) throw std::invalid_argument("Invalid multiaddr format: unsupported protocol " + to_string(protocol));

                auto value = buffer_t{};
                if (details::_AddrTable[index].len != 0)
                {
                    // extract separator
                    if (view[0] != '/') throw std::invalid_argument("Invalid multiaddr format: a value must follow a '/'");
                    view = view.last(view.size() - 1);

                    // serialize 
                    view = details::_AddrTable[index].serialize(view, value);
                }

                // create a new addr_buffer
                auto addr = addr_buffer<>(details::_AddrTable[index].key, value);
                _protocols.push_back(addr);
            }
        }
        multiaddr(const char* _Right) : multiaddr(gsl::ensure_z(_Right))
        { }

        //    - from bufferview_t
        multiaddr(bufferview_t _Right)
        {
            auto view = _Right;

            while (!view.empty()) {

                // extract protocol
                uint32_t protocol;
                view = uvarint::decode(view, &protocol);

                // find protocol index
                auto index = details::find_addrimpl_by_code(protocol);
                if (index == 0) throw std::invalid_argument("Invalid multiaddr format: unsupported protocol");

                // extract value
                auto value = buffer_t{};
                view = details::_AddrTable[index].read(view, value);

                // create a new addr_buffer
                auto addr = addr_buffer<>(details::_AddrTable[index].key, value);
                _protocols.push_back(addr);
            }
        }

        //    - from addr_buffer<A>
        template <addr_t _Addr>
        multiaddr(const addr_buffer<_Addr>& _Right) : _protocols({_Right})
        { }

        multiaddr(const store_t& _Right) : _protocols(_Right)
        { }

        template<class _Iter>
        multiaddr(_Iter _First, _Iter _Last) : _protocols(_First, _Last)
        { }

        //    - from multiaddr
        multiaddr(const multiaddr& _Right) : _protocols(_Right._protocols)
        { }


        //
        //
        multiaddr encapsulate(const multiaddr& _Right) const
        {
            auto protocols = _protocols;
            protocols.insert(protocols.end(), _Right._protocols.begin(), _Right._protocols.end());
            return { protocols };
        }
        template <addr_t _Addr>
        multiaddr encapsulate(const addr_buffer<_Addr>& _Right) const 
        {
            auto protocols = _protocols;
            protocols.push_back(_Right);
            return { protocols };
        }


        multiaddr decapsulate(addr_t protocol) const
        {
            auto rit = std::find_if(_protocols.rbegin(), _protocols.rend(), [&](auto a) { return a.addr() == protocol; });
            if (rit == _protocols.rend()) return _protocols;

            auto it = rit.base() - 1;

            auto protocols = store_t{};
            protocols.insert(protocols.begin(), _protocols.begin(), it);
            return { protocols };
        }

        template <addr_t _Addr>
        multiaddr decapsulate(const addr_buffer<_Addr>& _Right) const
        {
            return decapsulate(_Right.addr());
        }

        multiaddr decapsulate(const multiaddr& _Right) const
        {
            auto it = std::find_end(_protocols.begin(), _protocols.end(), _Right._protocols.begin(), _Right._protocols.end());

            auto protocols = store_t{};
            protocols.insert(protocols.begin(), _protocols.begin(), it);
            return { protocols };
        }

        //
        inline bool has(addr_t protocol) const 
        {
            return std::find_if(_protocols.begin(), _protocols.end(), [&](auto a) { return a.addr() == protocol; }) != _protocols.end();
        }

        // Accessors
        bool        empty()              const { return _protocols.empty(); }
        const auto& protocols()          const { return _protocols; }
        size_t      size()               const { return _protocols.size(); }
        const auto& operator[](size_t i) const { return _protocols[i]; }
        
        string_t str() const 
        {
            if (empty()) return "/";

            auto oss = std::ostringstream{};
            for (auto prot : _protocols) {
                oss << "/" << prot._addr.name() << "/" << prot.str();
            }
            return oss.str(); 
        }
       
        buffer_t data() const 
        {
            auto buffer = buffer_t{};
            for (auto prot : _protocols) {
                uvarint::encode(prot._addr.code(), std::back_inserter(buffer));
                auto value = prot.data();
                std::copy(value.begin(), value.end(), std::back_inserter(buffer));
            }
            return buffer;
        }


    private:
        std::vector<addr_buffer<>> _protocols;
    };


    // Comparison operators
    inline bool operator==(const multiaddr& a, const multiaddr& b) 
    {
        return (a.protocols() == b.protocols());
    }
    
    inline bool operator!=(const multiaddr& a, const multiaddr& b) 
    {
        return !(a == b);
    }
    
    inline bool operator<(const multiaddr& a, const multiaddr& b) 
    {
        auto ap = a.protocols();
        auto bp = b.protocols();

        auto minsize = std::min(ap.size(), bp.size());
        for (auto i = 0; i < minsize; i++) {
            if (ap[i] < bp[i]) return true;
            if (bp[i] < ap[i]) return false;
        }
        return false;
    }



    //------------------------------------------------------
    // multiaddr filtering
    //------------------------------------------------------
    // https://github.com/multiformats/js-mafmt/blob/master/src/index.js

    namespace details {

        using addr_span = gsl::span<const addr_buffer<>>;

        template <addr_t _Addr>
        class vbase {
        public:
            static addr_span partialmatch(addr_span s)
            {
                if (s.empty()) return {};
                if (s[0].addr() != _Addr) return {};
                return s.last(s.size() - 1);
            }
        };

        template <class FirstType, class ... Types>
        class vand {
        public:
            static addr_span partialmatch(addr_span s)
            {
                auto a = FirstType::partialmatch(s);
                if (a.data() == nullptr || sizeof...(Types)==0) return a;
                return vand<Types...>::partialmatch(a);
            }
        };
        template <class FirstType>
        class vand<FirstType> {
        public:
            static addr_span partialmatch(addr_span s)
            {
                return FirstType::partialmatch(s);
            }
        };


        template <class FirstType, class ...Types>
        struct vor {
            static addr_span partialmatch(addr_span s)
            {
                auto a = FirstType::partialmatch(s);
                if (a.data() || sizeof...(Types)==0) return a;
                return vor<Types...>::partialmatch(s);
            }
        };
        template <class FirstType>
        struct vor<FirstType> {
            static addr_span partialmatch(addr_span s)
            {
                return FirstType::partialmatch(s);
            }
        };


        template <class Type>
        bool match(const multiaddr& ma)
        {
            auto out = Type::partialmatch(gsl::make_span(ma.protocols()));
            return out.data() && out.size() == 0;
        }

        //
        using DNS4 = vbase<dns4>;
        using DNS6 = vbase<dns6>;
        using _DNS = vor<vbase<dns>, DNS4, DNS6>;

        using IP = vor <vbase<ip4>, vbase<ip6>>;
        using TCP = vand <IP, vbase<tcp>>;
        using UDP = vand <IP, vbase<udp>>;
        using UTP = vand <UDP, vbase<utp>>;

        using DNS = vor <
            vand <_DNS, vbase<tcp>>,
            _DNS
        >;

        using WebSockets = vor <
            vand <TCP, vbase<ws>>,
            vand <DNS, vbase<ws>>
        >;

        using WebSocketsSecure = vor <
            vand <TCP, vbase<wss>>,
            vand <DNS, vbase<wss>>
        >;

        using HTTP = vor <
            vand <TCP, vbase<http>>,
            vand <DNS>,
            vand <DNS, vbase<http>>
        >;

        using WebRTCStar = vor <
            vand <WebSockets, vbase<p2p_webrtc_star>, vbase<ipfs>>,
            vand <WebSocketsSecure, vbase<p2p_webrtc_star>, vbase<ipfs>>
        >;

        using WebSocketStar = vor <
            vand <WebSockets, vbase<p2p_websocket_star>, vbase<ipfs>>,
            vand <WebSocketsSecure, vbase<p2p_websocket_star>, vbase<ipfs>>,
            vand <WebSockets, vbase<p2p_websocket_star>>,
            vand <WebSocketsSecure, vbase<p2p_websocket_star>>
        >;

        using WebRTCDirect = vand <HTTP, vbase<p2p_webrtc_direct>>;

        using Reliable = vor <
            WebSockets,
            WebSocketsSecure,
            HTTP,
            WebRTCStar,
            WebRTCDirect,
            TCP,
            UTP
        >;

        using _IPFS = vor <
            vand <Reliable, vbase<ipfs>>,
            WebRTCStar,
            vbase<ipfs>
        >;

        using _Circuit = vor <
            vand <_IPFS, vbase<p2p_circuit>, _IPFS>,
            vand <_IPFS, vbase<p2p_circuit>>,
            vand <vbase<p2p_circuit>, _IPFS>,
            vand <Reliable, vbase<p2p_circuit>>,
            vand <vbase<p2p_circuit>, Reliable>,
            vbase<p2p_circuit>
        >;


        class CircuitRecursive : public vor <
            vand <_Circuit, CircuitRecursive>,
            _Circuit
        >{};

        using Circuit = CircuitRecursive;

        using IPFS = vor <
            vand <Circuit, _IPFS, Circuit>,
            vand <_IPFS, Circuit>,
            vand <Circuit, _IPFS>,
            Circuit,
            _IPFS
        >;
    }

    inline bool is_dns4(const multiaddr& ma)            { return details::match<details::DNS4>(ma); }
    inline bool is_dns6(const multiaddr& ma)            { return details::match<details::DNS6>(ma); }
    inline bool is_dns(const multiaddr& ma)             { return details::match<details::DNS>(ma); }
    inline bool is_ip(const multiaddr& ma)              { return details::match<details::IP>(ma); }
    inline bool is_tcp(const multiaddr& ma)             { return details::match<details::TCP>(ma); }
    inline bool is_udp(const multiaddr& ma)             { return details::match<details::UDP>(ma); }
    inline bool is_utp(const multiaddr& ma)             { return details::match<details::UTP>(ma); }
    inline bool is_http(const multiaddr& ma)            { return details::match<details::HTTP>(ma); }
    inline bool is_websockets(const multiaddr& ma)      { return details::match<details::WebSockets>(ma); }
    inline bool is_websocketssecure(const multiaddr& ma){ return details::match<details::WebSocketsSecure>(ma); }
    inline bool is_websocketsstar(const multiaddr& ma)  { return details::match<details::WebSocketStar>(ma); }
    inline bool is_webrtcstar(const multiaddr& ma)      { return details::match<details::WebRTCStar>(ma); }
    inline bool is_webrtcdirect(const multiaddr& ma)    { return details::match<details::WebRTCDirect>(ma); }
    inline bool is_reliable(const multiaddr& ma)        { return details::match<details::Reliable>(ma); }
    inline bool is_circuit(const multiaddr& ma)         { return details::match<details::Circuit>(ma); }
    inline bool is_ipfs(const multiaddr& ma)            { return details::match<details::IPFS>(ma); }


}
