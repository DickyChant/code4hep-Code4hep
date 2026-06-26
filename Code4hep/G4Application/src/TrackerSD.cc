//---------------------------------------------------------------------------//
//! \file Code4hep/G4Application/src/TrackerSD.cc
//---------------------------------------------------------------------------//
#include "Code4hep/G4Application/TrackerSD.h"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

namespace c4h
{
//---------------------------------------------------------------------------//
/*!
 * Construct with sensitive detector name.
 */
TrackerSD::TrackerSD(G4String name)
  : G4VSensitiveDetector(name), hcid_(-1), collection_(nullptr)
{
  G4String HCname = name + "_HC";
  collectionName.insert(HCname);
}

//---------------------------------------------------------------------------//
/*!
 * Set up hit collections for a new event.
 */
void TrackerSD::Initialize(G4HCofThisEvent* hce)
{
  collection_
    = new TrackerHitsCollection(SensitiveDetectorName, collectionName[0]);
  if (hcid_ < 0)
  {
    hcid_ = G4SDManager::GetSDMpointer()->GetCollectionID(collection_);
  }
  hce->AddHitsCollection(hcid_, collection_);
}

//---------------------------------------------------------------------------//
/*!
 * Add hits to the current hit collection.
 */
G4bool TrackerSD::ProcessHits(G4Step* step, G4TouchableHistory*)
{
  // Get hit data for this sensitive detector
  auto touchable = step->GetPreStepPoint()->GetTouchable();
  unsigned int id = touchable->GetVolume()->GetCopyNo();
  auto time = step->GetPreStepPoint()->GetGlobalTime();
  auto pos = touchable->GetTranslation();

  //  Return this cell if it was hit before
  for (auto hit : *(collection_->GetVector()))
  {
    if (id == hit->id())
    {
      return true;
    }
  }

  // Otherwise, create a new hit:
  collection_->insert(new TrackerHit(id, time, pos));

  return true;
}

//---------------------------------------------------------------------------//
}  // namespace c4h
