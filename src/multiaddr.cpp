#include <multiformats/multiaddr.h>
#include <multiformats/multihash.h>

#include <sstream>

using namespace multiformats;

stringview_t multiformats::details::serialize_ipv4(stringview_t src, buffer_t& dst)
{
    auto first = std::begin(src);
    auto end = std::end(src);
    auto last = first + 1;
    for (int i = 0; i < 4; i++)
    {
        last = std::find_if(first, end, [](char c) { return c != 'x' && (c < '0' || c > '9'); });

        auto value = std::stoi(std::string{ first, last }, 0, 0);
        if (value < 0 || value > 0xFF) throw std::invalid_argument("Invalid IP4 address");

        dst.push_back(gsl::narrow<byte_t>(value));

        if (i < 3) {
            if (*last != '.') throw std::invalid_argument("Invalid IP4 address");
            first = last + 1;
        }
    }

    return src.last(end-last);
}
bufferview_t multiformats::details::deserialize_ipv4(bufferview_t src, std::string& dst)
{
    if (src.size() < 4)  throw std::invalid_argument("Invalid IP4 address");
    
    std::ostringstream oss;
    oss << uint32_t{ src[0] } << '.' << uint32_t{ src[1] } << '.' << uint32_t{ src[2] } << '.' << uint32_t{ src[3] };
    dst += oss.str();

    return src.last(src.size() - 4);
}
bufferview_t multiformats::details::read_ipv4(bufferview_t src, buffer_t& dst)
{
    if (src.size() < 4)  throw std::invalid_argument("Invalid IP4 address");
    
    auto first = src.first(4);
    dst.insert(dst.end(), first.begin(), first.end());
    
    return src.last(src.size() - 4);
}


stringview_t multiformats::details::serialize_ipv6(stringview_t src, buffer_t& dst)
{
    auto elems = std::vector<stringview_t>{};

    auto pos = to_string(src).find_first_not_of("0123456789abcdefABCDEF:");

    auto begin = src.begin();
    auto end = (pos == std::string::npos) ? src.end() : begin + pos;
    auto first = begin;
    auto last = first;

    while (last != end && elems.size() < 8) {
        last = std::find(first, end, ':');
        if (last - first > 4) throw std::invalid_argument("Invalid IP6 address: wrong size of element");

        elems.push_back(src.subspan(first - begin, last - first));

        if (last != end) first = last + 1;
    }

    auto zeros = 8 - elems.size();
    for (auto item : elems)
        if (item.empty())
            zeros++;

    auto elemview = gsl::span<stringview_t>(elems);
    if (elemview[0].empty()) elemview = elemview.last(elemview.size() - 1);
    if (elemview[elemview.size() - 1].empty()) elemview = elemview.first(elemview.size() - 1);

    for (auto item : elemview) {
        if (item.empty()) {
            dst.insert(dst.end(), 2 * zeros, 0);
        }
        else {
            auto value = std::stoi(gsl::to_string(item), nullptr, 16);

            dst.push_back(gsl::narrow<byte_t>(value >> 8));
            dst.push_back(gsl::narrow<byte_t>(value & 0xFF));
        }
    }

    return src.last(src.end() - last);
}
bufferview_t multiformats::details::deserialize_ipv6(bufferview_t src, string_t& dst)
{
    dst += encode(base58btc, src).str();
    return {};
}
bufferview_t multiformats::details::read_ipv6(bufferview_t src, buffer_t& dst)
{
    dst += src;
    return {};
}


stringview_t multiformats::details::serialize_ipfs(stringview_t src, buffer_t& dst)
{
    auto buffer = buffer_t{};
    auto view = decode(base58btc, src, buffer);
    if (multihash{ buffer }.hash() != sha2_256) throw std::invalid_argument("Invalid IPFS address : wrong hash");
    dst += buffer;
    return view;
}
bufferview_t multiformats::details::deserialize_ipfs(bufferview_t src, string_t& dst)
{
    dst += encode(base58btc, src).str();
    return {};
}
bufferview_t multiformats::details::read_ipfs(bufferview_t src, buffer_t& dst)
{
    dst += src;
    return {};
}


