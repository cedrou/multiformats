#pragma once

#include "common.h"
#include "multibase.h"
#include "uvarint.h"
#include <gsl/gsl>
#include <map>


namespace multiformats {

    enum hash_t {
        dynamic_hash = -1,
        sha1,
        sha2_256
    };

    namespace details {

        struct hashimpl {
            hash_t      key;
            const char* name;
            uint32_t    code;
            int32_t     len;
        };

        constexpr hashimpl _HashTable[] = {
            { dynamic_hash, "dynamic_hash", 0x00,  0 },
            { sha1,         "sha1",         0x11, 20 },
            { sha2_256,     "sha2_256",     0x12, 32 },
        };

        constexpr int find_hashimpl_by_key(hash_t code) {
            for (auto i = 0; i < _countof(_HashTable); i++)
                if (_HashTable[i].key == code) return i;
            return 0;
        }

        constexpr int find_hashimpl_by_code(uint32_t code) {
            for (auto i = 0; i < _countof(_HashTable); i++)
                if (_HashTable[i].code == code) return i;
            return 0;
        }

        template <hash_t _Hash = dynamic_hash, int _Index = find_hashimpl_by_key(_Hash)>
        class hashcode_type
        {
        public:
            static_assert(_Hash != dynamic_hash, "dynamic_hash is not allowed here");
            static_assert(_Index > 0, "The hash_t is not implemented");

            constexpr hashcode_type(hash_t hash) { Expects(hash == _Hash); }

            constexpr hash_t      hash()  const { return _HashTable[_Index].key; }
            constexpr const char* name()  const { return _HashTable[_Index].name; }
            constexpr uint32_t    code()  const { return _HashTable[_Index].code; }
            constexpr int32_t     len()   const { return _HashTable[_Index].len; }
            constexpr int         index() const { return _Index; }
        };


        template <>
        class hashcode_type<dynamic_hash>
        {
        public:
            explicit constexpr hashcode_type(hash_t hash) : _index(find_hashimpl_by_key(hash)) { Expects(_index > 0); }

            const hash_t      hash()  const { return _HashTable[_index].key; }
            const char*       name()  const { return _HashTable[_index].name; }
            const uint32_t    code()  const { return _HashTable[_index].code; }
            const int32_t     len()   const { return _HashTable[_index].len; }
            const int         index() const { return _index; }
        private:
            const int _index;
        };

    }


    // A strongly typed byte buffer that contains an immutable representation of the output digest for a specific hash algorithm
    // known at compile-time.
    // The specialization with dynamic_hash is used when the hash function is not known at compile-time.

    template <hash_t _Hash = dynamic_hash>
    class digest_buffer
    {
    public:
        // Construct empty
        digest_buffer() : _hash(_Hash) {}
        digest_buffer(hash_t hash) : _hash(hash) {}

        // Construct by copying _Right
        //    - from bufferview_t
        digest_buffer(bufferview_t _Right) : _buffer(std::begin(_Right), std::end(_Right)), _hash(_Hash)
        {
            static_assert(_Hash != dynamic_hash, "dynamic_hash is not allowed here");
            Expects(!_Right.empty());
            Expects(_hash.len() < 0 || _Right.size() == _hash.len());
        }
        digest_buffer(hash_t hash, bufferview_t _Right) : _buffer(std::begin(_Right), std::end(_Right)), _hash(hash)
        {
            Expects(!_Right.empty());
            Expects(_hash.len() < 0 || _Right.size() == _hash.len());
        }

        //    - from digest_buffer<>
        digest_buffer(const digest_buffer<_Hash>& _Right) : _buffer(_Right._buffer), _hash(_Right.hash())
        {
            Expects(!_Right.empty());
        }

        template <hash_t _RightHash>
        digest_buffer(const digest_buffer<_RightHash>& _Right) : _buffer(_Right._buffer), _hash(_Right.hash())
        {
            Expects(!_Right.empty());
        }

        // Construct by moving _Right
        digest_buffer(buffer_t&& _Right) : _buffer(std::move(_Right)), _hash(_Hash) 
        {
            static_assert(_Hash != dynamic_hash, "dynamic_hash is not allowed here");
        }
        digest_buffer(hash_t hash, buffer_t&& _Right) : _buffer(std::move(_Right)), _hash(hash) {}

