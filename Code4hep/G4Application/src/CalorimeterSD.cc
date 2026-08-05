//---------------------------------------------------------------------------//
//! \file Code4hep/G4Application/src/CalorimeterSD.cc
//---------------------------------------------------------------------------//
#include "Code4hep/G4Application/CalorimeterSD.h"

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
CalorimeterSD::CalorimeterSD(G4String name)
    : G4VSensitiveDetector(name), hcid_(-1), collection_(nullptr)
{
  G4String HCname = name + "_HC";
  collectionName.insert(HCname);
}

//---------------------------------------------------------------------------//
/*!
 * Set up hit collections for a new event.
 */  
void CalorimeterSD::Initialize(G4HCofThisEvent* hce)
{
  collection_ = new CalorimeterHitsCollection(SensitiveDetectorName,
                                              collectionName[0]);
  if (hcid_ < 0)
  {
      hcid_ = G4SDManager::GetSDMpointer()->GetCollectionID(collection_);
  }
  hce->AddHitsCollection(hcid_, collection_);
}

G4bool CalorimeterSD::ProcessHits(G4Step* step, G4TouchableHistory*)
{
  auto edep = step->GetTotalEnergyDeposit();

  if (edep == 0.)
  {
      return false;
  }

  // Get hit data for this sensitive detector
  auto touchable = step->GetPreStepPoint()->GetTouchable();
  unsigned int id = touchable->GetVolume()->GetCopyNo();
  auto time = step->GetPreStepPoint()->GetGlobalTime();
  auto pos = touchable->GetTranslation();

  // Add energy deposition for this cell if it was hit before
  for (auto hit : *(collection_->GetVector()))
  {
    if (id == hit->id())
    {
      hit->add_edep(edep);
      return true;
    }
  }

  // Otherwise, create a new hit:
  collection_->insert(new CalorimeterHit(id, edep, time, pos));

  return true;
}

//---------------------------------------------------------------------------//
}  // namespace c4h
