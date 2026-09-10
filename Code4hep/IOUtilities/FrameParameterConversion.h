#ifndef Code4Hep_IOUtilities_FrameParameterConversion_h
#define Code4Hep_IOUtilities_FrameParameterConversion_h

#include <string_view>

namespace podio {
  class CollectionBase;
  class Frame;
}

namespace c4h {
  // Framework events can carry podio collections but not Frame generic
  // parameters. Materialize every parameter as a reserved UserDataCollection
  // before filling the ProductRegistry.
  void materializeFrameParameters(podio::Frame& frame);

  // Convert one reserved materialized collection back into a Frame parameter.
  // Returns false for an ordinary collection.
  bool restoreFrameParameter(podio::Frame& frame,
                             std::string_view collectionName,
                             podio::CollectionBase const& collection);
}

#endif
