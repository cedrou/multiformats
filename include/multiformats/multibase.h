#pragma once

#include "common.h"

#include <string>
#include <vector>
#include <array>
#include <map>
#include <gsl\gsl>
#include <type_traits>

namespace multiformats {
    
    enum base_t {
        dynamic_base = -1,
        identity = 0,
        base256,
        //base1,        // '1'
        base2,        // '01'
        base8,        // '01234567'
        base10,       // '0123456789'
        base16,       // '0123456789abcdef'
        BASE16,       // '0123456789ABCDEF'
        base32,       // 'abcdefghijklmnopqrstuvwxyz234567' - rfc4648 no padding
        BASE32,       // 'ABCDEFGHIJKLMNOPQRSTUVWXYZ234567' - rfc4648 no padding
        base32pad,    // 'abcdefghijklmnopqrstuvwxyz234567=' - rfc4648 with padding
        BASE32pad,    // 'ABCDEFGHIJKLMNOPQRSTUVWXYZ234567=' - rfc4648 with padding
        base32hex,    // '0123456789abcdefghijklmnopqrstuv' - rfc4648 no padding - highest char
        BASE32hex,    // '0123456789ABCDEFGHIJKLMNOPQRSTUV' - rfc4648 no padding - highest char
        base32hexpad, // '0123456789abcdefghijklmnopqrstuv=' - rfc4648 with padding
        BASE32hexpad, // '0123456789ABCDEFGHIJKLMNOPQRSTUV=' - rfc4648 with padding
        base32z,      // 'ybndrfg8ejkmcpqxot1uwisza345h769' - z - base - 32 - used by Tahoe - LAFS - highest letter
        base58flickr, // '123456789abcdefghijkmnopqrstuvwxyzABCDEFGHJKLMNPQRSTUVWXYZ' - highest letter
        base58btc,    // '123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz' - highest letter
        base64,       // 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/' - rfc4648 no padding
        base64pad,    // 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=' - rfc4648 with padding - MIME encoding
        base64url,    // 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_' - rfc4648 no padding
        base64urlpad, // 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_=' - rfc4648 with padding
    };




    namespace details {

        typedef buffer_t(*CodecFunc)(bufferview_t, const char*);
        inline buffer_t codec_noimpl(bufferview_t /*data*/, const char* /*digits*/) { return {}; }
        buffer_t encode_base0(bufferview_t data, const char* digits);
        buffer_t encode_base2(bufferview_t data, const char* digits);
        buffer_t encode_base8(bufferview_t data, const char* digits);
        //buffer_t encode_base10(bufferview_t data, const char* digits);
        buffer_t encode_base16(bufferview_t data, const char* digits);
        buffer_t encode_base32(bufferview_t data, const char* digits);
        //buffer_t encode_base58(bufferview_t data, const char* digits);
        buffer_t encode_base64(bufferview_t data, const char* digits);

        buffer_t decode_base0(bufferview_t data, const char* digits);
        buffer_t decode_base2(bufferview_t data, const char* digits);
        buffer_t decode_base8(bufferview_t data, const char* digits);
        //buffer_t decode_base10(bufferview_t data, const char* digits);
        buffer_t decode_base16(bufferview_t data, const char* digits);
        buffer_t decode_base32(bufferview_t data, const char* digits);
        //buffer_t decode_base58(bufferview_t data, const char* digits);
        buffer_t decode_base64(bufferview_t data, const char* digits);

        template <base_t _FromBase, base_t _ToBase>
        buffer_t convert_base(bufferview_t from, const char*);


        struct baseimpl {
            base_t    key;
            const char* name;
            char        code;
            int         radix;
            CodecFunc   encode;
            CodecFunc   decode;
            const char* digits;
        };

