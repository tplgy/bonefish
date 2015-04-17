//
// Adapted from:
//
// MessagePack for C++ (release 1.0.1)
// Copyright (C) 2008-2009 FURUHASHI Sadayuki
// (Note jpetso: I'm sure there's some additional copyright but none as been
// added by upstream contributors. As long as it's all Apache, it's all good.)
//
// https://github.com/xpol/xchange, commit c823ecd6f1bf42a6469a00b297a770e46a23d8e1 (2015-03-27)
// Copyright (C) 2014-2015 Xpol Wan
//
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.
//
#ifndef BONEFISH_SERIALIZATION_JSON_MSGPACK_SAX_HPP
#define BONEFISH_SERIALIZATION_JSON_MSGPACK_SAX_HPP

#include <msgpack.hpp>
#include <stack>

namespace bonefish {
namespace serialization {

// Adapted from msgpack::detail::unpack_array_item(), include/msgpack/unpack.hpp.
inline void copy_array_item(msgpack::object& c, std::size_t index, msgpack::object const& o)
{
#if defined(__GNUC__) && !defined(__clang__)
    std::memcpy(&c.via.array.ptr[index], &o, sizeof(msgpack::object));
#else  /* __GNUC__ && !__clang__ */
    c.via.array.ptr[index] = o;
#endif /* __GNUC__ && !__clang__ */
}

// Adapted from msgpack::detail::unpack_map_item(), include/msgpack/unpack.hpp.
inline void copy_map_item(
        msgpack::object& c, std::size_t index, msgpack::object const& k, msgpack::object const& v)
{
#if defined(__GNUC__) && !defined(__clang__)
    std::memcpy(&c.via.map.ptr[index].key, &k, sizeof(msgpack::object));
    std::memcpy(&c.via.map.ptr[index].val, &v, sizeof(msgpack::object));
#else  /* __GNUC__ && !__clang__ */
    c.via.map.ptr[index].key = k;
    c.via.map.ptr[index].val = v;
#endif /* __GNUC__ && !__clang__ */
}

// (Heavily) Adapted from https://github.com/xpol/xchange, src/msgpack/type/rapidjson/document.hpp.
struct msgpack_from_json_handler
{
    msgpack_from_json_handler(msgpack::object& root, msgpack::zone& zone)
        : m_containers()
        , m_queued()
        , m_root(root)
        , m_zone(zone)
        , m_previously_initialized(false)
    {
    }

    msgpack::object* queue_object() {
        if (m_containers.empty()) {
            if (m_previously_initialized) {
                return nullptr;
            }
            m_previously_initialized = true;
            return &m_root; // root can be queued exactly once, and never again
        }
        m_queued.push_back(msgpack::object());
        return &m_queued.at(m_queued.size() - 1);
    }

    bool Null() {
        msgpack::object* o = queue_object();
        if (!o) { return false; }
        o->type = msgpack::type::NIL;
        return true;
    }

    bool Bool(bool b) {
        msgpack::object* o = queue_object();
        if (!o) { return false; }
        o->type = msgpack::type::BOOLEAN;
        o->via.boolean = b;
        return true;
    }

    bool Int(int i) {
        msgpack::object* o = queue_object();
        if (!o) { return false; }
        if (i >= 0) { return Uint64(static_cast<uint64_t>(i)); }
        o->type = msgpack::type::NEGATIVE_INTEGER;
        o->via.i64 = i;
        return true;
    }

    bool Uint(unsigned i) {
        msgpack::object* o = queue_object();
        if (!o) { return false; }
        o->type = msgpack::type::POSITIVE_INTEGER;
        o->via.u64 = i;
        return true;
    }

    bool Int64(int64_t i) {
        msgpack::object* o = queue_object();
        if (!o) { return false; }
        if (i >= 0) { return Uint64(static_cast<uint64_t>(i)); }
        o->type = msgpack::type::NEGATIVE_INTEGER;
        o->via.i64 = i;
        return true;
    }

    bool Uint64(uint64_t i) {
        msgpack::object* o = queue_object();
        if (!o) { return false; }
        o->type = msgpack::type::POSITIVE_INTEGER;
        o->via.u64 = i;
        return true;
    }

    bool Double(double d) {
        msgpack::object* o = queue_object();
        if (!o) { return false; }
        o->type = msgpack::type::FLOAT;
        o->via.f64 = d;
        return true;
    }

    bool String(const char* str, std::size_t length, bool copy) {
        msgpack::object* o = queue_object();
        if (!o) { return false; }

        // FIXME: Implement support for BIN.

        o->type = msgpack::type::STR;
        if (copy) {
            char* tmp = static_cast<char*>(m_zone.allocate_align(length));
            if (!tmp) { return false; }
            std::memcpy(tmp, str, length);
            o->via.str.ptr = tmp;
        } else {
            o->via.str.ptr = str;
        }
        o->via.str.size = length;
        return true;
    }

