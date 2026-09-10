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

#include <cstdint>
#include <limits>
#include <optional>
#include <stdexcept>
#include <string>

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
    std::string sensitiveDetector;
    std::optional<std::uint64_t> cellIDBase;
    for (const auto &setting : aux.second) {
      if (setting.type == "SensDet") {
        if (!sensitiveDetector.empty()) {
          throw std::runtime_error(
              "GDML logical volume has multiple SensDet settings");
        }
        sensitiveDetector = setting.value;
      } else if (setting.type == "CellIDBase") {
        if (cellIDBase.has_value()) {
          throw std::runtime_error(
              "GDML logical volume has multiple CellIDBase settings");
        }
        std::size_t consumed{};
        try {
          cellIDBase = std::stoull(setting.value, &consumed, 0);
        } catch (const std::exception &) {
          throw std::runtime_error("GDML CellIDBase is not an integer");
        }
        if (consumed != setting.value.size() ||
            (*cellIDBase & std::numeric_limits<std::uint32_t>::max()) != 0) {
          throw std::runtime_error(
              "GDML CellIDBase must reserve its low 32 bits for copy number");
        }
      }
    }
    if (sensitiveDetector.empty()) {
      if (cellIDBase.has_value()) {
        throw std::runtime_error(
            "GDML CellIDBase requires a SensDet on the same logical volume");
      }
      continue;
    }

    const G4String name = aux.first->GetName();
    if (sensitiveDetector == "si_tracker_sd") {
      auto *trackerSD = new TrackerSD(name, true, cellIDBase);
      sd_manager->AddNewDetector(trackerSD);
      aux.first->SetSensitiveDetector(trackerSD);
    } else if (sensitiveDetector == "step_tracker_sd") {
      auto *trackerSD = new TrackerSD(name, false, cellIDBase);
      sd_manager->AddNewDetector(trackerSD);
      aux.first->SetSensitiveDetector(trackerSD);
    } else if (sensitiveDetector == "em_calorimeter_sd") {
      if (cellIDBase.has_value()) {
        throw std::runtime_error(
            "GDML CellIDBase is currently supported only for tracker hits");
      }
      auto *calorimeterSD = new CalorimeterSD(name);
      sd_manager->AddNewDetector(calorimeterSD);
      aux.first->SetSensitiveDetector(calorimeterSD);
    }
  }

  // Create global magnetic field messenger.
  G4ThreeVector fieldValue(0., 0., magneticFieldTesla_ * CLHEP::tesla);
  fieldMessenger_ = new G4GlobalMagFieldMessenger(fieldValue);
  G4AutoDelete::Register(fieldMessenger_);
}

//---------------------------------------------------------------------------//
} // namespace c4h
