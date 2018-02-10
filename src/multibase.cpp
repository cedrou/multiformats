#include "multiformats/multibase.h"
#include <map>
#include <string>
using namespace std::string_literals; // enables s-suffix for std::string literals 


using namespace multiformats;


buffer_t multiformats::details::encode_base0(bufferview_t data, const char* /*digits*/)
{
    Expects(!data.empty());
    return { std::begin(data), std::end(data) };
}
buffer_t multiformats::details::decode_base0(bufferview_t data, const char* /*digits*/)
{
    Expects(!data.empty());
    return { std::begin(data), std::end(data) };
}


buffer_t multiformats::details::encode_base2(bufferview_t data, const char* /*digits*/)
{
    const auto patterns = std::array<const char*, 16>{
        "0000", "0001", "0010", "0011", "0100", "0101", "0110", "0111",
        "1000", "1001", "1010", "1011", "1100", "1101", "1110", "1111",
    };

    const auto dataBits = data.size_bytes() * 8;
    auto encoded = buffer_t(dataBits, '0');
    auto encodedIndex = 0;

    for (auto value : data)
    {
        const auto left = value >> 4;
        const auto right = value & 0x0F;
        encoded[encodedIndex++] = patterns[left][0];
        encoded[encodedIndex++] = patterns[left][1];
        encoded[encodedIndex++] = patterns[left][2];
        encoded[encodedIndex++] = patterns[left][3];
        encoded[encodedIndex++] = patterns[right][0];
        encoded[encodedIndex++] = patterns[right][1];
        encoded[encodedIndex++] = patterns[right][2];
        encoded[encodedIndex++] = patterns[right][3];
    }

    return encoded;
}
buffer_t multiformats::details::decode_base2(bufferview_t data, const char* /*digits*/)
{
    Expects(!data.empty());
    
    const auto contentSize = data.size();
    const auto byteSize = (contentSize + 7) / 8;
    const auto leadSize = 8 * byteSize - contentSize;

    auto decoded = buffer_t(byteSize, 0);
    auto bitIndex = leadSize;
    for (auto byteIndex = 0; byteIndex < byteSize; byteIndex++)
    {
        auto byte = byte_t{ 0 };
        do {
            byte <<= 1;
            auto bit = gsl::narrow<byte_t>(data[bitIndex - leadSize] - '0');
            byte |= bit;
        } while (++bitIndex % 8);
            
        decoded[byteIndex] = byte;
    }

    return decoded;
}


