//---------------------------------------------------------------------------//
//! \file Code4hep/G4Application/interface/CalorimeterHit.h
//---------------------------------------------------------------------------//
#pragma once

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

namespace c4h
{
//---------------------------------------------------------------------------//
/*!
 * Example sensitive hit class.
 */
class CalorimeterHit : public G4VHit
{
    using id_type = unsigned int;

  public:
    CalorimeterHit() : G4VHit() {}
    CalorimeterHit(id_type id, G4double edep, G4double time, G4ThreeVector pos);
    ~CalorimeterHit() override;

    CalorimeterHit(const CalorimeterHit&) = default;
    const CalorimeterHit& operator=(const CalorimeterHit& rhs);
    G4bool operator==(const CalorimeterHit& rhs) const;

    inline void* operator new(size_t);
    inline void operator delete(void*);

    // Accessors
    inline id_type id() const { return id_; };
    inline G4double edep() const { return edep_; };
    inline G4double time() const { return time_; };
    inline G4ThreeVector pos() const { return pos_; };

    inline void add_edep(G4double edep) { edep_ += edep; };

  private:
    id_type       id_{0};
    G4double      edep_{0};
    G4double      time_{0};
    G4ThreeVector pos_{0, 0, 0};
};

using CalorimeterHitsCollection = G4THitsCollection<CalorimeterHit>;
extern G4ThreadLocal G4Allocator<CalorimeterHit>* CalorimeterHitAllocator;

//---------------------------------------------------------------------------//
// INLINE DEFINITIONS
//---------------------------------------------------------------------------//
/*!
 * Use G4Allocator to allocate memory for a CalorimeterHit.
 */
inline void* CalorimeterHit::operator new(size_t)
{
    if (!CalorimeterHitAllocator)
    {
        CalorimeterHitAllocator = new G4Allocator<CalorimeterHit>;
    }
    return (void*)CalorimeterHitAllocator->MallocSingle();
}

//---------------------------------------------------------------------------//
/*!
 * Use G4Allocator to release memory for a CalorimeterHit.
 */
inline void CalorimeterHit::operator delete(void* hit)
{
    CalorimeterHitAllocator->FreeSingle((CalorimeterHit*)hit);
}

//---------------------------------------------------------------------------//
}  // namespace c4h
