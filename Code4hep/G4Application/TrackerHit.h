//---------------------------------------------------------------------------//
//! \file Code4hep/G4Application/TrackerHit.h
//---------------------------------------------------------------------------//
#ifndef Code4hep_G4Application_TrackerHit_h
#define Code4hep_G4Application_TrackerHit_h

#include "G4Allocator.hh"
#include "G4THitsCollection.hh"
#include "G4ThreeVector.hh"
#include "G4VHit.hh"

namespace c4h
{
//---------------------------------------------------------------------------//
/*!
 * Example tracker (sensitive) hit class.
 */

class TrackerHit : public G4VHit
{
  using id_type = unsigned int;

public:
  TrackerHit() : G4VHit() {}
  TrackerHit(id_type id, G4int trackID, G4double energyDeposit, G4double time, G4double pathLength, G4ThreeVector pos,
             G4ThreeVector momentum);
  ~TrackerHit() override;

  TrackerHit(const TrackerHit &) = default;
  const TrackerHit &operator=(const TrackerHit &rhs);
  G4bool operator==(const TrackerHit &rhs) const;

  inline void *operator new(size_t);
  inline void operator delete(void *);

  // Accessors
  inline id_type id() const { return id_; };
  inline G4int trackID() const { return trackID_; };
  inline G4double energyDeposit() const { return energyDeposit_; };
  inline G4double time() const { return time_; };
  inline G4double pathLength() const { return pathLength_; };
  inline G4ThreeVector pos() const { return pos_; };
  inline G4ThreeVector momentum() const { return momentum_; };
  inline void add(G4double energyDeposit, G4double pathLength)
  {
    energyDeposit_ += energyDeposit;
    pathLength_ += pathLength;
  }

private:
  id_type id_{0};
  G4int trackID_{0};
  G4double energyDeposit_{0};
  G4double time_{0};
  G4double pathLength_{0};
  G4ThreeVector pos_{0, 0, 0};
  G4ThreeVector momentum_{0, 0, 0};
};

using TrackerHitsCollection = G4THitsCollection<TrackerHit>;
extern G4ThreadLocal G4Allocator<TrackerHit> *TrackerHitAllocator;

//---------------------------------------------------------------------------//
// INLINE DEFINITIONS
//---------------------------------------------------------------------------//
/*!
 * Use G4Allocator to allocate memory for a TrackerHit.
 */
inline void *TrackerHit::operator new(size_t)
{
  if (!TrackerHitAllocator)
  {
    TrackerHitAllocator = new G4Allocator<TrackerHit>;
  }
  return (void *)TrackerHitAllocator->MallocSingle();
}

//---------------------------------------------------------------------------//
/*!
 * Use G4Allocator to release memory for a TrackerHit.
 */
inline void TrackerHit::operator delete(void *hit) { TrackerHitAllocator->FreeSingle((TrackerHit *)hit); }

//---------------------------------------------------------------------------//
} // namespace c4h
#endif
