#include "Code4hep/IOUtilities/CollectionNameConversion.h"

#include <stdexcept>

namespace c4h {
namespace {

constexpr std::string_view marker = "c4hPodioEncoded";
constexpr char hexDigits[] = "0123456789abcdef";

int hexValue(char digit) {
  if (digit >= '0' && digit <= '9') return digit - '0';
  if (digit >= 'a' && digit <= 'f') return digit - 'a' + 10;
  return -1;
}

}  // namespace

std::string collectionNameToProductLabel(std::string_view collectionName) {
  if (collectionName.find('_') == std::string_view::npos &&
      !collectionName.starts_with(marker)) {
    return std::string(collectionName);
  }

  std::string result(marker);
  result.reserve(marker.size() + 2 * collectionName.size());
  for (const unsigned char character : collectionName) {
    result.push_back(hexDigits[character >> 4]);
    result.push_back(hexDigits[character & 0xf]);
  }
  return result;
}

std::string productLabelToCollectionName(std::string_view productLabel) {
  if (!productLabel.starts_with(marker)) return std::string(productLabel);

  const auto encoded = productLabel.substr(marker.size());
  if (encoded.size() % 2 != 0) {
    throw std::runtime_error("Malformed encoded podio product label");
  }

  std::string result;
  result.reserve(encoded.size() / 2);
  for (std::size_t index = 0; index < encoded.size(); index += 2) {
    const int high = hexValue(encoded[index]);
    const int low = hexValue(encoded[index + 1]);
    if (high < 0 || low < 0) {
      throw std::runtime_error("Malformed encoded podio product label");
    }
    result.push_back(static_cast<char>((high << 4) | low));
  }
  return result;
}

}  // namespace c4h
