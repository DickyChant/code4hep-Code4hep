//---------------------------------------------------------------------------//
//! \file Code4hep/G4Application/src/TrackerSD.cc
//---------------------------------------------------------------------------//
#include "Code4hep/G4Application/TrackerSD.h"
#include "Code4hep/G4Application/TrackProvenance.h"

#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4ios.hh"

namespace c4h {
//---------------------------------------------------------------------------//
/*!
 * Construct with sensitive detector name.
 */
TrackerSD::TrackerSD(G4String name, bool mergeSteps,
                     std::optional<std::uint64_t> cellIDBase)
    : G4VSensitiveDetector(name), hcid_(-1), collection_(nullptr),
      mergeSteps_(mergeSteps), cellIDBase_(cellIDBase) {
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
  const auto copyNumber =
      static_cast<std::uint32_t>(touchable->GetVolume()->GetCopyNo());
  const auto id = cellIDBase_.value_or(0) | copyNumber;
  auto time = step->GetPreStepPoint()->GetGlobalTime();
  auto pos = step->GetPreStepPoint()->GetPosition();
  auto momentum = step->GetPreStepPoint()->GetMomentum();
  auto trackID = step->GetTrack()->GetTrackID();
  auto mcParticleIndex = -1;
  if (const auto *information = dynamic_cast<const TrackInformation *>(
          step->GetTrack()->GetUserInformation())) {
    mcParticleIndex = information->mcParticleIndex();
  }
  const auto key = std::pair{trackID, id};

  // Merge steps from one particle inside one sensor, but never merge distinct
  // tracks: downstream digitizers need both occupancy and total path length.
  if (mergeSteps_) {
    if (const auto found = hitByTrackAndCell_.find(key);
        found != hitByTrackAndCell_.end()) {
      found->second->add(step->GetTotalEnergyDeposit(), step->GetStepLength());
      return true;
    }
  }
  auto *hit = new TrackerHit(
      id, trackID, mcParticleIndex, step->GetTotalEnergyDeposit(), time,
      step->GetStepLength(), pos, momentum, cellIDBase_.has_value());
  collection_->insert(hit);
  if (mergeSteps_) {
    hitByTrackAndCell_.emplace(key, hit);
  }

  return true;
}

//---------------------------------------------------------------------------//
} // namespace c4h
