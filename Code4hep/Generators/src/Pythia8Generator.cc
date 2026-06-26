//---------------------------------------------------------------------------//
//! \file Code4hep/Generators/src/Pythia8Generator.cc
//---------------------------------------------------------------------------//
#include "Code4hep/Generators/Pythia8Generator.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include <vector>

namespace c4h
{
//---------------------------------------------------------------------------//
/*!
 * Construct from parameter set.
 */
Pythia8Generator::Pythia8Generator(const edm::ParameterSet& p)
{
  const auto& commands =
      p.getParameter<std::vector<std::string>>("Pythia8Parameters");

  for (const auto& cmd : commands)
  {
    edm::LogVerbatim("Code4hepGenerators") << "Pythia8Parameters: " << cmd;
    pythia_.readString(cmd);
  }

  pythia_.init();
}

//---------------------------------------------------------------------------//
/*!
 * Generate a Pythia8 event and return edm::MCParticleCollection.
 */
auto Pythia8Generator::operator()(CLHEP::HepRandomEngine*) -> UPMCParticle
{
  auto particles = std::make_unique<edm4hep::MCParticleCollection>();

  // Generate a Pythia8 event
  if (!pythia_.next())
  {
    return particles;
  }

  // Convert it to edm::MCParticleCollection. 
  convertPythiaEvent(pythia_.event, *particles);

  return particles;
}

//---------------------------------------------------------------------------//
/*!
 * Convert a Pythia8 event to edm::MCParticleCollection.
 */
void Pythia8Generator::convertPythiaEvent(const Pythia8::Event& event,
                                      edm4hep::MCParticleCollection& particles)
{
  const int nParticles = event.size();
  
  std::vector<edm4hep::MutableMCParticle> edmParticles;
  edmParticles.reserve(nParticles);

  // First pass: create particles
  for (int i = 0; i < nParticles; ++i)
  {
    const auto& p = event[i];
    auto mc = particles.create();

    mc.setPDG(p.id());
    mc.setGeneratorStatus(p.status());
    mc.setCharge(static_cast<float>(p.charge()));
    mc.setMass(static_cast<float>(p.m()));
    mc.setMomentum(edm4hep::Vector3d{p.px(), p.py(), p.pz()});
    mc.setVertex(edm4hep::Vector3d{p.xProd(), p.yProd(), p.zProd()});

    edmParticles.push_back(mc);
  }

  // Second pass: establish relations
  for (int i = 0; i < nParticles; ++i)
  {
    const auto& p = event[i];

    auto mc = edmParticles[i];

    // Mothers
    int m1 = p.mother1();
    int m2 = p.mother2();

    if (m1 > 0)
    {
      mc.addToParents(edmParticles[m1]);
    }
    if (m2 > 0 && m2 != m1)
    {
      mc.addToParents(edmParticles[m2]);
    }

    // Daughters
    int d1 = p.daughter1();
    int d2 = p.daughter2();

    if (d1 > 0)
    {
      for (int d = d1; d <= d2; ++d)
      {
        mc.addToDaughters(edmParticles[d]);
      }
    }
  }
}

//---------------------------------------------------------------------------//
}  // namespace c4h
