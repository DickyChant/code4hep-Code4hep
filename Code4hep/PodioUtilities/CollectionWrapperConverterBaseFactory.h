#ifndef Code4Hep_PodioUtilities_CollectionWrapperConverterBaseFactory_h
#define Code4Hep_PodioUtilities_CollectionWrapperConverterBaseFactory_h
#include "FWCore/PluginManager/interface/PluginFactory.h"
#include "Code4hep/PodioUtilities/CollectionWrapperConverterBase.h"

namespace c4h {
  using CollectionWrapperConverterBaseFactory = edmplugin::PluginFactory<CollectionWrapperConverterBase*()>;
}  // namespace c4h

#endif
