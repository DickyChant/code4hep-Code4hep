//---------------------------------------------------------------------------//
//! \file Code4hep/G4Application/src/TrackerHit.cc
//---------------------------------------------------------------------------//
#include "Code4hep/G4Application/interface/TrackerHit.h"

namespace c4h
{
G4ThreadLocal G4Allocator<TrackerHit>* TrackerHitAllocator = nullptr;

//---------------------------------------------------------------------------//
/*!
 * Construct with hit data.
 */
TrackerHit::TrackerHit(id_type id, G4double time, G4ThreeVector pos)
    : G4VHit(), id_(id), time_(time), pos_(pos)
{
}

TrackerHit::~TrackerHit() {}

const TrackerHit& TrackerHit::operator=(const TrackerHit& rhs)
{
  id_   = rhs.id_;
  time_ = rhs.time_;
  pos_  = rhs.pos_;
  return *this;
}

G4bool TrackerHit::operator==(const TrackerHit& rhs) const
{
  return (this == &rhs) ? true : false;
}

//---------------------------------------------------------------------------//
}  // namespace c4h
