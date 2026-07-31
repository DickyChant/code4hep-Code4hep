//---------------------------------------------------------------------------//
//! \file Code4hep/G4Application/src/DetectorConstruction.cc
//---------------------------------------------------------------------------//
#include "Code4hep/G4Application/interface/DetectorConstruction.h"
#include "Code4hep/G4Application/interface/CalorimeterSD.h"
#include "Code4hep/G4Application/interface/TrackerSD.h"

#include "G4VPhysicalVolume.hh"
#include "G4GDMLParser.hh"
#include "G4SDManager.hh"

#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4AutoDelete.hh"

namespace c4h
{
G4ThreadLocal
G4GlobalMagFieldMessenger* DetectorConstruction::fieldMessenger_ = nullptr;

//---------------------------------------------------------------------------//
/*!
 * Construct a detector from a GDML filename
 */
DetectorConstruction::DetectorConstruction(G4String gdmlFile)
  : G4VUserDetectorConstruction()
{
  parser_ = std::make_unique<G4GDMLParser>();
  parser_->Read(gdmlFile, false);
}

DetectorConstruction::~DetectorConstruction()
{
  parser_.reset();
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  return parser_.get()->GetWorldVolume();
}
//---------------------------------------------------------------------------//
/*!
 * Construct thread-local sensitive detectors and field.
 */
void DetectorConstruction::ConstructSDandField()
{
  // Add Sensitive detectors
  G4SDManager* sd_manager = G4SDManager::GetSDMpointer();
  const G4GDMLAuxMapType* aux_map = parser_.get()->GetAuxMap();
  for (auto& aux : *aux_map)
  {
    for (auto const& sd : aux.second)
    {
      if (sd.type != "SensDet")
      {
	continue;
      }
      
      if (sd.value == "si_tracker_sd")
      {
        G4String name = (aux.first)->GetName();
        TrackerSD* tracker_sd = new TrackerSD(name);
        sd_manager->AddNewDetector(tracker_sd);
        (aux.first)->SetSensitiveDetector(tracker_sd);
      }
      if (sd.value == "em_calorimeter_sd")
      {
        G4String name = (aux.first)->GetName();
        CalorimeterSD* calor_sd = new CalorimeterSD(name);
        sd_manager->AddNewDetector(calor_sd);
        (aux.first)->SetSensitiveDetector(calor_sd);
      }
    }
  }

  // Create global magnetic field messenger.
  G4ThreeVector fieldValue(0., 0., 0.1*CLHEP::tesla);
  fieldMessenger_ = new G4GlobalMagFieldMessenger(fieldValue);
  G4AutoDelete::Register(fieldMessenger_);
}
  
//---------------------------------------------------------------------------//
}  // namespace c4h