buffer_t multiformats::details::encode_base8(bufferview_t data, const char* /*digits*/)
{
    const auto dataSize = std::size(data);
    const auto dataTotalBits = dataSize * 8;
    const auto baseBits = 3;

    const auto inputBlockSize = 3;
    const auto outputBlockSize = 8;

    const auto inputFullBlocks = dataSize / inputBlockSize;
    const auto inputTailChars = dataSize % inputBlockSize;

    auto encodedSize = inputFullBlocks * outputBlockSize;
    switch (inputTailChars)
    {
    case 1: encodedSize += 3; break;
    case 2: encodedSize += 6; break;
    }

    auto encoded = buffer_t(encodedSize, '0');
    auto encodedIndex = 0;
    auto dataIter = std::begin(data);

    if (inputTailChars == 1)
    {
        const auto b0 = 0;
        const auto b1 = 0;
        const auto b2 = static_cast<unsigned char>(*dataIter++);

        encoded[encodedIndex++] = '0' + (b2 >> 6);
        encoded[encodedIndex++] = '0' + ((b2 >> 3) & 0x07);
        encoded[encodedIndex++] = '0' + (b2 & 0x07);
    }
    else if (inputTailChars == 2)
    {
        const auto b0 = 0;
        const auto b1 = static_cast<unsigned char>(*dataIter++);
        const auto b2 = static_cast<unsigned char>(*dataIter++);

        encoded[encodedIndex++] = '0' + (b1 >> 7);
        encoded[encodedIndex++] = '0' + ((b1 >> 4) & 0x07);
        encoded[encodedIndex++] = '0' + ((b1 >> 1) & 0x07);
        encoded[encodedIndex++] = '0' + (((b1 & 0x01) << 2) + (b2 >> 6));
        encoded[encodedIndex++] = '0' + ((b2 >> 3) & 0x07);
        encoded[encodedIndex++] = '0' + (b2 & 0x07);
    }

    for (auto block = decltype(inputFullBlocks){0}; block < inputFullBlocks; block++)
    {
        // input   000000001111111122222222
        // output  000111222333444555666777
        const auto b0 = static_cast<unsigned char>(*dataIter++);
        const auto b1 = static_cast<unsigned char>(*dataIter++);
        const auto b2 = static_cast<unsigned char>(*dataIter++);

        encoded[encodedIndex++] = '0' + (b0 >> 5);
        encoded[encodedIndex++] = '0' + ((b0 >> 2) & 0x07);
        encoded[encodedIndex++] = '0' + (((b0 & 0x03) << 1) + (b1 >> 7));
        encoded[encodedIndex++] = '0' + ((b1 >> 4) & 0x07);
        encoded[encodedIndex++] = '0' + ((b1 >> 1) & 0x07);
        encoded[encodedIndex++] = '0' + (((b1 & 0x01) << 2) + (b2 >> 6));
        encoded[encodedIndex++] = '0' + ((b2 >> 3) & 0x07);
        encoded[encodedIndex++] = '0' + (b2 & 0x07);
    }

    return encoded;
}
buffer_t multiformats::details::decode_base8(bufferview_t data, const char* /*digits*/)
{
    Expects(!data.empty());

    const auto inputBlockSize = 8;
    const auto outputBlockSize = 3;

    const auto inputSize = data.size();
    
    const auto inputBlocks = (inputSize + inputBlockSize - 1) / inputBlockSize;
    const auto charsInFirstBlock = inputSize % inputBlockSize;

    auto decoded = buffer_t{};

    // input   000111222333444555666777
    // output  000000001111111122222222
    auto inputBlock = buffer_t(inputBlockSize, 0);
    //auto outputBlock = buffer_t(outputBlockSize, 0);
     
    auto inputIndex = (inputBlockSize - charsInFirstBlock) % inputBlockSize;
    auto skipCharsInOutput = 0;
    switch (charsInFirstBlock)
    {
    case 0: skipCharsInOutput = 0; break;
    case 1: skipCharsInOutput = 2; break;
    case 2: skipCharsInOutput = 2; break;
    case 3: skipCharsInOutput = data[0] & 0x4 ? 1 : 2; break;
    case 4: skipCharsInOutput = 1; break;
    case 5: skipCharsInOutput = 1; break;
    case 6: skipCharsInOutput = data[0] & 0x6 ? 0 : 1; break;
    case 7: skipCharsInOutput = 0; break;
    }

    auto inputIt = std::begin(data);
    for (auto block = 0; block < inputBlocks; block++)
    {
        while (inputIndex < inputBlockSize)
            inputBlock[inputIndex++] = *inputIt++ - '0';

        const auto o0 = static_cast<byte_t>((inputBlock[0] << 5) + (inputBlock[1] << 2) + (inputBlock[2] >> 1));
        const auto o1 = static_cast<byte_t>((inputBlock[2] << 7) + (inputBlock[3] << 4) + (inputBlock[4] << 1) + (inputBlock[5] >> 2));
        const auto o2 = static_cast<byte_t>((inputBlock[5] << 6) + (inputBlock[6] << 3) + inputBlock[7]);

        if (skipCharsInOutput == 0) {
            decoded.push_back(o0);
            decoded.push_back(o1);
            decoded.push_back(o2);
        }
        else if (skipCharsInOutput == 1) {
            decoded.push_back(o1);
            decoded.push_back(o2);
        }
        else if (skipCharsInOutput == 2) {
            decoded.push_back(o2);
        }

        inputIndex = 0;
        skipCharsInOutput = 0;
    }

    return decoded;
}

buffer_t multiformats::details::encode_base16(bufferview_t data, const char* digits)
{
    const auto dataTotalBits = std::size(data) * 8;
    const auto baseBits = 4;

    const auto encodedSize = dataTotalBits / baseBits;

    auto encoded = buffer_t(encodedSize, '0');
    auto encodedIndex = 0;

    auto dataIter = std::begin(data);
    while (dataIter != std::end(data))
    {
        // input   00000000
        // output  00001111
        const auto b0 = static_cast<unsigned char>(*dataIter++);

        encoded[encodedIndex++] = digits[b0 >> 4];
        encoded[encodedIndex++] = digits[b0 & 0x0F];
    }

    return encoded;
}
buffer_t multiformats::details::decode_base16(bufferview_t data, const char* digits)
{
    auto decoded = buffer_t{};
    
    auto sample = std::array<byte_t, 2>();
    auto sampleIndex = 0;

    if (data.size() % 2) sample[sampleIndex++] = '0';

    const auto begin = digits;
    const auto end = digits + 16;

    auto dataIt = std::begin(data);
    while (dataIt != std::end(data))
    {
        sample[sampleIndex++] = *dataIt++;
        if (sampleIndex == sample.size()) {
            sampleIndex = 0;

            const auto a0 = details::from_digit(sample[0], begin, end);
            const auto a1 = details::from_digit(sample[1], begin, end);

            decoded.push_back((a0 << 4) + a1);
        }
    }

    return decoded;
}

