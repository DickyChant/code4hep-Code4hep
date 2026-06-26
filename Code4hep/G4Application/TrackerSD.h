//---------------------------------------------------------------------------//
//! \file Code4hep/G4Application/TrackerSD.h
//---------------------------------------------------------------------------//
#ifndef Code4hep_G4Application_TrackerSD_h
#define Code4hep_G4Application_TrackerSD_h

#include "Code4hep/G4Application/TrackerHit.h"

#include "G4VSensitiveDetector.hh"

class G4Step;
class G4HCofThisEvent;

namespace c4h
{
//---------------------------------------------------------------------------//
/*!
 * Example sensitive detector.
 */
class TrackerSD : public G4VSensitiveDetector
{
public:
  TrackerSD(G4String name);
  ~TrackerSD() = default;

  void Initialize(G4HCofThisEvent*) final;
  G4bool ProcessHits(G4Step*, G4TouchableHistory*) final;

private:
  G4int hcid_{-1};
  TrackerHitsCollection* collection_{nullptr};
};

//---------------------------------------------------------------------------//
}  // namespace c4h
#endif
