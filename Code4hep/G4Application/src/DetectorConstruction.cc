//---------------------------------------------------------------------------//
//! \file Code4hep/G4Application/src/DetectorConstruction.cc
//---------------------------------------------------------------------------//
#include "Code4hep/G4Application/DetectorConstruction.h"
#include "Code4hep/G4Application/CalorimeterSD.h"
#include "Code4hep/G4Application/TrackerSD.h"

#include "G4GDMLParser.hh"
#include "G4LogicalVolume.hh"
#include "G4SDManager.hh"
#include "G4UIcommand.hh"
#include "G4UserLimits.hh"
#include "G4VPhysicalVolume.hh"

#include "G4AutoDelete.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"

#include <stdexcept>

namespace c4h {
G4ThreadLocal G4GlobalMagFieldMessenger *DetectorConstruction::fieldMessenger_ =
    nullptr;

//---------------------------------------------------------------------------//
/*!
 * Construct a detector from a GDML filename
 */
DetectorConstruction::DetectorConstruction(G4String gdmlFile,
                                           double magneticFieldTesla)
    : G4VUserDetectorConstruction(), magneticFieldTesla_(magneticFieldTesla) {
  parser_ = std::make_unique<G4GDMLParser>();
  parser_->Read(gdmlFile, false);
}

DetectorConstruction::~DetectorConstruction() { parser_.reset(); }

G4VPhysicalVolume *DetectorConstruction::Construct() {
  const auto *auxMap = parser_->GetAuxMap();
  for (const auto &aux : *auxMap) {
    for (const auto &setting : aux.second) {
      if (setting.type != "StepLimit") {
        continue;
      }
      const auto unit = setting.unit.empty()
                            ? 1.0
                            : G4UIcommand::ValueOf(setting.unit.c_str());
      const auto maximumStep = std::stod(setting.value) * unit;
      if (maximumStep <= 0) {
        throw std::runtime_error("GDML StepLimit must be positive");
      }
      auto limits = std::make_unique<G4UserLimits>(maximumStep);
      aux.first->SetUserLimits(limits.get());
      userLimits_.push_back(std::move(limits));
    }
  }
  return parser_.get()->GetWorldVolume();
}
//---------------------------------------------------------------------------//
/*!
 * Construct thread-local sensitive detectors and field.
 */
void DetectorConstruction::ConstructSDandField() {
  // Add Sensitive detectors
  G4SDManager *sd_manager = G4SDManager::GetSDMpointer();
  const G4GDMLAuxMapType *aux_map = parser_.get()->GetAuxMap();
  for (auto &aux : *aux_map) {
    for (auto const &sd : aux.second) {
      if (sd.type != "SensDet") {
        continue;
      }

      if (sd.value == "si_tracker_sd") {
        G4String name = (aux.first)->GetName();
        TrackerSD *tracker_sd = new TrackerSD(name);
        sd_manager->AddNewDetector(tracker_sd);
        (aux.first)->SetSensitiveDetector(tracker_sd);
      }
      if (sd.value == "step_tracker_sd") {
        G4String name = (aux.first)->GetName();
        TrackerSD *tracker_sd = new TrackerSD(name, false);
        sd_manager->AddNewDetector(tracker_sd);
        (aux.first)->SetSensitiveDetector(tracker_sd);
      }
      if (sd.value == "em_calorimeter_sd") {
        G4String name = (aux.first)->GetName();
        CalorimeterSD *calor_sd = new CalorimeterSD(name);
        sd_manager->AddNewDetector(calor_sd);
        (aux.first)->SetSensitiveDetector(calor_sd);
      }
    }
  }

  // Create global magnetic field messenger.
  G4ThreeVector fieldValue(0., 0., magneticFieldTesla_ * CLHEP::tesla);
  fieldMessenger_ = new G4GlobalMagFieldMessenger(fieldValue);
  G4AutoDelete::Register(fieldMessenger_);
}

//---------------------------------------------------------------------------//
} // namespace c4h