buffer_t multiformats::details::encode_base32(bufferview_t data, const char* digits)
{
    const auto dataSize = std::size(data);
    const auto dataTotalBits = dataSize * 8;
    const auto usePad = digits[32] == '=';

    const auto inputBlockSize = 5;
    const auto outputBlockSize = 8;

    const auto inputFullBlocks = dataSize / inputBlockSize;
    const auto inputTailChars = dataSize % inputBlockSize;

    auto encodedSize = inputFullBlocks * outputBlockSize;
    if (inputTailChars)
    {
        if (usePad)
        {
            encodedSize += outputBlockSize;
        }
        else
        {
            switch (inputTailChars)
            {
            case 1: encodedSize += 2; break;
            case 2: encodedSize += 4; break;
            case 3: encodedSize += 5; break;
            case 4: encodedSize += 7; break;
            }
        }
    }

    auto encoded = buffer_t(encodedSize, '0');
    auto encodedIndex = 0;
    auto dataIter = std::begin(data);
    for (auto block = decltype(inputFullBlocks){0}; block < inputFullBlocks; block++)
    {
        // input   0000000011111111222222223333333344444444
        // output  0000011111222223333344444555556666677777
        const auto b0 = static_cast<unsigned char>(*dataIter++);
        const auto b1 = static_cast<unsigned char>(*dataIter++);
        const auto b2 = static_cast<unsigned char>(*dataIter++);
        const auto b3 = static_cast<unsigned char>(*dataIter++);
        const auto b4 = static_cast<unsigned char>(*dataIter++);

        encoded[encodedIndex++] = digits[b0 >> 3]; // 8-5
        encoded[encodedIndex++] = digits[((b0 & 0x07) << 2) + (b1 >> 6)];
        encoded[encodedIndex++] = digits[(b1 >> 1) & 0x1F];
        encoded[encodedIndex++] = digits[((b1 & 0x01) << 4) + (b2 >> 4)];
        encoded[encodedIndex++] = digits[((b2 & 0x0F) << 1) + (b3 >> 7)];
        encoded[encodedIndex++] = digits[(b3 >> 2) & 0x1F];
        encoded[encodedIndex++] = digits[((b3 & 0x03) << 3) + (b4 >> 5)];
        encoded[encodedIndex++] = digits[b4 & 0x1F];
    }

    if (inputTailChars == 1)
    {
        const auto b0 = static_cast<unsigned char>(*dataIter++);

        encoded[encodedIndex++] = digits[b0 >> 3]; // 8-5
        encoded[encodedIndex++] = digits[((b0 & 0x07) << 2)];
        if (usePad)
        {
            encoded[encodedIndex++] = '=';
            encoded[encodedIndex++] = '=';
            encoded[encodedIndex++] = '=';
            encoded[encodedIndex++] = '=';
            encoded[encodedIndex++] = '=';
            encoded[encodedIndex++] = '=';
        }
    }
    else if (inputTailChars == 2)
    {
        const auto b0 = static_cast<unsigned char>(*dataIter++);
        const auto b1 = static_cast<unsigned char>(*dataIter++);

        encoded[encodedIndex++] = digits[b0 >> 3];
        encoded[encodedIndex++] = digits[((b0 & 0x07) << 2) + (b1 >> 6)];
        encoded[encodedIndex++] = digits[(b1 >> 1) & 0x1F];
        encoded[encodedIndex++] = digits[((b1 & 0x01) << 4)];
        if (usePad)
        {
            encoded[encodedIndex++] = '=';
            encoded[encodedIndex++] = '=';
            encoded[encodedIndex++] = '=';
            encoded[encodedIndex++] = '=';
        }
    }
    else if (inputTailChars == 3)
    {
        const auto b0 = static_cast<unsigned char>(*dataIter++);
        const auto b1 = static_cast<unsigned char>(*dataIter++);
        const auto b2 = static_cast<unsigned char>(*dataIter++);

        encoded[encodedIndex++] = digits[b0 >> 3];
        encoded[encodedIndex++] = digits[((b0 & 0x07) << 2) + (b1 >> 6)];
        encoded[encodedIndex++] = digits[(b1 >> 1) & 0x1F];
        encoded[encodedIndex++] = digits[((b1 & 0x01) << 4) + (b2 >> 4)];
        encoded[encodedIndex++] = digits[((b2 & 0x0F) << 1)];
        if (usePad)
        {
            encoded[encodedIndex++] = '=';
            encoded[encodedIndex++] = '=';
            encoded[encodedIndex++] = '=';
        }
    }
    else if (inputTailChars == 4)
    {
        const auto b0 = static_cast<unsigned char>(*dataIter++);
        const auto b1 = static_cast<unsigned char>(*dataIter++);
        const auto b2 = static_cast<unsigned char>(*dataIter++);
        const auto b3 = static_cast<unsigned char>(*dataIter++);

        encoded[encodedIndex++] = digits[b0 >> 3];
        encoded[encodedIndex++] = digits[((b0 & 0x07) << 2) + (b1 >> 6)];
        encoded[encodedIndex++] = digits[(b1 >> 1) & 0x1F];
        encoded[encodedIndex++] = digits[((b1 & 0x01) << 4) + (b2 >> 4)];
        encoded[encodedIndex++] = digits[((b2 & 0x0F) << 1) + (b3 >> 7)];
        encoded[encodedIndex++] = digits[(b3 >> 2) & 0x1F];
        encoded[encodedIndex++] = digits[((b3 & 0x03) << 3)];
        if (usePad)
        {
            encoded[encodedIndex++] = '=';
        }
    }

    return encoded;
}
buffer_t multiformats::details::decode_base32(bufferview_t data, const char* digits)
{
    auto decoded = buffer_t{};

    // input  0000011111222223333344444555556666677777
    // output 0000000011111111222222223333333344444444
    auto sample = std::array<byte_t, 8>();
    auto sampleIndex = 0;

    const auto begin = digits;
    const auto end = digits + 32;

    auto dataIt = std::begin(data);
    while (dataIt != std::end(data))
    {
        auto a = *dataIt++;
        if (a == '=') break;

        sample[sampleIndex++] = a;
        if (sampleIndex == sample.size()) {
            sampleIndex = 0;

            for (auto& s : sample) {
                s = details::from_digit(s, begin, end);
            }

            decoded.push_back((sample[0] << 3) + (sample[1] >> 2));
            decoded.push_back((sample[1] << 6) + (sample[2] << 1) + (sample[3] >> 4));
            decoded.push_back((sample[3] << 4) + (sample[4] >> 1));
            decoded.push_back((sample[4] << 7) + (sample[5] << 2) + (sample[6] >> 3));
            decoded.push_back((sample[6] << 5) + sample[7]);
        }
    }

    if (sampleIndex) {
        for (auto i = 0; i < sampleIndex; i++) {
            sample[i] = details::from_digit(sample[i], begin, end);
        }

        switch (sampleIndex) {
        case 1:
            decoded.push_back(sample[0] << 3);
            break;
        case 2:
            decoded.push_back((sample[0] << 3) + (sample[1] >> 2));
            if (byte_t(sample[1] << 6)) decoded.push_back(sample[1] << 6);
            break;
        case 3:
            decoded.push_back((sample[0] << 3) + (sample[1] >> 2));
            decoded.push_back((sample[1] << 6) + (sample[2] << 1));
            break;
        case 4:
            decoded.push_back((sample[0] << 3) + (sample[1] >> 2));
            decoded.push_back((sample[1] << 6) + (sample[2] << 1) + (sample[3] >> 4));
            if (byte_t(sample[3] << 4)) decoded.push_back(sample[3] << 4);
            break;
        case 5:
            decoded.push_back((sample[0] << 3) + (sample[1] >> 2));
            decoded.push_back((sample[1] << 6) + (sample[2] << 1) + (sample[3] >> 4));
            decoded.push_back((sample[3] << 4) + (sample[4] >> 1));
            if (byte_t(sample[4] << 7)) decoded.push_back(sample[4] << 7);
            break;
        case 6:
            decoded.push_back((sample[0] << 3) + (sample[1] >> 2));
            decoded.push_back((sample[1] << 6) + (sample[2] << 1) + (sample[3] >> 4));
            decoded.push_back((sample[3] << 4) + (sample[4] >> 1));
            decoded.push_back((sample[4] << 7) + (sample[5] << 2));
            break;
        case 7:
            decoded.push_back((sample[0] << 3) + (sample[1] >> 2));
            decoded.push_back((sample[1] << 6) + (sample[2] << 1) + (sample[3] >> 4));
            decoded.push_back((sample[3] << 4) + (sample[4] >> 1));
            decoded.push_back((sample[4] << 7) + (sample[5] << 2) + (sample[6] >> 3));
            if (byte_t(sample[6] << 5)) decoded.push_back(sample[6] << 5);
        }
    }

    return decoded;
}

