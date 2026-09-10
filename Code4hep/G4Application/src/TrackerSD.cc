//---------------------------------------------------------------------------//
//! \file Code4hep/G4Application/src/TrackerSD.cc
//---------------------------------------------------------------------------//
#include "Code4hep/G4Application/TrackerSD.h"

#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4ios.hh"

namespace c4h {
//---------------------------------------------------------------------------//
/*!
 * Construct with sensitive detector name.
 */
TrackerSD::TrackerSD(G4String name, bool mergeSteps)
    : G4VSensitiveDetector(name), hcid_(-1), collection_(nullptr),
      mergeSteps_(mergeSteps) {
  G4String HCname = name + "_HC";
  collectionName.insert(HCname);
}

//---------------------------------------------------------------------------//
/*!
 * Set up hit collections for a new event.
 */
void TrackerSD::Initialize(G4HCofThisEvent *hce) {
  collection_ =
      new TrackerHitsCollection(SensitiveDetectorName, collectionName[0]);
  hitByTrackAndCell_.clear();
  if (hcid_ < 0) {
    hcid_ = G4SDManager::GetSDMpointer()->GetCollectionID(collection_);
  }
  hce->AddHitsCollection(hcid_, collection_);
}

//---------------------------------------------------------------------------//
/*!
 * Add hits to the current hit collection.
 */
G4bool TrackerSD::ProcessHits(G4Step *step, G4TouchableHistory *) {
  // Get hit data for this sensitive detector
  auto touchable = step->GetPreStepPoint()->GetTouchable();
  unsigned int id = touchable->GetVolume()->GetCopyNo();
  auto time = step->GetPreStepPoint()->GetGlobalTime();
  auto pos = step->GetPreStepPoint()->GetPosition();
  auto momentum = step->GetPreStepPoint()->GetMomentum();
  auto trackID = step->GetTrack()->GetTrackID();
  const auto key = (static_cast<std::uint64_t>(trackID) << 32U) |
                   static_cast<std::uint64_t>(id);

  // Merge steps from one particle inside one sensor, but never merge distinct
  // tracks: downstream digitizers need both occupancy and total path length.
  if (mergeSteps_) {
    if (const auto found = hitByTrackAndCell_.find(key);
        found != hitByTrackAndCell_.end()) {
      found->second->add(step->GetTotalEnergyDeposit(), step->GetStepLength());
      return true;
    }
  }
  auto *hit = new TrackerHit(id, trackID, step->GetTotalEnergyDeposit(), time,
                             step->GetStepLength(), pos, momentum);
  collection_->insert(hit);
  if (mergeSteps_) {
    hitByTrackAndCell_.emplace(key, hit);
  }

  return true;
}

//---------------------------------------------------------------------------//
} // namespace c4h