        digest_buffer(digest_buffer<_Hash>&& _Right) : _buffer(std::move(_Right._buffer)), _hash(_Right.hash()) {}
        template <hash_t _RightHash>
        digest_buffer(digest_buffer<_RightHash>&& _Right) : _string(std::move(_Right._buffer)), _hash(_Right.hash()) {}


        // Assign by copying _Right
        // Note: an digest_buffer<> is immutable once initialized
        digest_buffer<_Hash>& operator =(bufferview_t _Right)
        {
            Expects(_buffer.empty() && !_Right.empty());
            _buffer = _Right;
            return *this;
        }

        digest_buffer<_Hash>& operator =(const digest_buffer<_Hash>& _Right)
        {
            Expects(_Right.hash() == hash());
            Expects(_buffer.empty() && !_Right.empty());
            _buffer = _Right._buffer;
            return *this;
        }

        template <hash_t _RightHash>
        digest_buffer<_Hash>& operator =(const digest_buffer<_RightHash>& _Right)
        {
            static_assert(_RightHash == _Hash || _RightHash == dynamic_hash || _Hash == dynamic_hash, "Mismatch between codes.");
            Expects(_Right.hash() == hash());
            Expects(_buffer.empty() && !_Right.empty());
            _buffer = _Right._buffer;
            return *this;
        }

        // Assign by moving _Right
        // Note: an digest_buffer<> is immutable once initialized and cannot be emptied
        digest_buffer<_Hash>& operator =(bufferview_t&& _Right)
        {
            Expects(_buffer.empty() && !_Right.empty());
            _buffer = std::move(_Right);
            return *this;
        }

        digest_buffer<_Hash>& operator =(digest_buffer<_Hash>&& _Right)
        {
            Expects(_Right.hash() == hash());
            Expects(_buffer.empty() && !_Right.empty());
            _buffer = std::move(_Right._buffer);
            return *this;
        }

        template <hash_t _RightHash>
        digest_buffer<_Hash>& operator =(digest_buffer<_RightHash>&& _Right)
        {
            static_assert(_RightHash == _Hash || _RightHash == dynamic_hash || _Hash == dynamic_hash, "Mismatch between codes.");
            Expects(_Right.hash() == hash());
            Expects(_buffer.empty() && !_Right.empty());
            _buffer = std::move(_Right._buffer);
            return *this;
        }


        auto hash()  const { return _hash.hash(); }
        auto data()  const { return _buffer; }
        auto code()  const { return _hash.code(); }

        auto empty() const { return _buffer.empty(); }
        auto size()  const { return _buffer.size(); }
        auto begin() const { return _buffer.begin(); }
        auto end()   const { return _buffer.end(); }

    private:
        const details::hashcode_type<_Hash> _hash;
        buffer_t _buffer;

        template <hash_t _FriendHash> friend class digest_buffer;
    };

    // operator==
    template <hash_t _HashLeft, hash_t _HashRight>
    bool operator==(const digest_buffer<_HashLeft>& _Left, const digest_buffer<_HashRight>& _Right)
    {
        return (_Left.hash() == _Right.hash()) && (_Left.data() == _Right.data());
    }


    // A string that contains an encoded string in base _Base of a digest with algo _Hash
    template <hash_t _Hash = dynamic_hash, base_t _Base = dynamic_base>
    class digest_string : public encoded_string<_Base>
    {
        using string_t = encoded_string<_Base>;

    public:
        // Construct empty
        digest_string() : string_t(), _hash(_Hash) {}
        digest_string(hash_t hash) : string_t(), _hash(hash) {}
        digest_string(base_t base) : string_t(base), _hash(_Hash) {}
        digest_string(hash_t hash, base_t base) : string_t(base), _hash(hash) {}

        // Construct by copying _Right
        //    - from string
        digest_string(stringview_t _Right) : string_t(_Right), _hash(_Hash) 
        {
            static_assert(_Hash != dynamic_hash, "dynamic_hash is not allowed here");
            Expects(!_Right.empty());
            Expects(_hash.len() < 0 || decode(this->base(), _Right).size() == _hash.len());
        }
        digest_string(hash_t hash, stringview_t _Right) : string_t(_Right), _hash(hash)
        {
            Expects(!_Right.empty());
            Expects(_hash.len() < 0 || decode(this->base(), _Right).size() == _hash.len());
        }
        digest_string(base_t base, stringview_t _Right) : string_t(base, _Right), _hash(_Hash)
        {
            static_assert(_Hash != dynamic_hash, "dynamic_hash is not allowed here");
            Expects(!_Right.empty());
            Expects(_hash.len() < 0 || decode(this->base(), _Right).size() == _hash.len());
        }
        digest_string(hash_t hash, base_t base, stringview_t _Right) : string_t(base, _Right), _hash(hash)
        {
            Expects(!_Right.empty());
            Expects(_hash.len() < 0 || decode(this->base(), _Right).size() == _hash.len());
        }

