//---------------------------------------------------------------------------//
//! \file Code4hep/G4Application/src/PrimaryGeneratorAction.cc
//---------------------------------------------------------------------------//
#include "Code4hep/G4Application/interface/PrimaryGeneratorAction.h"

#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"

namespace c4h
{
//---------------------------------------------------------------------------//
/*!
 * Construct from a shared generator.
 */
PrimaryGeneratorAction::PrimaryGeneratorAction()
{
  G4int n_particle = 1;
  particleGun_ = new G4ParticleGun(n_particle);

  // default particle kinematic
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particle = particleTable->FindParticle("gamma");
  particleGun_->SetParticleDefinition(particle);
  particleGun_->SetParticleMomentumDirection(G4ThreeVector(1., 0., 0.));
  particleGun_->SetParticleEnergy(10. * CLHEP::MeV);
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete particleGun_;
}

//---------------------------------------------------------------------------//
/*!
 * Generate primaries from a generator.
 */
void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
  // This function is called at the begining of ecah event
  particleGun_->SetParticlePosition(G4ThreeVector(0., 0., 0.));
  particleGun_->GeneratePrimaryVertex(event);
}

//---------------------------------------------------------------------------//
}  // namespace c4h
