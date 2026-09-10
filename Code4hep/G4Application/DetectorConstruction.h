//---------------------------------------------------------------------------//
//! \file Code4hep/G4Application/DetectorConstruction.h
//---------------------------------------------------------------------------//
#ifndef Code4hep_G4Application_DetectorConstruction_h
#define Code4hep_G4Application_DetectorConstruction_h

#include "G4VUserDetectorConstruction.hh"

class G4GDMLParser;
class G4String;
class G4VPhysicalVolume;
class G4GlobalMagFieldMessenger;
class G4UserLimits;

#include <memory>
#include <vector>

namespace c4h {
//---------------------------------------------------------------------------//
/*!
 * Detector construction class to define materials and geometry.
 */
class DetectorConstruction : public G4VUserDetectorConstruction {
public:
  DetectorConstruction(G4String gdmlName, double magneticFieldTesla);
  virtual ~DetectorConstruction();

  G4VPhysicalVolume *Construct() final;
  void ConstructSDandField() final;

private:
  std::unique_ptr<G4GDMLParser> parser_;
  std::vector<std::unique_ptr<G4UserLimits>> userLimits_;
  double magneticFieldTesla_;

  // Static data members
  static G4ThreadLocal G4GlobalMagFieldMessenger *fieldMessenger_;
};

//---------------------------------------------------------------------------//
} // namespace c4h
#endif