        digest_string(const char* _Right) : digest_string(gsl::ensure_z(_Right)) {}
        digest_string(hash_t hash, const char* _Right) : digest_string(hash, gsl::ensure_z(_Right)) {}
        digest_string(base_t base, const char* _Right) : digest_string(base, gsl::ensure_z(_Right)) {}
        digest_string(hash_t hash, base_t base, const char* _Right) : digest_string(hash, base, gsl::ensure_z(_Right)) {}

        //    - from encoded_string<>
        digest_string(const encoded_string<_Base>& _Right) : string_t(_Right), _hash(_Hash)
        {
            static_assert(_Hash != dynamic_hash, "dynamic_hash is not allowed here");
            Expects(!_Right.empty());
            Expects(_hash.len() < 0 || decode(_Right).size() == _hash.len());
        }
        digest_string(hash_t hash, const encoded_string<_Base>& _Right) : string_t(_Right), _hash(hash)
        {
            Expects(!_Right.empty());
            Expects(_hash.len() < 0 || decode(_Right).size() == _hash.len());
        }

        template <base_t _RightBase>
        digest_string(const encoded_string<_RightBase>& _Right) : string_t(_Right), _hash(_Hash)
        {
            static_assert(_RightBase == _Base || _RightBase == dynamic_base || _Base == dynamic_base, "Mismatch between codes.");
            static_assert(_Hash != dynamic_hash, "dynamic_hash is not allowed here");
            Expects(!_Right.empty());
            Expects(_hash.len() < 0 || decode(_Right).size() == _hash.len());
        }
        template <base_t _RightBase>
        digest_string(hash_t hash, const encoded_string<_RightBase>& _Right) : string_t(_Right), _hash(hash)
        {
            static_assert(_RightBase == _Base || _RightBase == dynamic_base || _Base == dynamic_base, "Mismatch between codes.");
            Expects(!_Right.empty());
            Expects(_hash.len() < 0 || decode(_Right).size() == _hash.len());
        }

        //    - from digest_string<>
        digest_string(const digest_string<_Hash, _Base>& _Right) : string_t(_Right), _hash(_Right.hash())
        {
            Expects(!_Right.empty());
        }
        template <hash_t _RightHash>
        digest_string(const digest_string<_RightHash, _Base>& _Right) : string_t(_Right), _hash(_Right.hash())
        {
            static_assert(_RightHash == _Hash || _RightHash == dynamic_hash || _Hash == dynamic_hash, "Mismatch between codes.");
            Expects(!_Right.empty());
        }
        template <base_t _RightBase>
        digest_string(const digest_string<_Hash, _RightBase>& _Right) : string_t(_Right), _hash(_Right.hash())
        {
            static_assert(_RightBase == _Base || _RightBase == dynamic_base || _Base == dynamic_base, "Mismatch between codes.");
            Expects(!_Right.empty());
        }
        template <hash_t _RightHash, base_t _RightBase>
        digest_string(const digest_string<_RightHash, _RightBase>& _Right) : string_t(_Right), _hash(_Right.hash())
        {
            static_assert(_RightHash == _Hash || _RightHash == dynamic_hash || _Hash == dynamic_hash, "Mismatch between codes.");
            static_assert(_RightBase == _Base || _RightBase == dynamic_base || _Base == dynamic_base, "Mismatch between codes.");
            Expects(!_Right.empty());
        }


        // Assign by copying _Right
        // Note: an digest_string<> is immutable once initialized
        digest_string<_Hash, _Base>& operator =(stringview_t _Right)
        {
            Expects(_hash.len() < 0 || decode(this->base(), _Right).size() == _hash.len());
            *((string_t*)this) = _Right;
            return *this;
        }
        digest_string<_Hash, _Base>& operator =(const char* _Right)
        {
            return operator=(gsl::ensure_z(_Right));
        }

