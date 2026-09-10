#ifndef Code4hep_G4Application_EventProducts_h
#define Code4hep_G4Application_EventProducts_h

#include "edm4hep/SimCalorimeterHitCollection.h"
#include "edm4hep/SimTrackerHitCollection.h"

#include <memory>

class G4Event;

namespace c4h {

struct EventProducts {
  std::unique_ptr<edm4hep::SimTrackerHitCollection> trackerHits;
  std::unique_ptr<edm4hep::SimCalorimeterHitCollection> calorimeterHits;
};

// Copy Geant4's thread-local hit collections into framework-owned EDM4hep
// collections before the G4Event is destroyed.
EventProducts makeEventProducts(G4Event &event);

} // namespace c4h

#endif