        constexpr baseimpl _BaseTable[] = {
            { dynamic_base, "dynamic_base",       -1,  -1, codec_noimpl                       , codec_noimpl                       , nullptr },
            { base256,      "base256",        0, 256, codec_noimpl                       , codec_noimpl                       , nullptr },
            { identity,     "identity",       0,   0, encode_base0                       , decode_base0                       , nullptr },
            { base2,        "base2",        '0',   2, encode_base2                       , decode_base2                       , "01" },
            { base8,        "base8",        '7',   8, encode_base8                       , decode_base8                       , "01234567" },
            { base10,       "base10",       '9',  10, convert_base<base256, base10>      , convert_base<base10, base256>      , "0123456789" },
            { base16,       "base16",       'f',  16, encode_base16                      , decode_base16                      , "0123456789abcdef" },
            { BASE16,       "BASE16",       'F',  16, encode_base16                      , decode_base16                      , "0123456789ABCDEF" },
            { base32,       "base32",       'b',  32, encode_base32                      , decode_base32                      , "abcdefghijklmnopqrstuvwxyz234567" },
            { BASE32,       "BASE32",       'B',  32, encode_base32                      , decode_base32                      , "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567" },
            { base32pad,    "base32pad",    'c',  32, encode_base32                      , decode_base32                      , "abcdefghijklmnopqrstuvwxyz234567=" },
            { BASE32pad,    "BASE32pad",    'C',  32, encode_base32                      , decode_base32                      , "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567=" },
            { base32hex,    "base32hex",    'v',  32, encode_base32                      , decode_base32                      , "0123456789abcdefghijklmnopqrstuv" },
            { BASE32hex,    "BASE32hex",    'V',  32, encode_base32                      , decode_base32                      , "0123456789ABCDEFGHIJKLMNOPQRSTUV" },
            { base32hexpad, "base32hexpad", 't',  32, encode_base32                      , decode_base32                      , "0123456789abcdefghijklmnopqrstuv=" },
            { BASE32hexpad, "BASE32hexpad", 'T',  32, encode_base32                      , decode_base32                      , "0123456789ABCDEFGHIJKLMNOPQRSTUV=" },
            { base32z,      "base32z",      'h',  32, encode_base32                      , decode_base32                      , "ybndrfg8ejkmcpqxot1uwisza345h769" },
            { base58flickr, "base58flickr", 'Z',  58, convert_base<base256, base58flickr>, convert_base<base58flickr, base256>, "123456789abcdefghijkmnopqrstuvwxyzABCDEFGHJKLMNPQRSTUVWXYZ" },
            { base58btc,    "base58btc",    'z',  58, convert_base<base256, base58btc>   , convert_base<base58btc, base256>   , "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz" },
            { base64,       "base64",       'm',  64, encode_base64                      , decode_base64                      , "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/" },
            { base64pad,    "base64pad",    'M',  64, encode_base64                      , decode_base64                      , "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=" },
            { base64url,    "base64url",    'u',  64, encode_base64                      , decode_base64                      , "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_" },
            { base64urlpad, "base64urlpad", 'U',  64, encode_base64                      , decode_base64                      , "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_=" },
        };

        constexpr int find_baseimpl(base_t code) {
            for (auto i = 0; i < _countof(_BaseTable); i++)
                if (_BaseTable[i].key == code) return i;
            return 0;
        }
        constexpr base_t find_basecode(byte_t code) {
            for (auto i = 0; i < _countof(_BaseTable); i++)
                if (_BaseTable[i].code == code) return _BaseTable[i].key;
            return dynamic_base;
        }

        inline byte_t from_digit(byte_t digit, const char* begin, const char* end)
        {
            auto ptr = std::find(begin, end, digit);
            if (ptr == end) throw std::out_of_range("Provided digit is not in alphabet");
            return gsl::narrow<byte_t>(ptr - begin);
        }

