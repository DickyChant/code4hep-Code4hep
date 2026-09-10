//---------------------------------------------------------------------------//
//! \file Code4hep/Generator/plugins/GenProducer.cc
//---------------------------------------------------------------------------//
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/Run.h"
#include "FWCore/Framework/interface/one/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "Code4hep/Generators/Generator.h"
#include "Code4hep/Generators/HepMC3Generator.h"
#include "Code4hep/Generators/Pythia8Generator.h"
#include "Code4hep/PodioUtilities/setCollectionID.h"

#include "CLHEP/Random/JamesRandom.h"
#include <edm4hep/MCParticleCollection.h>

using namespace edm;

namespace {

unsigned int initialSeed(const ParameterSet &pset) {
  const auto seed = pset.getParameter<unsigned int>("initialSeed");
  if (seed == 0) {
    throw cms::Exception("Configuration")
        << "GenProducer initialSeed must be positive";
  }
  return seed;
}

} // namespace

//---------------------------------------------------------------------------//
/*!
 * Code4hep producer that drives an edm4hep MC event generation.
 */
class GenProducer : public one::EDProducer<one::WatchRuns, EndRunProducer> {
public:
  GenProducer(const ParameterSet &pset);

  void beginRun(const edm::Run &, const edm::EventSetup &) override {};
  void endRun(edm::Run const &, const edm::EventSetup &) override {};
  void endRunProduce(edm::Run &, const edm::EventSetup &) override {};

  void produce(edm::Event &event, const edm::EventSetup &es) override;

private:
  edm::EDPutTokenT<edm4hep::MCParticleCollection> mcParticlesToken_;
  std::unique_ptr<c4h::Generator> generator_;
  std::unique_ptr<CLHEP::HepRandomEngine> randomEngine_;
};

//---------------------------------------------------------------------------//
// INLINE DEFINITIONS
//---------------------------------------------------------------------------//
GenProducer::GenProducer(const ParameterSet &pset)
    : mcParticlesToken_(produces<edm4hep::MCParticleCollection>("MCParticles")),
      randomEngine_(
          std::make_unique<CLHEP::HepJamesRandom>(initialSeed(pset))) {
  auto type = pset.getParameter<std::string>("generatorType");

  if (type == "HepMC3Generator") {
    generator_ = std::make_unique<c4h::HepMC3Generator>(pset);
  } else if (type == "Pythia8Generator") {
    generator_ = std::make_unique<c4h::Pythia8Generator>(pset);
  } else {
    throw cms::Exception("Configuration") << "Unknown generatorType = " << type;
  }
}

void GenProducer::produce(edm::Event &event, const edm::EventSetup &es) {
  // Generate a edm4hep::MCParticleCollections and store it into edm::Event
  // GenProducer is an edm::one module, so its owned engine is never used
  // concurrently and does not require the CMSSW-only RNG service package.
  auto mcParticles = (*generator_)(randomEngine_.get());

  c4h::setCollectionID(*mcParticles, event, *this, mcParticlesToken_);
  event.emplace(mcParticlesToken_, std::move(*mcParticles));
}

// define this as a plug-in
DEFINE_FWK_MODULE(GenProducer);
