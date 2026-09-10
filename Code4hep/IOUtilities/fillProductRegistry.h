#ifndef Code4hep_IOUtilities_fillProductRegistry_h
#define Code4hep_IOUtilities_fillProductRegistry_h

#include <memory>
#include <string>

#include "podio/Frame.h"
#include "DataFormats/Provenance/interface/ProductRegistry.h"

namespace c4h {
  // Describe every collection in a Frame as an input product. The first frame
  // establishes the schema for the source, matching PodioSource semantics.
  std::unique_ptr<edm::ProductRegistry> fillProductRegistry(podio::Frame const&,
                                                            std::string const& processName,
                                                            bool ignoreMissingOnFirstEvent);
}  // namespace c4h

#endif
