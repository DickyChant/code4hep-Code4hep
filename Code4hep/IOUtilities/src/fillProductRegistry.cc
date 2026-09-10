#include "Code4hep/IOUtilities/fillProductRegistry.h"

#include <string_view>
#include <typeinfo>
#include <vector>

#include "podio/CollectionBase.h"

#include "Code4hep/IOUtilities/CollectionNameConversion.h"
#include "Code4hep/PodioUtilities/CollectionWrapperConverterBaseFactory.h"
#include "DataFormats/Provenance/interface/ProductDescription.h"
#include "FWCore/Utilities/interface/BranchType.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/Utilities/interface/TypeID.h"

namespace c4h {
  std::unique_ptr<edm::ProductRegistry> fillProductRegistry(podio::Frame const& frame,
                                                            std::string const& processName,
                                                            bool ignoreMissingOnFirstEvent) {
    auto productRegistry = std::make_unique<edm::ProductRegistry>();
    productRegistry->setProcessOrder({processName});

    auto factory = CollectionWrapperConverterBaseFactory::get();
    for (std::string const& collectionName : frame.getAvailableCollections()) {
      const podio::CollectionBase* collection = frame.get(collectionName);
      if (!collection) {
        if (ignoreMissingOnFirstEvent) continue;
        throw cms::Exception("MissingCollection")
            << "podio::Frame::get returned nullptr for collection '"
            << collectionName << "' in the source's first event";
      }

      edm::TypeID typeID;
      try {
        const std::string typeName(collection->getTypeName());
        typeID = edm::TypeID(factory->create(typeName)->typeID());
      } catch (cms::Exception const&) {
        throw cms::Exception("UnregisteredCollectionType")
            << "The data product type '" << collection->getTypeName()
            << "' is not registered with C4H_COLLECTION";
      }

      edm::ProductDescription description(edm::InEvent,
                                          collectionNameToProductLabel(collectionName),
                                          processName,
                                          "",
                                          typeID,
                                          false);
      productRegistry->copyProduct(description);
    }
    return productRegistry;
  }
}  // namespace c4h
