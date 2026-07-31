//---------------------------------------------------------------------------//
//! \file Code4hep/Generators/interface/Pythia8Generator.h
//---------------------------------------------------------------------------//
#ifndef Code4hep_Generators_Pythia8Generator_h
#define Code4hep_Generators_Pythia8Generator_h

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "Code4hep/Generators/interface/Generator.h"

#include "Pythia8/Pythia.h"

namespace c4h
{
//---------------------------------------------------------------------------//
/*!
 * Generate a Pythia8 event using pythia gand convert it into an edm4hep 
 * MCParticle collection.
 */
class Pythia8Generator : public Generator
{
public:
  explicit Pythia8Generator(const edm::ParameterSet& p);
  ~Pythia8Generator() override = default;

  // Generate a Pythia8 event.
  Generator::UPMCParticle operator()(CLHEP::HepRandomEngine* rng) override;

private:
  // Convert a Pythia8 event into an edm4hep MCParticle collection.
  void convertPythiaEvent(const Pythia8::Event& event,
                          edm4hep::MCParticleCollection& particles);

private:
  Pythia8::Pythia pythia_;
};

//---------------------------------------------------------------------------//
} // namespace c4h  
#endif
