#pragma once

#include "common.h"

namespace multiformats {
namespace uvarint {
namespace details {

    template <typename T, typename ValueType> struct Iterator_value_type          { typedef ValueType type; };
    template <typename T>                     struct Iterator_value_type<T, void> { typedef typename T::container_type::value_type type; };

    template <typename Iter> struct IteratorValueType        { typedef typename Iterator_value_type<Iter, typename Iter::value_type>::type type; };
    template <typename Iter> struct IteratorValueType<Iter*> { typedef Iter type; };
}


    const size_t max_varint_size = 9;
    const uint64_t max_value = 0x7FFF'FFFF'FFFF'FFFF;


    template <typename T, typename OutputIt>
    inline OutputIt encode(T value, OutputIt outIt)
    {
        using OutputType = typename details::IteratorValueType<OutputIt>::type;

        static_assert(std::is_integral<T>::value || std::is_enum<T>::value, "Integral type is required.");
        static_assert(sizeof(OutputType) == 1, "Output iterator value type must be 8 bits.");
        
        using uT = typename std::make_unsigned<T>::type;

        auto uvalue = static_cast<uT>(value);

        if (uvalue > max_value) throw std::out_of_range("value to encode to uvarint type is too large");
        
        auto out = buffer_t{};
        auto outIter = std::back_inserter(out);

        *outIter++ = gsl::narrow<OutputType>(uvalue & 0x7F);
        uvalue >>= 7;
        while (uvalue)
        {
            *outIter++ = gsl::narrow<OutputType>( (uvalue & 0x7F) | 0x80 );
            uvalue >>= 7;
        }

        //return std::copy(std::rbegin(out), std::rend(out), first);
        auto begin = std::rbegin(out);
        auto end = std::rend(out);
        for (auto it = begin; it != end; ++it)
        {
            *outIt++ = *it;
        }

        return outIt;
    }

    template <typename T>
    inline buffer_t encode(T value)
    {
        auto out = buffer_t{};

        encode(value, std::back_inserter(out));

        return out;
    }

    template <typename T, typename InputIt>
    inline InputIt decode(InputIt first, InputIt last, T* pOut)
    {
        auto out = uint64_t{ 0 };
        auto stop = false;
        while (!stop && first != last) {
            if (first == last) throw std::exception("Invalid input data");

            auto value = gsl::narrow<unsigned char>(*first++);
            stop = !(value & 0x80);

            out <<= 7;
            out += value & 0x7F;
        }

        *pOut = gsl::narrow<T>(out);
        return first;
    }

    template <typename T>
    inline bufferview_t decode(bufferview_t src, T* pOut) {
        auto it = decode(src.begin(), src.end(), pOut);
        return src.last(src.end() - it);
    }

    inline uint64_t decode(bufferview_t in)
    {
        auto out = uint64_t{ 0 };
        decode(std::begin(in), std::end(in), &out);
        return out;
    }

}
}