        template <base_t _Base>
        byte_t from_digit(byte_t digit) {
            constexpr auto _Index = details::find_baseimpl(_Base);
            constexpr auto _Impl = details::_BaseTable[_Index];

            static_assert(_Index > 0, "base not implemented");

            const auto begin = _Impl.digits;
            const auto end = _Impl.digits + _Impl.radix;
            return from_digit(digit, begin, end);
        }
        template<>
        inline byte_t from_digit<base256>(byte_t digit) {
            return digit;
        }

        template <base_t _Base>
        byte_t to_digit(byte_t value) {
            constexpr auto _Index = details::find_baseimpl(_Base);
            constexpr auto _Impl = details::_BaseTable[_Index];

            static_assert(_Index > 0, "base not implemented");
            if (value >= _Impl.radix) throw std::out_of_range(std::string("Provided value is not in accepted range of ") + _Impl.name + ".");

            return _Impl.digits[value];
        }
        template<>
        inline byte_t to_digit<base256>(byte_t value) {
            return value;
        }

        template <base_t _FromBase, base_t _ToBase>
        buffer_t convert_base(bufferview_t from, const char*)
        {
            constexpr auto _FromIndex = details::find_baseimpl(_FromBase);
            constexpr auto _ToIndex = details::find_baseimpl(_ToBase);
            constexpr auto _FromImpl = details::_BaseTable[_FromIndex];
            constexpr auto _ToImpl = details::_BaseTable[_ToIndex];

            static_assert(_FromIndex > 0 && _ToIndex > 0, "base not implemented");

            // Skip & count leading zeroes.
            auto first = std::find_if_not(std::begin(from), std::end(from), [](auto v) { return v == 0; });
            auto last = std::end(from);

            const auto leadingZeroes = first - std::begin(from);
            const auto dataSize = last - first;

            // Compute the max size of the encoded string, maybe shorter (log(256) / log(10), rounded up).
            const auto tmpSize = size_t(dataSize * log((double)_FromImpl.radix) / log((double)_ToImpl.radix)) + 1;
            auto tmp = buffer_t(tmpSize, 0);

            // Process the data
            auto encodedLen = 0;
            while (first != last)
            {
                auto carry = static_cast<uint32_t>(from_digit<_FromBase>(*first++));

                auto i = 0;
                for (auto it = tmp.rbegin(); it != tmp.rend() && (carry || i < encodedLen); i++, it++)
                {
                    carry += _FromImpl.radix * (*it);
                    *it = gsl::narrow<byte_t>(carry % _ToImpl.radix);
                    carry /= _ToImpl.radix;
                }

                encodedLen = i;
            }

            // Skip leading zeroes in encoded buffer (keep at least one zero if null)
            auto tmpFirst = std::find_if_not(std::begin(tmp), std::end(tmp), [](auto v) { return v == 0; });
            if (tmpFirst == std::end(tmp)) tmpFirst--;

            // translate with digits
            const auto encodedSize = leadingZeroes + (std::end(tmp) - tmpFirst);
            auto encoded = buffer_t(encodedSize, to_digit<_ToBase>(0));
            for (auto i = leadingZeroes; i < encodedSize; ++i)
                encoded[i] = to_digit<_ToBase>(*tmpFirst++);

            return encoded;
        }


        template <base_t _Base = dynamic_base, int _Index = find_baseimpl(_Base)>
        class basecode_type
        {
        public:
            static_assert(_Base != dynamic_base, "dynamic_base is not allowed here");
            static_assert(_Index > 0, "The base_t is not implemented");

            constexpr basecode_type(base_t base) { Expects(base == _Base); }

            constexpr base_t      base()   const { return _BaseTable[_Index].key; }
            constexpr const char* name()   const { return _BaseTable[_Index].name; }
            constexpr uint32_t    code()   const { return _BaseTable[_Index].code; }
            constexpr int32_t     radix()  const { return _BaseTable[_Index].radix; }
            constexpr const char* digits() const { return _BaseTable[_Index].digits; }
            constexpr CodecFunc   encode() const { return _BaseTable[_Index].encode; }
            constexpr CodecFunc   decode() const { return _BaseTable[_Index].decode; }
            constexpr int         index()  const { return _Index; }
        };
        template <>
        class basecode_type<dynamic_base>
        {
        public:
            explicit constexpr basecode_type(base_t base) : _index(find_baseimpl(base)) { Expects(_index > 0); }

