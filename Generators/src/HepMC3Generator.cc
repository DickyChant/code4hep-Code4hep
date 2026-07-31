//---------------------------------------------------------------------------//
//! \file Code4hep/Generators/src/HepMC3Generator.cc
//---------------------------------------------------------------------------//
#include "Code4hep/Generators/interface/HepMC3Generator.h"

#include "G4ParticleTable.hh"
#include <CLHEP/Units/SystemOfUnits.h>

#include "HepMC3/GenEvent.h"
#include "HepMC3/GenParticle.h"
#include "HepMC3/GenVertex.h"
#include "HepMC3/FourVector.h"

namespace c4h
{
//---------------------------------------------------------------------------//
/*!
 * Construct from parameter set.
 */
HepMC3Generator::HepMC3Generator(const edm::ParameterSet &p)
    : verbose_(p.getUntrackedParameter<int>("Verbosity", 0)),
      pdgcode_(p.getUntrackedParameter<int>("PartID")),
      min_pt_(p.getParameter<double>("MinPt")),  // in GeV 
      max_pt_(p.getParameter<double>("MaxPt")),
      min_eta_(p.getParameter<double>("MinEta")),
      max_eta_(p.getParameter<double>("MaxEta")),
      min_phi_(p.getParameter<double>("MinPhi")),  // in radians
      max_phi_(p.getParameter<double>("MaxPhi"))
{
}
  
//---------------------------------------------------------------------------//
/*!
 * Generate a HepMC3 event and return edm::MCParticleCollection.
 */
auto HepMC3Generator::operator()(CLHEP::HepRandomEngine* rng) -> UPMCParticle
{
  // Create a HepMC3 event and the primary vertex
  auto evt = std::make_unique<HepMC3::GenEvent>();
  auto vtx = std::make_shared<HepMC3::GenVertex>(HepMC3::FourVector{});

  double pt  = CLHEP::RandFlat::shoot(rng, min_pt_, max_pt_);
  double eta = CLHEP::RandFlat::shoot(rng, min_eta_, max_eta_);
  double phi = CLHEP::RandFlat::shoot(rng, min_phi_, max_phi_);

  //TODO: Use HEPPDT?
  auto particle = G4ParticleTable::GetParticleTable()->FindParticle(pdgcode_);
  double mass = particle->GetPDGMass()/CLHEP::MeV;

  double theta = 2. * atan(exp(-eta));
  double mom = pt / sin(theta);
  double px = pt * cos(phi);
  double py = pt * sin(phi);
  double pz = mom * cos(theta);
  double energy = sqrt(mom * mom + mass * mass);

  HepMC3::FourVector p(px, py, pz, energy);

  auto gen_particle = std::make_shared<HepMC3::GenParticle>(p, pdgcode_, 1);

  vtx->add_particle_out(gen_particle);
  evt->add_vertex(vtx);

  auto mcParticles = std::make_unique<edm4hep::MCParticleCollection>();
  this->convertHepMC3ToEDM4hep(*evt, *mcParticles);

  return mcParticles;
}

//---------------------------------------------------------------------------//
/*!
 * Convert a HepMC3 event to edm::MCParticleCollection.
 */
void HepMC3Generator::convertHepMC3ToEDM4hep(
    const HepMC3::GenEvent& hepmcEvent,
    edm4hep::MCParticleCollection& mcParticles)
{
  // Map HepMC3 particle pointer -> EDM4hep particle
  std::unordered_map<const HepMC3::GenParticle*, edm4hep::MutableMCParticle>
      particleMap;

  // Create EDM4hep particles
  for (const auto& p : hepmcEvent.particles())
  {
    auto edmParticle = mcParticles.create();

    edmParticle.setPDG(p->pid());
    edmParticle.setGeneratorStatus(p->status());

    const auto& mom = p->momentum();

    edmParticle.setMomentum(edm4hep::Vector3d{mom.px(), mom.py(), mom.pz()});
    edmParticle.setMass(p->generated_mass());

    // production vertex
    if (p->production_vertex())
    {
      const auto& v = p->production_vertex()->position();
      edmParticle.setVertex(edm4hep::Vector3d{v.x(), v.y(), v.z()});
      edmParticle.setTime(static_cast<float>(v.t()));
    }

    particleMap[p.get()] = edmParticle;
  }

  // Build parent/daughter relations
  for (const auto& p : hepmcEvent.particles())
  {
    auto edmParticle = particleMap[p.get()];

    if (!p->production_vertex())
    {  
        continue;
    }
    
    for (const auto& parent : p->production_vertex()->particles_in())
    {
      auto parentEDM = particleMap[parent.get()];

      edmParticle.addToParents(parentEDM);
      parentEDM.addToDaughters(edmParticle);
    }
  }
}

//---------------------------------------------------------------------------//
}  // namespace c4h
