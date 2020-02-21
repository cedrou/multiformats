#pragma once

#include <gsl/gsl>
#include <vector>
#include <sstream>

namespace multiformats {

    //
    // Raw memory byte
    //
    using byte_t = uint8_t;

    //
    // Raw memory buffer
    //   Represents a contiguous run of byte_t elements, or a const view of it.
    //
    using buffer_t = std::vector<byte_t>;
    using bufferview_t = gsl::span<const byte_t>;

    //    Appends a buffer or a byte to a buffer
    inline buffer_t& operator += (buffer_t& _Left, bufferview_t _Right) {
        _Left.insert(_Left.end(), _Right.begin(), _Right.end());
        return _Left;
    }
    inline buffer_t& operator += (buffer_t& _Left, byte_t _Value) {
        _Left.push_back(_Value);
        return _Left;
    }

    //    Concatenates two buffers
    inline buffer_t operator + (bufferview_t _Left, bufferview_t _Right) {
        auto result = buffer_t(_Left.size() + _Right.size());
        std::copy(_Right.begin(), _Right.end(), std::copy(_Left.begin(), _Left.end(), result.begin()));
        return result;
    }

    //    Buffer comparison
    inline bool operator == (bufferview_t _Left, bufferview_t _Right) {
        return std::equal(_Left.begin(), _Left.end(), _Right.begin(), _Right.end());
    }

    inline bool operator != (bufferview_t _Left, bufferview_t _Right) { 
        return !(_Left == _Right);
    }


    //
    // String and constant view of strings
    //
    using string_t = std::string;
    using stringview_t = gsl::cstring_span<>;

    inline std::vector<stringview_t> split(stringview_t s, char delim) {
        auto elems = std::vector<stringview_t>{};

        auto begin = s.begin();
        auto end = s.end();
        auto first = begin;
        while (first != end) {
            auto last = std::find(first, end, delim);
            elems.push_back(s.subspan(first - begin, last - first));
            if (last == end) break;
            first = last + 1;
        }

        return elems;
    }

    inline string_t operator+(stringview_t _Left, const string_t& _Right) { return (to_string(_Left) + _Right); }
    inline string_t operator+(const string_t& _Left, stringview_t _Right) { return (_Left + to_string(_Right)); }

    // 
    // Conversions
    //   to_* methods create a new container and copy the content
    //   as_* methods return a const view of the container without copy
    //
    inline buffer_t to_buffer(stringview_t s) { return { s.begin(), s.end() }; }
    inline buffer_t to_buffer(const char* s)  { return to_buffer(gsl::ensure_z(s)); }

    inline bufferview_t as_buffer(stringview_t s) { return { reinterpret_cast<const byte_t*>(s.data()), gsl::narrow<ptrdiff_t>(s.size()) }; }
    inline stringview_t as_string(bufferview_t b) { return { reinterpret_cast<const char*>(b.data()), b.size() }; }
    
    inline string_t to_string(bufferview_t b) { return to_string(as_string(b)); }
}