            constexpr base_t      base()   const { return _BaseTable[_index].key; }
            constexpr const char* name()   const { return _BaseTable[_index].name; }
            constexpr uint32_t    code()   const { return _BaseTable[_index].code; }
            constexpr int32_t     radix()  const { return _BaseTable[_index].radix; }
            constexpr const char* digits() const { return _BaseTable[_index].digits; }
            constexpr CodecFunc   encode() const { return _BaseTable[_index].encode; }
            constexpr CodecFunc   decode() const { return _BaseTable[_index].decode; }
            constexpr int         index()  const { return _index; }
        private:
            const int _index;
        };
    }


    //
    // The class `encoded_string<base_t>` is used to strongly type strings that are encoded in a base known at compile-time.
    // The specialization `encoded_string<dynamic_base>` is used to type encoded strings for which the base is not known at compile-time.
    //
    template <base_t _Base = dynamic_base>
    class encoded_string
    {
    public:
        // Construct empty encoded_string<>
        encoded_string() : _base(_Base) {}
        encoded_string(base_t code) : _base(code) {}

        // Construct by copying _Right
        //    - from string
        encoded_string(stringview_t _Right) : _string(to_string(_Right)), _base(_Base) {}
        encoded_string(base_t code, stringview_t _Right) : _string(to_string(_Right)), _base(code) {}

        encoded_string(const char* _Right) : encoded_string(gsl::ensure_z(_Right)) {}
        encoded_string(base_t code, const char* _Right) : encoded_string(code, gsl::ensure_z(_Right)) {}

        //    - from encoded_string<>
        encoded_string(const encoded_string<_Base>& _Right) : _string(_Right._string), _base(_Right.base()) {}
        template <base_t _RightBase>
        encoded_string(const encoded_string<_RightBase>& _Right) : _string(_Right._string), _base(_Right.base()) 
        {
            static_assert(_RightBase == _Base || _RightBase == dynamic_base || _Base == dynamic_base, "Mismatch between codes.");
        }

        // Construct by moving _Right
        //    - from string
        encoded_string(string_t&& _Right) : _string(std::move(_Right)), _base(_Base) {}
        encoded_string(base_t code, string_t&& _Right) : _string(std::move(_Right)), _base(code) {}

        //    - from encoded_string<>
        encoded_string(encoded_string<_Base>&& _Right) : _string(std::move(_Right._string)), _base(_Right.base()) {}
        template <base_t _RightBase>
        encoded_string(encoded_string<_RightBase>&& _Right) : _string(std::move(_Right._string)), _base(_Right.base())
        {
            static_assert(_RightBase == _Base || _RightBase == dynamic_base || _Base == dynamic_base, "Mismatch between codes.");
        }


        // Assign by copying _Right
        // Note: an encoded_string<> is immutable once initialized
        //    - from string
        encoded_string<_Base>& operator =(stringview_t _Right)
        {
            Expects(_string.empty() && !_Right.empty()); 
            _string = to_string(_Right);
            return *this;
        }
        encoded_string<_Base>& operator =(const char* _Right)
        {
            return operator+(gsl::ensure_z(_Right));
        }


        //    - from encoded_string<>
        encoded_string<_Base>& operator =(const encoded_string<_Base>& _Right)
        {
            Expects(_Right.base() == base());
            Expects(_string.empty() && !_Right.empty());
            _string = _Right._string;
            return *this;
        }
        template <base_t _RightBase>
        encoded_string<_Base>& operator =(const encoded_string<_RightBase>& _Right)
        {
            static_assert(_RightBase == _Base || _RightBase == dynamic_base || _Base == dynamic_base, "Mismatch between codes.");
            Expects(_Right.base() == base());
            Expects(_string.empty() && !_Right.empty());
            _string = _Right._string;
            return *this;
        }


