#ifndef BONEFISH_UTILITY_WAMP_URI_HPP
#define BONEFISH_UTILITY_WAMP_URI_HPP

#include <string>

// TODO
//
// + IMPLEMENT
//
// ++ Strict URIs
//
// WAMP recommends following a stricter URI rule: URI components
// SHOULD only contain letters, digits and _.
//
// strict URI check disallowing empty URI components
// pattern = re.compile(r"^([0-9a-z_]+\.)*([0-9a-z_]+)$")
//
// When empty URI components are allowed (which only is the case in
// specific messages from the Advanced Profile), you can use this
// regular expression:
//
// strict URI check allowing empty URI components
// pattern = re.compile(r"^(([0-9a-z_]+\.)|\.)*([0-9a-z_]+)?$")
//
// ++ Reserved URIs
//
// application URIs MUST NOT use wamp as a first URI component, since
// this is reserved for URIs predefined with the WAMP protocol itself.
// For example:
//
//     wamp.error.not_authorized
//     wamp.error.procedure_already_exists

namespace bonefish {

/**
 * Pass one or more flags (combined with the bitwise OR operator, "|") to change
 * the behaviour of URI checking in is_valid_uri().
 */
namespace uri_flags {

/** is_valid_uri() does not allow empty URI components by default, add this flag to allow them. */
extern unsigned int allow_empty_components;

/** is_valid_uri() checks for relaxed rules by default, add this flag for a strict check. */
extern unsigned int strict;

} // namespace uri_flags


bool is_valid_uri(const std::string& uri, int flags = 0);

} // namespace bonefish

#endif // BONEFISH_UTILITY_WAMP_URI_HPP