        digest_string<_Hash, _Base>& operator =(const encoded_string<_Base>& _Right)
        {
            Expects(_hash.len() < 0 || decode(_Right).size() == _hash.len());
            *((string_t*)this) = _Right;
            return *this;
        }
        template <base_t _RightBase>
        digest_string<_Hash, _Base>& operator =(const encoded_string<_RightBase>& _Right)
        {
            static_assert(_RightBase == _Base || _RightBase == dynamic_base || _Base == dynamic_base, "Mismatch between codes.");
            Expects(_hash.len() < 0 || decode(_Right).size() == _hash.len());
            *((string_t*)this) = _Right;
            return *this;
        }

        digest_string<_Hash, _Base>& operator =(const digest_string<_Hash, _Base>& _Right)
        {
            Expects(_Right.hash() == hash());
            *this = _Right.str();
            return *this;
        }
        template <hash_t _RightHash, base_t _RightBase>
        digest_string<_Hash, _Base>& operator =(const digest_string<_RightHash, _RightBase>& _Right)
        {
            static_assert(_RightHash == _Hash || _RightHash == dynamic_hash || _Hash == dynamic_hash, "Mismatch between codes.");
            static_assert(_RightBase == _Base || _RightBase == dynamic_base || _Base == dynamic_base, "Mismatch between codes.");
            Expects(_Right.hash() == hash());
            *this = _Right.str();
            return *this;
        }

        hash_t hash() const { return _hash.hash(); }

    private:
        const details::hashcode_type<_Hash> _hash;
    };

    //
    // Encode a digest into a _Base encoded_string
    //
    template <hash_t _Hash, base_t _Base>
    digest_string<_Hash, _Base> encode(const digest_buffer<_Hash>& digest)
    {
        Expects(!digest.empty());
        return encode<_Base>(digest);
    }

    template <hash_t _Hash>
    digest_string<_Hash> encode(base_t base, const digest_buffer<_Hash>& digest)
    {
        Expects(!digest.empty());
        return encode(base, digest);
    }

    inline digest_string<> encode(hash_t hash, base_t base, bufferview_t digest)
    {
        Expects(!digest.empty());
        return { hash, encode(base, digest) };
    }


    template <hash_t _Hash, base_t _Base>
    digest_buffer<_Hash> decode(digest_string<_Hash, _Base> digest)
    {
        return decode(static_cast<encoded_string<_Base>>(digest));
    }



    class multihash
    {
    public:
        multihash(bufferview_t mhview)
        {
            auto posIt = mhview.begin();

            // parse and check hash code
            auto code = uint32_t{};
            posIt = uvarint::decode(posIt, mhview.end(), &code);

            auto index = details::find_hashimpl_by_code(code);
            if (index == 0) throw std::invalid_argument("Failed to parse multihash buffer: wrong hash code");
            _hash = details::_HashTable[index].key;

            // parse and check size
            posIt = uvarint::decode(posIt, mhview.end(), &_size);

            if (static_cast<ptrdiff_t>(_size) != mhview.end() - posIt) throw std::invalid_argument("Failed to parse multihash buffer: wrong size");
            if (details::_HashTable[index].len > 0 && _size != details::_HashTable[index].len) throw std::invalid_argument("Failed to parse multihash buffer: wrong size");

            // Seems legit
            _data.assign(mhview.begin(), mhview.end());
        }

        hash_t       hash()   const { return _hash; }
        size_t       size()   const { return _size; }
        bufferview_t digest() const { return bufferview_t{ _data }.last(_size); }

        bufferview_t data()   const { return _data; }


    private:
        hash_t _hash;
        size_t _size;
        buffer_t _data;
    };

    // Create a multihash from a digest_buffer
    template <hash_t _Hash>
    multihash to_multihash(digest_buffer<_Hash> digest) {
        auto mh = buffer_t{};

        uvarint::encode(digest.code(), std::back_inserter(mh));
        uvarint::encode(digest.size(), std::back_inserter(mh));
        mh.insert(mh.end(), digest.begin(), digest.end());

        return { mh };
    }

    // Create a multihash from a digest_string
    template <hash_t _Hash, base_t _Base>
    multihash to_multihash(digest_string<_Hash, _Base> digest) { return to_multihash(decode(digest)); }

    // Create a multihash from a raw digest_string
    template <hash_t _Hash>
    multihash to_multihash(bufferview_t digest) { return to_multihash(digest_buffer<_Hash>{ digest }); }
    inline multihash to_multihash(hash_t hash, bufferview_t digest) { return to_multihash(digest_buffer<>{ hash, digest }); }
}