        // Assign by moving _Right
        // Note: an encoded_string<> is immutable once initialized
        //    - from string
        encoded_string<_Base>& operator =(string_t&& _Right)
        {
            Expects(_string.empty() && !_Right.empty());
            _string = std::move(_Right);
            return *this;
        }

        //    - from encoded_string<>
        encoded_string<_Base>& operator =(encoded_string<_Base>&& _Right)
        {
            Expects(_Right.base() == base());
            Expects(_string.empty() && !_Right.empty());
            _string = std::move(_Right._string);
            return *this;
        }

        template <base_t _RightBase>
        encoded_string<_Base>& operator =(encoded_string<_RightBase>&& _Right)
        {
            static_assert(_RightBase == _Base || _RightBase == dynamic_base || _Base == dynamic_base, "Mismatch between codes.");
            Expects(_Right.base() == base());
            Expects(_string.empty() && !_Right.empty());
            _string = std::move(_Right._string);
            return *this;
        }


        base_t base() const { return _base.base(); }
        const string_t& str() const { return _string; }

        bool empty() const { return _string.empty(); }


    private:
        const details::basecode_type<_Base> _base;
        string_t _string;
        
        template <base_t _FriendBase> friend class encoded_string;
    };

    // operator+
    template <base_t _Base>
    std::string operator+(stringview_t _Left, const encoded_string<_Base>& _Right) { return (_Left + _Right.str()); }
    template <base_t _Base>
    std::string operator+(const encoded_string<_Base>& _Left, stringview_t _Right) { return (_Left.str() + _Right); }
    
    //template <base_t _Base>
    //std::string operator+(stringview_t _Left, const encoded_string<_Base>& _Right) { return (_Left + _Right.str()); }
    //template <base_t _Base>
    //std::string operator+(const encoded_string<_Base>& _Left, stringview_t _Right) { return (_Left.str() + _Right); }

    template <base_t _Base>
    std::string operator+(const char* _Left, const encoded_string<_Base>& _Right) { return (_Left + _Right.str()); }
    template <base_t _Base>
    std::string operator+(const encoded_string<_Base>& _Left, const char* _Right) { return (_Left.str() + _Right); }

    // operator==
    template <base_t _BaseLeft, base_t _BaseRight>
    bool operator==(const encoded_string<_BaseLeft>& _Left, const encoded_string<_BaseRight>& _Right) 
    { 
        return (_Left.base() == _Right.base()) && (_Left.str() == _Right.str());
    }

    template <base_t _Base>
    bool operator==(const std::string& _Left, const encoded_string<_Base>& _Right) { return (_Left == _Right.str()); }
    template <base_t _Base>
    bool operator==(const encoded_string<_Base>& _Left, const std::string& _Right) { return (_Left.str() == _Right); }

    //
    template <base_t _Base>
    std::ostream& operator<< (std::ostream& os, const encoded_string<_Base>& _Right) { return os << _Right.str(); }

    //
    // Encode a buffer/string into a _Base encoded_string
    //
    template <base_t _Base>
    encoded_string<_Base> encode(bufferview_t data) 
    {
        constexpr auto _Index = details::find_baseimpl(_Base);
        constexpr auto _Impl = details::_BaseTable[_Index];

        static_assert(_Base != dynamic_base, "encode<_Base>(bufferview_t) is not allowed for _Base == basecode::dynamic_base");
        static_assert(_Index > 0, "encode<_Base>(bufferview_t) is not implementeed for this _Base");
        
        Expects(!data.empty());
        return as_string(_Impl.encode(data, _Impl.digits));
    }