buffer_t multiformats::details::encode_base64(bufferview_t data, const char* digits)
{
    const auto dataSize = std::size(data);
    const auto dataTotalBits = dataSize * 8;
    const auto usePad = digits[64] == '=';

    const auto inputFullBlocks = dataSize / 3;
    const auto inputTailChars = dataSize % 3;

    const auto encodedSize = inputFullBlocks * 4 + (inputTailChars ? (usePad ? 4 : 1 + inputTailChars) : 0);

    auto encoded = buffer_t(encodedSize, '0');
    auto encodedIndex = 0;
    auto dataIter = std::begin(data);
    for (auto block = decltype(inputFullBlocks){0}; block < inputFullBlocks; block++)
    {
        // input   000000001111111122222222
        // output  000000111111222222333333
        const auto b0 = static_cast<unsigned char>(*dataIter++);
        const auto b1 = static_cast<unsigned char>(*dataIter++);
        const auto b2 = static_cast<unsigned char>(*dataIter++);

        encoded[encodedIndex++] = digits[b0 >> 2];
        encoded[encodedIndex++] = digits[((b0 & 0x03) << 4) + (b1 >> 4)];
        encoded[encodedIndex++] = digits[((b1 & 0x0F) << 2) + (b2 >> 6)];
        encoded[encodedIndex++] = digits[b2 & 0x3F];
    }

    if (inputTailChars == 1)
    {
        const auto b0 = static_cast<unsigned char>(*dataIter++);

        encoded[encodedIndex++] = digits[b0 >> 2];
        encoded[encodedIndex++] = digits[(b0 & 0x03) << 4];
        if (usePad)
        {
            encoded[encodedIndex++] = '=';
            encoded[encodedIndex++] = '=';
        }
    }
    else if (inputTailChars == 2)
    {
        const auto b0 = static_cast<unsigned char>(*dataIter++);
        const auto b1 = static_cast<unsigned char>(*dataIter++);

        encoded[encodedIndex++] = digits[b0 >> 2];
        encoded[encodedIndex++] = digits[((b0 & 0x03) << 4) + (b1 >> 4)];
        encoded[encodedIndex++] = digits[(b1 & 0x0F) << 2];
        if (usePad)
        {
            encoded[encodedIndex++] = '=';
        }
    }

    return encoded;
}
buffer_t multiformats::details::decode_base64(bufferview_t data, const char* digits)
{
    auto decoded = buffer_t{};

    // input  000000111111222222333333
    // output 000000001111111122222222
    auto sample = std::array<byte_t, 4>();
    auto sampleIndex = 0;

    const auto begin = digits;
    const auto end = digits + 64;

    auto dataIt = std::begin(data);
    while (dataIt != std::end(data))
    {
        auto a = *dataIt++;
        if (a == '=') break;

        sample[sampleIndex++] = a;
        if (sampleIndex == sample.size()) {
            sampleIndex = 0;

            for (auto& s : sample) {
                s = details::from_digit(s, begin, end);
            }

            decoded.push_back((sample[0] << 2) + (sample[1] >> 4));
            decoded.push_back((sample[1] << 4) + (sample[2] >> 2));
            decoded.push_back((sample[2] << 6) + sample[3]);
        }
    }

    if (sampleIndex) {
        for (auto i = 0; i < sampleIndex; i++) {
            sample[i] = details::from_digit(sample[i], begin, end);
        }

        switch (sampleIndex) {
        case 1:
            decoded.push_back(sample[0] << 2);
            break;
        case 2:
            decoded.push_back((sample[0] << 2) + (sample[1] >> 4));
            if (byte_t(sample[1] << 4)) decoded.push_back(sample[1] << 4);
            break;
        case 3:
            decoded.push_back((sample[0] << 2) + (sample[1] >> 4));
            decoded.push_back((sample[1] << 4) + (sample[2] >> 2));
            if (byte_t(sample[2] << 6)) decoded.push_back(sample[2] << 6);
            break;
        }
    }

    return decoded;
}
