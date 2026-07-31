//---------------------------------------------------------------------------//
//! \file Code4hep/G4Application/src/CalorimeterHit.cc
//---------------------------------------------------------------------------//
#include "Code4hep/G4Application/interface/CalorimeterHit.h"

namespace c4h
{
G4ThreadLocal G4Allocator<CalorimeterHit>* CalorimeterHitAllocator = nullptr;

//---------------------------------------------------------------------------//
/*!
 * Construct with hit data.
 */
CalorimeterHit::CalorimeterHit(id_type id,
                               G4double edep,
                               G4double time,
                               G4ThreeVector pos)
  : G4VHit(), id_(id), edep_(edep), time_(time), pos_(pos)
{
}

CalorimeterHit::~CalorimeterHit() {}

const CalorimeterHit& CalorimeterHit::operator=(const CalorimeterHit& rhs)
{
  id_   = rhs.id_;
  edep_ = rhs.edep_;
  time_ = rhs.time_;
  pos_  = rhs.pos_;

  return *this;
}

G4bool CalorimeterHit::operator==(const CalorimeterHit& rhs) const
{
  return (this == &rhs) ? true : false;
}

//---------------------------------------------------------------------------//
}  // namespace c4h