    inline encoded_string<> encode(base_t base, bufferview_t data)
    {
        Expects(!data.empty());

        const auto index = details::find_baseimpl(base);
        const auto impl = details::_BaseTable[index];
        Expects(index > 0);

        return { base, as_string(impl.encode(data, impl.digits)) };
    }


    template <base_t _Base>
    encoded_string<_Base> encode(const std::string& string)                { return encode<_Base>(as_buffer(string)); }
    inline encoded_string<> encode(base_t code, const std::string& string) { return encode(code, as_buffer(string)); }

    //
    // Decode a _Base encoded_string into a buffer
    //
    template <base_t _Base>
    buffer_t decode(const encoded_string<_Base>& string)
    {
        constexpr auto _Index = details::find_baseimpl(_Base);
        constexpr auto _Impl = details::_BaseTable[_Index];

        static_assert(_Base != dynamic_base, "decode<_Base>(encoded_string) is not allowed for _Base == basecode::dynamic_base");
        static_assert(_Index > 0, "decode<_Base>(encoded_string) is not implementeed for this _Base");
        Expects(!string.empty());

        return _Impl.decode(as_buffer(string.str()), _Impl.digits);
    }

    inline buffer_t decode(const encoded_string<>& string)
    {
        Expects(!string.empty());

        const auto index = details::find_baseimpl(string.base());
        const auto impl = details::_BaseTable[index];
        Expects(index > 0);

        return impl.decode(as_buffer(string.str()), impl.digits);
    }

    inline stringview_t decode(base_t base, stringview_t src, buffer_t& dst)
    {
        Expects(!src.empty());

        const auto index = details::find_baseimpl(base);
        const auto impl = details::_BaseTable[index];
        Expects(index > 0);

        auto pos = to_string(src).find_first_not_of(impl.digits);
        if (pos == string_t::npos) pos = src.size();

        dst += decode({ base, src.first(pos) });

        return src.last(src.size() - pos);
    }
    inline buffer_t decode(base_t base, stringview_t src)
    {
        auto dst = buffer_t{};
        decode(base, src, dst);
        return dst;
    }
    inline buffer_t decode(base_t base, const char* src)
    {
        return decode(base, gsl::ensure_z(src));
    }


    //
    // Self-identifying base-encoded string
    //
    template <base_t _Base = dynamic_base>
    using multibase = std::string;


    template <base_t _Base>
    multibase<_Base> encode_multibase(bufferview_t data) {
        constexpr auto _Index = details::find_baseimpl(_Base);
        constexpr auto _Impl = details::_BaseTable[_Index];

        static_assert(_Base != dynamic_base, "make_multibase<_Base>(bufferview_t) is not allowed for _Base == basecode::dynamic_base");
        static_assert(_Index > 0, "make_multibase<_Base>(bufferview_t) is not implementeed for this _Base");

        return _Impl.code + encode<_Base>(data).str();
    }

    inline multibase<> encode_multibase(base_t base, bufferview_t data) {
        const auto index = details::find_baseimpl(base);
        const auto impl = details::_BaseTable[index];
        Expects(index > 0);
        return impl.code + encode(base, data).str();
    }

    template <base_t _Base>
    multibase<_Base> encode_multibase(const std::string& string)                { return encode_multibase<_Base>(as_buffer(string)); }
    inline multibase<> encode_multibase(base_t code, const std::string& string) { return encode_multibase(code, as_buffer(string)); }


    template <base_t _Base>
    encoded_string<_Base> decode_multibase(multibase<_Base> mb)        { return mb.substr(1); }
    inline encoded_string<> decode_multibase(const std::string& mb)    { return { details::find_basecode(mb[0]), mb.substr(1) }; }
}

inline multiformats::encoded_string<multiformats::base16> operator "" _16(const char* s, std::size_t) 
{ return multiformats::encoded_string<multiformats::base16>(s); }

inline multiformats::encoded_string<multiformats::base64url> operator "" _64url(const char* s, std::size_t)
{
    return multiformats::encoded_string<multiformats::base64url>(s);
}