    bool Key(const char* str, std::size_t length, bool copy) {
        return String(str, length, copy);
    }

    bool StartObject() {
        msgpack::object* o = queue_object();
        if (!o) { return false; }
        o->type = msgpack::type::MAP;
        m_containers.push(o);
        return true;
    }

    bool EndObject(std::size_t memberCount) {
        if (m_containers.empty()) { return false; }
        if (m_queued.size() < memberCount * 2) { return false; }
        msgpack::object* o = m_containers.top();
        m_containers.pop();
        if (o->type != msgpack::type::MAP) { return false; }

        if (memberCount == 0) {
            o->via.map.ptr = nullptr;
            o->via.map.size = 0;
        } else {
            msgpack::object_kv* p = static_cast<msgpack::object_kv*>(
                    m_zone.allocate_align(sizeof(msgpack::object_kv)*memberCount));
            if (!p) { return false; }
            o->via.map.ptr = p;
            o->via.map.size = memberCount;

            for (std::size_t i = 0; i < memberCount; ++i) {
                const auto queued_index = m_queued.size() - (memberCount - i) * 2;
                copy_map_item(*o, i, m_queued.at(queued_index), m_queued.at(queued_index + 1));
            }
            m_queued.resize(m_queued.size() - memberCount * 2);
        }
        return true;
    }

    bool StartArray() {
        msgpack::object* o = queue_object();
        if (!o) { return false; }
        o->type = msgpack::type::ARRAY;
        m_containers.push(o);
        return true;
    }

    bool EndArray(std::size_t elementCount) {
        if (m_containers.empty()) { return false; }
        if (m_queued.size() < elementCount) { return false; }
        msgpack::object* o = m_containers.top();
        m_containers.pop();
        if (o->type != msgpack::type::ARRAY) { return false; }

        if (elementCount == 0) {
            o->via.array.ptr = nullptr;
            o->via.array.size = 0;
        } else {
            msgpack::object* p = static_cast<msgpack::object*>(
                    m_zone.allocate_align(sizeof(msgpack::object)*elementCount));
            if (!p) { return false; }
            o->via.array.ptr = p;
            o->via.array.size = elementCount;

            for (std::size_t i = 0; i < elementCount; ++i) {
                copy_array_item(*o, i, m_queued.at(m_queued.size() - elementCount + i));
            }
            m_queued.resize(m_queued.size() - elementCount);
        }
        return true;
    }

private:
    std::stack<msgpack::object*> m_containers;
    std::vector<msgpack::object> m_queued;
    msgpack::object& m_root;
    msgpack::zone& m_zone;
    bool m_previously_initialized;
};

// (Heavily) Adapted from https://github.com/xpol/xchange, src/msgpack/type/rapidjson/document.hpp.
template <typename Writer>
inline bool write_json(const msgpack::object& o, Writer& writer)
{
    switch (o.type) {
    case msgpack::type::BOOLEAN: return writer.Bool(o.via.boolean);
    case msgpack::type::POSITIVE_INTEGER: return writer.Uint64(o.via.u64);
    case msgpack::type::NEGATIVE_INTEGER: return writer.Int64(o.via.i64);
    case msgpack::type::FLOAT: return writer.Double(o.via.f64); break;
    case msgpack::type::STR: return writer.String(o.via.str.ptr, o.via.str.size);
    case msgpack::type::BIN: return writer.String(o.via.bin.ptr, o.via.bin.size); // FIXME: implement correctly
    case msgpack::type::ARRAY:
        {
            writer.StartArray();
            msgpack::object* ptr = o.via.array.ptr;
            msgpack::object* end = ptr + o.via.array.size;
            for (; ptr < end; ++ptr) {
                if (!write_json(*ptr, writer)) {
                    return false;
                }
            }
            return writer.EndArray();
        }
    case msgpack::type::MAP:
        {
            writer.StartObject();
            msgpack::object_kv* ptr = o.via.map.ptr;
            msgpack::object_kv* end = ptr + o.via.map.size;
            for (; ptr < end; ++ptr) {
                // Writer::Key() is a synonym of Writer::String() so we can delegate.
                // The SAX API will likely complain if the key is not a string, but we
                // can offload that responsibility instead of ensuring it ourselves.
                if (!write_json(ptr->key, writer)) {
                    return false;
                }
                if (!write_json(ptr->val, writer)) {
                    return false;
                }
            }
            return writer.EndObject();
        }
    case msgpack::type::EXT: // FIXME: implement some extension types?
    case msgpack::type::NIL:
    default:
        return writer.Null();
    }
}

} // namespace serialization
} // namespace bonefish

#endif // BONEFISH_SERIALIZATION_JSON_MSGPACK_SAX_HPP
