#include "Code4hep/IOUtilities/putOnReadForAllProducts.h"

#include <memory>
#include <optional>
#include <string>

#include "podio/CollectionBase.h"

#include "Code4hep/IOUtilities/CollectionNameConversion.h"
#include "Code4hep/IOUtilities/TypeNameConversion.h"
#include "Code4hep/PodioUtilities/CollectionWrapperConverterBaseFactory.h"
#include "DataFormats/Common/interface/WrapperBase.h"
#include "DataFormats/Provenance/interface/ProductDescription.h"
#include "DataFormats/Provenance/interface/ProductProvenance.h"
#include "FWCore/Framework/interface/EventPrincipal.h"

namespace c4h {
  void putOnReadForAllProducts(podio::Frame const& frame,
                               edm::ProductRegistry const& productRegistry,
                               edm::EventPrincipal& eventPrincipal) {
    auto factory = CollectionWrapperConverterBaseFactory::get();

    // Resolve all relations before moving any collection out of the Frame.
    for (auto const& entry : productRegistry.productList()) {
      frame.get(productLabelToCollectionName(entry.second.moduleLabel()));
    }

    for (auto const& entry : productRegistry.productList()) {
      auto const& description = entry.second;
      const auto collectionName =
          productLabelToCollectionName(description.moduleLabel());
      const podio::CollectionBase* constCollection = frame.get(collectionName);
      std::unique_ptr<edm::WrapperBase> wrapper;
      if (constCollection) {
        auto* collection = const_cast<podio::CollectionBase*>(constCollection);
        const std::string typeName(collection->getTypeName());
        wrapper = factory->create(typeName)->toWrapper(*collection);
      } else {
        wrapper = factory->create(typeNameConversion(description.fullClassName()))
                      ->createEmptyWrapper();
      }
      eventPrincipal.putOnRead(description, std::move(wrapper),
                               std::optional<edm::ProductProvenance>{});
    }
  }
}  // namespace c4h
