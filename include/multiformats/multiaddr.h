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

    // operator==
    template <addr_t _LeftAddr, addr_t _RightAddr>
    bool operator==(const addr_buffer<_LeftAddr>& _Left, const addr_buffer<_RightAddr>& _Right)
    {
        return (_Left.addr() == _Right.addr()) && (_Left.data() == _Right.data());
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

        // Accessors
        bool        empty()     const { return _protocols.empty(); }
        const auto& protocols() const { return _protocols; }
        
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

    //struct invalid_protocol_exception : public std::exception
    //{
    //    invalid_protocol_exception(const char* message) : std::exception(message) {}
    //};

    //struct parsing_error : public std::exception
    //{};



    //const _Protocol& find_protocol(code_t code) {
    //    for (auto& item : _Protocols)
    //        if (item.code == code)
    //            return item;

    //    throw invalid_protocol_exception("invalid protocol code");
    //}

    //const _Protocol& find_protocol(const std::string& name) {
    //    for (auto& item : _Protocols)
    //        if (item.name == name)
    //            return item;

    //    throw invalid_protocol_exception("invalid protocol name");
    //}


    //class multiaddr {
    //public:
    //    using store_t = std::vector<byte_t>;

    //    store_t _Store;

    //public:
    //    multiaddr() {}

    //    multiaddr(gsl::cstring_span<> addr)
    //    {
    //        auto first = addr.begin();
    //        if (addr[0] == '/') first++;

    //        auto last = first;

    //        while (last != addr.end())
    //        {
    //            // next token
    //            last = std::find(first, addr.end(), '/');
    //            
    //            auto protoName = std::string(first, last);
    //            first = last + 1;

    //            auto protocol = find_protocol(protoName); // throws if not found
    //            uvarint::encode(protocol.code, std::back_inserter(_Store));

    //            if (protocol.len) 
    //            {
    //                // next token
    //                if (last == addr.end()) throw std::invalid_argument("Invalid address format");
    //                last = std::find(first, addr.end(), '/');

    //                protocol.encode({ first, last }, std::back_inserter(_Store));
    //                first = last + 1;
    //            }
    //        }
    //    }

    //    //multiaddr(gsl::czstring_span<> addr)
    //    //{

    //    //}



    //    inline bool empty() const { return _Store.empty(); }
    //};

    //inline bool operator<(const multiaddr& a, const multiaddr& b) {
    //    return a._Store < b._Store;
    //}


}
