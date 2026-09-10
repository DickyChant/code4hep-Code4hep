#include "Code4hep/IOUtilities/FrameParameterConversion.h"

#include "podio/CollectionBase.h"
#include "podio/Frame.h"
#include "podio/UserDataCollection.h"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_set>
#include <utility>
#include <vector>

namespace c4h {
namespace {

constexpr std::string_view marker = "c4hPodioFrameParameterV1";

char hexDigit(unsigned value) {
  return "0123456789abcdef"[value & 0xfU];
}

std::string encodeKey(std::string_view key) {
  std::string encoded;
  encoded.reserve(key.size() * 2U);
  for (const unsigned char byte : key) {
    encoded.push_back(hexDigit(byte >> 4U));
    encoded.push_back(hexDigit(byte));
  }
  return encoded;
}

int hexValue(char value) {
  if (value >= '0' && value <= '9') return value - '0';
  if (value >= 'a' && value <= 'f') return value - 'a' + 10;
  return -1;
}

std::string decodeKey(std::string_view encoded) {
  if (encoded.size() % 2U != 0U) {
    throw std::invalid_argument("Malformed materialized podio parameter name");
  }
  std::string key;
  key.reserve(encoded.size() / 2U);
  for (std::size_t i = 0; i < encoded.size(); i += 2U) {
    const int high = hexValue(encoded[i]);
    const int low = hexValue(encoded[i + 1U]);
    if (high < 0 || low < 0) {
      throw std::invalid_argument("Malformed materialized podio parameter name");
    }
    key.push_back(static_cast<char>((high << 4) | low));
  }
  return key;
}

std::string parameterCollectionName(char type, std::string_view key) {
  std::string name(marker);
  name.push_back(type);
  name.append(encodeKey(key));
  return name;
}

template <typename T>
void putParameterCollection(podio::Frame& frame,
                            std::unordered_set<std::string> const& collectionNames,
                            char type,
                            std::string const& key,
                            std::vector<T> values) {
  const auto name = parameterCollectionName(type, key);
  if (collectionNames.contains(name)) {
    throw std::invalid_argument("Podio collection name collides with reserved Frame parameter name '" + name + "'");
  }
  frame.put(podio::UserDataCollection<T>{std::move(values)}, name);
}

std::vector<int32_t> encodeStrings(std::vector<std::string> const& values) {
  if (values.size() > static_cast<std::size_t>(std::numeric_limits<int32_t>::max())) {
    throw std::length_error("Too many values in podio string parameter");
  }
  std::vector<int32_t> encoded;
  encoded.push_back(static_cast<int32_t>(values.size()));
  for (auto const& value : values) {
    if (value.size() > static_cast<std::size_t>(std::numeric_limits<int32_t>::max())) {
      throw std::length_error("Podio string parameter value is too long");
    }
    encoded.push_back(static_cast<int32_t>(value.size()));
    encoded.reserve(encoded.size() + value.size());
    for (const unsigned char byte : value) encoded.push_back(byte);
  }
  return encoded;
}

std::vector<std::string> decodeStrings(podio::UserDataCollection<int32_t> const& encoded) {
  auto at = encoded.begin();
  if (at == encoded.end() || *at < 0) {
    throw std::invalid_argument("Malformed materialized podio string parameter");
  }
  const auto count = static_cast<std::size_t>(*at++);
  std::vector<std::string> values;
  values.reserve(count);
  for (std::size_t i = 0; i < count; ++i) {
    if (at == encoded.end() || *at < 0) {
      throw std::invalid_argument("Malformed materialized podio string parameter");
    }
    const auto length = static_cast<std::size_t>(*at++);
    std::string value;
    value.reserve(length);
    for (std::size_t j = 0; j < length; ++j) {
      if (at == encoded.end() || *at < 0 || *at > 255) {
        throw std::invalid_argument("Malformed materialized podio string parameter");
      }
      value.push_back(static_cast<char>(*at++));
    }
    values.emplace_back(std::move(value));
  }
  if (at != encoded.end()) {
    throw std::invalid_argument("Malformed materialized podio string parameter");
  }
  return values;
}

template <typename T>
podio::UserDataCollection<T> const& asUserData(podio::CollectionBase const& collection) {
  const auto* typed = dynamic_cast<podio::UserDataCollection<T> const*>(&collection);
  if (!typed) throw std::invalid_argument("Materialized podio parameter has the wrong collection type");
  return *typed;
}

template <typename T>
std::vector<T> copyValues(podio::UserDataCollection<T> const& collection) {
  return {collection.begin(), collection.end()};
}

}  // namespace

void materializeFrameParameters(podio::Frame& frame) {
  const auto available = frame.getAvailableCollections();
  const std::unordered_set<std::string> collectionNames(available.begin(), available.end());
  auto const& parameters = frame.getParameters();

  {
    auto [keys, values] = parameters.getKeysAndValues<int>();
    for (std::size_t i = 0; i < keys.size(); ++i) {
      std::vector<int32_t> converted(values[i].begin(), values[i].end());
      putParameterCollection(frame, collectionNames, 'I', keys[i], std::move(converted));
    }
  }
  {
    auto [keys, values] = parameters.getKeysAndValues<float>();
    for (std::size_t i = 0; i < keys.size(); ++i) {
      putParameterCollection(frame, collectionNames, 'F', keys[i], std::move(values[i]));
    }
  }
  {
    auto [keys, values] = parameters.getKeysAndValues<double>();
    for (std::size_t i = 0; i < keys.size(); ++i) {
      putParameterCollection(frame, collectionNames, 'D', keys[i], std::move(values[i]));
    }
  }
  {
    auto [keys, values] = parameters.getKeysAndValues<std::string>();
    for (std::size_t i = 0; i < keys.size(); ++i) {
      putParameterCollection(frame, collectionNames, 'S', keys[i], encodeStrings(values[i]));
    }
  }
}

bool restoreFrameParameter(podio::Frame& frame,
                           std::string_view collectionName,
                           podio::CollectionBase const& collection) {
  if (!collectionName.starts_with(marker) || collectionName.size() <= marker.size()) return false;

  const char type = collectionName[marker.size()];
  const auto key = decodeKey(collectionName.substr(marker.size() + 1U));
  switch (type) {
    case 'I': {
      auto values = copyValues(asUserData<int32_t>(collection));
      frame.putParameter(key, std::vector<int>(values.begin(), values.end()));
      return true;
    }
    case 'F':
      frame.putParameter(key, copyValues(asUserData<float>(collection)));
      return true;
    case 'D':
      frame.putParameter(key, copyValues(asUserData<double>(collection)));
      return true;
    case 'S':
      frame.putParameter(key, decodeStrings(asUserData<int32_t>(collection)));
      return true;
    default:
      throw std::invalid_argument("Malformed materialized podio parameter type");
  }
}

}  // namespace c4h
