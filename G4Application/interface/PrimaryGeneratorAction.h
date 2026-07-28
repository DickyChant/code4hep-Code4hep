//---------------------------------------------------------------------------//
//! \file Code4hep/G4Application/interface/PrimaryGeneratorAction.h
//---------------------------------------------------------------------------//
#pragma once

#include "G4VUserPrimaryGeneratorAction.hh"

class G4ParticleGun;
class G4Event;
class G4Box;

namespace c4h
{
//---------------------------------------------------------------------------//
/*!
 * The primary generator action class with particle gun.
 */
class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    PrimaryGeneratorAction();
    ~PrimaryGeneratorAction() override;

    // Method from the base class
    void GeneratePrimaries(G4Event*) override;

    // Method to access particle gun
    const G4ParticleGun* GetParticleGun() const { return particleGun_; }

  private:
    G4ParticleGun* particleGun_ = nullptr;
};

//---------------------------------------------------------------------------//
}  // namespace c4h
