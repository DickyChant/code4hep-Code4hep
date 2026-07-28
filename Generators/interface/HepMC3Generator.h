//---------------------------------------------------------------------------//
//! \file Code4hep/Generator3/interface/MepMC3Generator.h
//---------------------------------------------------------------------------//
#pragma once

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "Code4hep/Generators/interface/Generator.h"

#include "HepMC3/GenEvent.h"

namespace c4h
{
//---------------------------------------------------------------------------//
/*!
 * Generate a HepMC3 event using a particle gun and convert it into 
 * an edm4hep MCParticle collection.
 */
class HepMC3Generator final : public Generator
{
public:
  explicit HepMC3Generator(const edm::ParameterSet &pset);
  virtual ~HepMC3Generator() override = default;

  // Generate a HepMC3 event.
  Generator::UPMCParticle operator()(CLHEP::HepRandomEngine* rng) final;

private:
  // Convert a HepMC3 event into an edm4hep MCParticle collection.
  void convertHepMC3ToEDM4hep(const HepMC3::GenEvent& hepmcEvent,
                              edm4hep::MCParticleCollection& mcParticles);
  
private:
  int    verbose_{};
  int    pdgcode_{};

  double min_pt_{};
  double max_pt_{};
  double min_eta_{};
  double max_eta_{};
  double min_phi_{};
  double max_phi_{};
};

//---------------------------------------------------------------------------//
}  // namespace c4h
