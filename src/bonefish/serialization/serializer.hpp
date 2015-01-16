#ifndef BONEFISH_SERIALIZATION_SERIALIZER_HPP
#define BONEFISH_SERIALIZATION_SERIALIZER_HPP

#include <cstdint>

namespace bonefish
{

class wamp_message;

class serializer
{
public:
    serializer();
    virtual ~serializer();

    virtual wamp_message* deserialize(const char* buffer, size_t length) = 0;
    virtual size_t serialize(const wamp_message* message, char* buffer, size_t length) = 0;
};

inline serializer::serializer()
{
}

inline serializer::~serializer()
{
}

} // namespace bonefish

#endif // BONEFISH_SERIALIZATION_SERIALIZER_HPP
