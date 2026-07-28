//---------------------------------------------------------------------------//
//! \file Code4hep/G4Application/interface/CalorimeterSD.h
//---------------------------------------------------------------------------//
#pragma once

#include "Code4hep/G4Application/interface/CalorimeterHit.h"

#include "G4VSensitiveDetector.hh"

class G4Step;
class G4HCofThisEvent;

namespace c4h
{
//---------------------------------------------------------------------------//
/*!
 * Example sensitive detector.
 */
class CalorimeterSD : public G4VSensitiveDetector
{
  public:
    CalorimeterSD(G4String name);
    ~CalorimeterSD() = default;

    void Initialize(G4HCofThisEvent*) final;
    G4bool ProcessHits(G4Step*, G4TouchableHistory*) final;

  private:
    G4int hcid_{-1};
    CalorimeterHitsCollection* collection_{nullptr};
};

//---------------------------------------------------------------------------//
}  // namespace c4h   
