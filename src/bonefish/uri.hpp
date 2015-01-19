#ifndef BONEFISH_URI_HPP
#define BONEFISH_URI_HPP

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

bool is_valid_uri(const std::string& uri);

} // namespace bonefish

#endif // BONEFISH_URI_HPP
