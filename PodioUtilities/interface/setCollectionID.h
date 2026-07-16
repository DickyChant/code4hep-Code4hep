#ifndef Code4Hep_PodioUtilities_setCollectionID_h
#define Code4Hep_PodioUtilities_setCollectionID_h

/**

 Description: Calculates the collection ID for a given collection.
 Then it saves the collection ID in the collection object.
 This function needs to be called by producers before collections
 are put into the edm::Event.

 It may be possible to modify the infrastructure code to handle
 this automatically and then it might be possible to delete this
 function entirely and the need for producers to call it.
 We are adding it now to fix a current bug that causes all
 links and relations to be broken in an output file if they
 point into collections created in a Code4Hep process.
 We are adding it now because making it automatic will
 take some time and development effort (if it is possible
 at all) and we want a working solution now.
*/
//
// Author:      W. David Dagenhart
// Created:     16 July 2026

#include "podio/CollectionBase.h"
#include "podio/CollectionIDTable.h"

#include <concepts>
#include <optional>
#include <string>

namespace c4h {

  template <typename T>
  concept PodioCollection = std::derived_from<T, podio::CollectionBase>;

  template <PodioCollection CollectionType, typename U, typename ProducerType>
  void setCollectionID(
      CollectionType& collection,
      U const& iEvent,
      ProducerType const& producer,
      edm::EDPutTokenT<CollectionType> const& token) {
    const std::string& moduleLabel =
        iEvent.moduleCallingContext()->moduleDescription()->moduleLabel();
    const std::string& instanceName =
        producer.typeLabelList()[token.index()].productInstanceName_;
    std::string collectionName = moduleLabel + instanceName;

    podio::CollectionIDTable collectionIDTable;
    auto id = collectionIDTable.collectionID(collectionName);
    if (!id.has_value()) {
      id = collectionIDTable.add(collectionName);
    }
    collection.setID(id.value());
  }
}
#endif
