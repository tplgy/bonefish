#ifndef BONEFISH_SERIALIZATION_BASE64_HPP
#define BONEFISH_SERIALIZATION_BASE64_HPP

#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>

namespace bonefish {
namespace base64 {

inline size_t decode_size(const char* base64_data, size_t length, size_t* src_padding_size = 0)
{
    if (length >= 1 && *(base64_data + length - 1) == '=') {
        if (length >= 2 && *(base64_data + length - 2) == '=') {
            if (src_padding_size) { *src_padding_size = 2; }
            return (length * 3 / 4 - 2);
        }
        if (src_padding_size) { *src_padding_size = 1; }
        return (length * 3 / 4 - 1);
    }
    if (src_padding_size) { *src_padding_size = 0; }
    return length * 3 / 4;
}

inline size_t encode_size(const char* base64_data, size_t length)
{
    (void) base64_data; // unused
    return (length + 2 - ((length + 2) % 3)) * 4 / 3; // round up to multiple of 3, then multiply
}

inline void decode(
        char* binary_result, size_t binary_length,
        const char* base64_data, size_t base64_length)
{
    using namespace boost::archive::iterators;
    using iterator = transform_width<binary_from_base64<const char*>, 8, 6>;

    size_t padding_size = 0;
    size_t calc_length = decode_size(base64_data, base64_length, &padding_size);
    if (binary_length < calc_length) {
        boost::serialization::throw_exception(
            boost::archive::iterators::dataflow_exception(dataflow_exception::invalid_conversion));
    }

    iterator end(base64_data + base64_length - padding_size);
    for (iterator it(base64_data); it != end; ++it) {
        *(binary_result++) = *it;
    }
}

inline void encode(
        char* base64_result, size_t base64_length,
        const char* binary_data, size_t binary_length)
{
    using namespace boost::archive::iterators;
    using iterator = base64_from_binary<transform_width<const char*, 6, 8>>;

    size_t calc_length = encode_size(binary_data, binary_length);
    if (base64_length < calc_length) {
        boost::serialization::throw_exception(
            boost::archive::iterators::dataflow_exception(dataflow_exception::invalid_conversion));
    }

    iterator end(binary_data + binary_length);
    for (iterator it(binary_data); it != end; ++it) {
        *(base64_result++) = *it;
    }

    switch (binary_length % 3) {
    case 1: *(base64_result++) = '='; // fall through
    case 2: *(base64_result++) = '='; // fall through
    default:
        if (base64_length > calc_length) {
            *base64_result = '\0';
        }
    }
}

} // namespace base64
} // namespace bonefish

#endif // BONEFISH_SERIALIZATION_BASE64_HPP