stringview_t multiformats::details::serialize_onion(stringview_t src, buffer_t& dst)
{
    auto last = std::find(src.begin(), src.end(), '/');

    auto view = src.first(last - src.begin());

    auto parts = split(src, ':');
    if (parts.size() != 2) throw std::invalid_argument("Invalid onion address : must <address>:<port>");

    // onion address without the ".onion" substring
    if (parts[0].size() != 16) throw std::invalid_argument("Invalid onion address : wrong address size");

    // onion port number
    auto port = std::stoi(gsl::to_string(parts[1]), nullptr, 0);
    if (port < 1 || port > 0xFFFF) throw std::invalid_argument("Invalid onion address : out-of-range port");

    dst += decode(base32, parts[0]);
    dst += gsl::narrow<byte_t>(port >> 8);
    dst += gsl::narrow<byte_t>(port & 0xFF);

    return src.last(src.end() - last);
}
bufferview_t multiformats::details::deserialize_onion(bufferview_t src, string_t& dst)
{
    return src;
}
bufferview_t multiformats::details::read_onion(bufferview_t src, buffer_t& dst)
{
    return src;
}


stringview_t serialize_lenstring(stringview_t src, buffer_t& dst)
{
    dst += uvarint::encode(src.size());
    dst += as_buffer(src);

    return {};
}
bufferview_t deserialize_lenstring(bufferview_t src, string_t& dst)
{
    auto len = ptrdiff_t{};
    src = uvarint::decode(src, &len);
    if (len > src.size()) throw std::invalid_argument("Invalid address : not enough data");

    dst.append(src.begin(), src.begin() + len);

    return src.last(src.size() - len);
}
bufferview_t read_lenstring(bufferview_t src, buffer_t& dst)
{
    auto len = ptrdiff_t{};
    src = uvarint::decode(src, &len);
    if (len > src.size()) throw std::invalid_argument("Invalid unix address : not enough data");

    dst += uvarint::encode(src.size());
    dst += src.first(len);

    return src.last(src.size() - len);
}


stringview_t multiformats::details::serialize_unix(stringview_t src, buffer_t& dst)
{
    // the address is the whole remaining string, prefixed by a varint len
    return serialize_lenstring(src, dst);
}
bufferview_t multiformats::details::deserialize_unix(bufferview_t src, string_t& dst)
{
    // the address is a varint len prefixed string
    return deserialize_lenstring(src, dst);
}
bufferview_t multiformats::details::read_unix(bufferview_t src, buffer_t& dst)
{
    return read_lenstring(src, dst);
}


stringview_t multiformats::details::serialize_dns(stringview_t src, buffer_t& dst)
{
    auto last = std::find(src.begin(), src.end(), '/');
    auto view = src.first(last - src.begin());

    serialize_lenstring(src, dst);

    return src.last(src.end() - last);
}
bufferview_t multiformats::details::deserialize_dns(bufferview_t src, string_t& dst)
{
    return deserialize_lenstring(src, dst);
}
bufferview_t multiformats::details::read_dns(bufferview_t src, buffer_t& dst)
{
    return read_lenstring(src, dst);
}


stringview_t multiformats::details::serialize_port(stringview_t src, buffer_t& dst)
{
    size_t index = 0;
    auto value = std::stoi(gsl::to_string(src), &index, 0);

    if (value < 0 || value > 0xFFFF) throw std::invalid_argument("Invalid port");
    
    dst.push_back(gsl::narrow<byte_t>(value >> 8));
    dst.push_back(gsl::narrow<byte_t>(value & 0xFF));
    
    return src.last(src.size() - index);
}
bufferview_t multiformats::details::deserialize_port(bufferview_t src, std::string& dst)
{
    if (src.size() < 2)  throw std::invalid_argument("Invalid port");

    uint16_t port = (uint16_t{ src[0] } << 8) + src[1];
    std::ostringstream oss;
    oss << port;
    dst += oss.str();

    return src.last(src.size() - 2);
}
bufferview_t multiformats::details::read_port(bufferview_t src, buffer_t& dst)
{
    if (src.size() < 2)  throw std::invalid_argument("Invalid port");

    auto first = src.first(2);
    dst.insert(dst.end(), first.begin(), first.end());

    return src.last(src.size() - 2);
}
