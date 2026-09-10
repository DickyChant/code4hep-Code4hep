#ifndef CODE4HEP_IOUTILITIES_COLLECTIONNAMECONVERSION_H
#define CODE4HEP_IOUTILITIES_COLLECTIONNAMECONVERSION_H

#include <string>
#include <string_view>

namespace c4h {

// Stitched uses '_' as the persistent branch-name field separator, so a
// podio collection containing '_' cannot be used directly as a module label.
// These functions provide a reversible, alphanumeric label for such names.
std::string collectionNameToProductLabel(std::string_view collectionName);
std::string productLabelToCollectionName(std::string_view productLabel);

}  // namespace c4h

#endif
