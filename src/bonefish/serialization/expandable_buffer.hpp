/**
 *  Copyright (C) 2015 Topology LP
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef BONEFISH_SERIALIZATION_EXPANDABLE_BUFFER_HPP
#define BONEFISH_SERIALIZATION_EXPANDABLE_BUFFER_HPP

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <new>
#include <stdexcept>

namespace bonefish
{

class expandable_buffer
{
public:
    expandable_buffer(size_t initial_capacity);
    ~expandable_buffer();

    expandable_buffer(expandable_buffer&& other);
    expandable_buffer& operator=(expandable_buffer&& other);

    expandable_buffer(const expandable_buffer&) = delete;
    expandable_buffer& operator=(const expandable_buffer&) = delete;

    void write(const char c);
    void write(const char* buffer, size_t length);

    char* data();
    const char* data() const;

    size_t size() const;
    char* release();
    void reset();

private:
    void expand(size_t length);

private:
    size_t m_size;
    char* m_data;
    size_t m_capacity;
};


inline expandable_buffer::expandable_buffer(size_t initial_capacity)
    : m_size(0)
    , m_data(nullptr)
    , m_capacity(initial_capacity)
{
    if(initial_capacity > 0) {
        m_data = static_cast<char*>(std::malloc(initial_capacity));
        if(!m_data) {
            throw std::bad_alloc();
        }
    }
}

inline expandable_buffer::~expandable_buffer()
{
    reset();
}

inline expandable_buffer::expandable_buffer(expandable_buffer&& other)
    : m_size(other.m_size)
    , m_data(other.m_data)
    , m_capacity(other.m_capacity)
{
    other.m_size = 0;
    other.m_data = nullptr;
    other.m_capacity = 0;
}

inline expandable_buffer& expandable_buffer::operator=(expandable_buffer&& other)
{
    if (this == &other) {
        return *this;
    }

    reset();
    m_size = other.m_size;
    m_data = other.m_data;
    m_capacity = other.m_capacity;

    other.m_size = 0;
    other.m_data = nullptr;
    other.m_capacity = 0;

    return *this;
}

inline void expandable_buffer::write(const char c)
{
    if (m_capacity - m_size == 0) {
        expand(m_capacity);
    }
    m_data[m_size++] = c;
}

inline void expandable_buffer::write(const char* buffer, size_t length)
{
    if (m_capacity - m_size < length) {
        expand(length);
    }
    std::memcpy(m_data + m_size, buffer, length);
    m_size += length;
}

inline char* expandable_buffer::data()
{
    return m_data;
}

inline const char* expandable_buffer::data() const
{
    return m_data;
}

inline size_t expandable_buffer::size() const
{
    return m_size;
}

inline char* expandable_buffer::release()
{
    char* tmp = m_data;
    m_size = 0;
    m_data = nullptr;
    m_capacity = 0;
    return tmp;
}

inline void expandable_buffer::reset()
{
    if (m_data) {
        m_size = 0;
        m_capacity = 0;
        std::free(m_data);
        m_data = nullptr;
    }
}

inline void expandable_buffer::expand(size_t length)
{
    size_t new_capacity = std::max(m_size + length, m_capacity * 2);
    char* new_data = static_cast<char*>(std::realloc(m_data, new_capacity));
    if (!new_data) {
        throw std::bad_alloc();
    }

    m_data = new_data;
    m_capacity = new_capacity;
}

} // namespace bonefish

#endif // BONEFISH_SERIALIZATION_WAMP_SERIALIZER_HPP
