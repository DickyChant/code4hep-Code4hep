//---------------------------------------------------------------------------//
//! \file Code4hep/G4Application/PrimaryGeneratorAction.h
//---------------------------------------------------------------------------//
#ifndef Code4hep_G4Application_PrimaryGeneratorAction_h
#define Code4hep_G4Application_PrimaryGeneratorAction_h

#include "G4VUserPrimaryGeneratorAction.hh"

namespace c4h
{
//---------------------------------------------------------------------------//
/*!
 * The primary generator action class.
 *
 * This class is used solely to initialize the Geant4 run manager.
 */
class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  PrimaryGeneratorAction();
  ~PrimaryGeneratorAction() override;

  // Method from the base class
  void GeneratePrimaries(G4Event*) override;
};

//---------------------------------------------------------------------------//
}  // namespace c4h
#endif
