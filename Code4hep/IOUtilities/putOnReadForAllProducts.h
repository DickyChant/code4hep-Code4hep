#ifndef Code4hep_IOUtilities_putOnReadForAllProducts_h
#define Code4hep_IOUtilities_putOnReadForAllProducts_h

#include "podio/Frame.h"

namespace edm {
  class EventPrincipal;
  class ProductRegistry;
}

namespace c4h {
  // Move all collections in a Frame into the corresponding Stitched input
  // products. Relations are materialised before any collection is moved.
  void putOnReadForAllProducts(podio::Frame const&,
                               edm::ProductRegistry const&,
                               edm::EventPrincipal&);
}  // namespace c4h

#endif
