//---------------------------------------------------------------------------//
//! \file Code4hep/G4Application/src/TrackerHit.cc
//---------------------------------------------------------------------------//
#include "Code4hep/G4Application/TrackerHit.h"

namespace c4h {
G4ThreadLocal G4Allocator<TrackerHit> *TrackerHitAllocator = nullptr;

//---------------------------------------------------------------------------//
/*!
 * Construct with hit data.
 */
TrackerHit::TrackerHit(id_type id, G4int trackID, G4int mcParticleIndex,
                       G4double energyDeposit, G4double time,
                       G4double pathLength, G4ThreeVector pos,
                       G4ThreeVector momentum, bool explicitCellID)
    : G4VHit(), id_(id), explicitCellID_(explicitCellID), trackID_(trackID),
      mcParticleIndex_(mcParticleIndex), energyDeposit_(energyDeposit),
      time_(time), pathLength_(pathLength), pos_(pos), momentum_(momentum) {}

TrackerHit::~TrackerHit() {}

const TrackerHit &TrackerHit::operator=(const TrackerHit &rhs) {
  id_ = rhs.id_;
  explicitCellID_ = rhs.explicitCellID_;
  trackID_ = rhs.trackID_;
  mcParticleIndex_ = rhs.mcParticleIndex_;
  energyDeposit_ = rhs.energyDeposit_;
  time_ = rhs.time_;
  pathLength_ = rhs.pathLength_;
  pos_ = rhs.pos_;
  momentum_ = rhs.momentum_;
  return *this;
}

G4bool TrackerHit::operator==(const TrackerHit &rhs) const {
  return (this == &rhs) ? true : false;
}

//---------------------------------------------------------------------------//
} // namespace c4h
