#include "Code4hep/IOUtilities/TypeNameConversion.h"

#include <unordered_map>

namespace c4h {
  std::string const& typeNameConversion(std::string const& rootTypeName) {
    static const std::unordered_map<std::string, std::string> conversion = {
        {"podio::UserDataCollection<float,void>", "podio::UserDataCollection<float>"},
        {"podio::UserDataCollection<int,void>", "podio::UserDataCollection<int32_t>"}};
    const auto found = conversion.find(rootTypeName);
    return found == conversion.end() ? rootTypeName : found->second;
  }
}  // namespace c4h
