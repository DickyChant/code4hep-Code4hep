#ifndef Code4hep_IOUtilities_TypeNameConversion_h
#define Code4hep_IOUtilities_TypeNameConversion_h

#include <string>

namespace c4h {
  // Convert a typename from the name used in a ProductDescription to the name
  // expected by the CollectionWrapperConverterBaseFactory.
  std::string const& typeNameConversion(std::string const&);
}  // namespace c4h

#endif
