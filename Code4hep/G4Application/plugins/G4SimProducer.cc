//---------------------------------------------------------------------------//
//! \file Code4hep/G4Application/plugins/G4SimProducer.cc
//---------------------------------------------------------------------------//
#include <iostream>
#include <memory>
#include <thread>

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/ServiceRegistry.h"

#include "Code4hep/G4Application/EventProducts.h"
#include "Code4hep/G4Application/G4MasterInterface.h"
#include "Code4hep/G4Application/G4WorkerInterface.h"
#include "Code4hep/G4Application/RandomSeed.h"
#include "Code4hep/G4Application/ThreadHandoff.h"
#include "Code4hep/Generators/MCParticlesToG4.h"
#include "Code4hep/IOUtilities/FrameParameterConversion.h"

#include "G4Event.hh"
#include <edm4hep/MCParticleCollection.h>
#include <edm4hep/SimCalorimeterHitCollection.h>
#include <edm4hep/SimTrackerHitCollection.h>
#include <podio/UserDataCollection.h>

#include <cstdint>
#include <vector>

namespace {

std::uint32_t simulationSeed(const edm::ParameterSet &config) {
  const auto seed = config.getParameter<unsigned int>("randomSeed");
  if (seed == 0) {
    throw cms::Exception("Configuration")
        << "G4SimProducer randomSeed must be positive";
  }
  return seed;
}

} // namespace

//---------------------------------------------------------------------------//
/*!
 * Stream-based CMS producer that drives Geant4 event processing.
 *
 * The producer owns a thread-local Geant4 worker interface and uses a
 * master interface in the global cache for shared configuration, geometry,
 * and physics setup. Event processing is forwarded into the worker thread
 * via ThreadHandoff to avoid concurrent Geant4 initialization on the wrong
 * thread.
 */
class G4SimProducer
    : public edm::stream::EDProducer<edm::GlobalCache<c4h::G4MasterInterface>,
                                     edm::RunCache<int>,
                                     edm::stream::WatchRuns> {
public:
  explicit G4SimProducer(const edm::ParameterSet &,
                         const c4h::G4MasterInterface *);
  ~G4SimProducer() override;

  static std::unique_ptr<c4h::G4MasterInterface>
  initializeGlobalCache(const edm::ParameterSet &iConfig);

  static std::shared_ptr<int>
  globalBeginRun(const edm::Run &iRun, const edm::EventSetup &iSetup,
                 const c4h::G4MasterInterface *masterThread);

  static void globalEndRun(const edm::Run &iRun, const edm::EventSetup &iSetup,
                           const RunContext *iContext);

  static void globalEndJob(c4h::G4MasterInterface *masterThread);

private:
  void produce(edm::Event &, const edm::EventSetup &) override;
  void beginRun(edm::Run const &, edm::EventSetup const &) override;
  void endRun(edm::Run const &, edm::EventSetup const &) override;

private:
  omt::ThreadHandoff m_handoff;
  const c4h::G4MasterInterface *m_masterThread;
  std::unique_ptr<c4h::G4WorkerInterface> m_workerInterface;
  std::uint32_t randomSeed_;

  edm::EDGetTokenT<edm4hep::MCParticleCollection> mcToken_;
  edm::EDPutTokenT<edm4hep::SimTrackerHitCollection> trackerHitsToken_;
  edm::EDPutTokenT<edm4hep::SimCalorimeterHitCollection> calorimeterHitsToken_;
  edm::EDPutTokenT<podio::UserDataCollection<double>> magneticFieldToken_;
};

//---------------------------------------------------------------------------//
// INLINE DEFINITIONS
//---------------------------------------------------------------------------//
std::unique_ptr<c4h::G4MasterInterface>
G4SimProducer::initializeGlobalCache(const edm::ParameterSet &iConfig) {
  return std::make_unique<c4h::G4MasterInterface>(iConfig);
}

G4SimProducer::G4SimProducer(const edm::ParameterSet &p,
                             const c4h::G4MasterInterface *masterThread)
    : m_handoff{p.getUntrackedParameter<int>("workerThreadStackSize",
                                             10 * 1024 * 1024)},
      m_masterThread(masterThread), randomSeed_(simulationSeed(p)),
      mcToken_(consumes(p.getParameter<edm::InputTag>("generator"))),
      trackerHitsToken_(
          produces<edm4hep::SimTrackerHitCollection>("SimTrackerHits")),
      calorimeterHitsToken_(
          produces<edm4hep::SimCalorimeterHitCollection>("SimCalorimeterHits")),
      magneticFieldToken_(produces<podio::UserDataCollection<double>>(
          c4h::frameParameterCollectionName(
              'D', "sim_detector_magneticFieldTesla"))) {
  // Construct the worker interface on the worker thread using the same
  // thread stack semantics as later event processing.
  auto token = edm::ServiceRegistry::instance().presentToken();
  m_handoff.runAndWait([this, &p, token]() {
    edm::ServiceRegistry::Operate guard{token};
    m_workerInterface = std::make_unique<c4h::G4WorkerInterface>();
  });
}

G4SimProducer::~G4SimProducer() {
  // Reset the worker interface on the worker thread to keep all Geant4
  // lifecycle operations bound to the same thread context.
  auto token = edm::ServiceRegistry::instance().presentToken();
  m_handoff.runAndWait([this, token]() {
    edm::ServiceRegistry::Operate guard{token};
    m_workerInterface.reset();
  });
}

//---------------------------------------------------------------------------//
// MEMBER FUNCTIONS
//---------------------------------------------------------------------------//
std::shared_ptr<int>
G4SimProducer::globalBeginRun(const edm::Run &iRun,
                              const edm::EventSetup &iSetup,
                              const c4h::G4MasterInterface *masterThread) {
  if (masterThread) {
    masterThread->beginRun();
  }

  return std::shared_ptr<int>();
}

void G4SimProducer::globalEndRun(const edm::Run &iRun,
                                 const edm::EventSetup &iSetup,
                                 const RunContext *iContext) {
  if (nullptr != iContext->global()) {
    iContext->global()->endRun();
  }
}

void G4SimProducer::globalEndJob(c4h::G4MasterInterface *masterThread) {
  if (masterThread) {
    masterThread->stopThread();
  }
}

// Method called to produce the data
void G4SimProducer::produce(edm::Event &e, const edm::EventSetup &es) {
  using namespace edm;

  auto token = edm::ServiceRegistry::instance().presentToken();

  c4h::EventProducts products;
  m_handoff.runAndWait([this, &e, &products, token]() {
    edm::ServiceRegistry::Operate guard{token};

    G4int evtid = static_cast<G4int>(e.id().event());
    auto const &genEvent = e.get(mcToken_);
    auto g4evt = c4h::MCParticlesToG4(genEvent, evtid);

    // Forward the converted EDM event into the Geant4 worker
    m_workerInterface->produce(
        g4evt.get(),
        c4h::geantEventSeed(randomSeed_, e.id().run(), e.id().event()));
    products = c4h::makeEventProducts(*g4evt);
  });

  e.emplace(trackerHitsToken_, std::move(*products.trackerHits));
  e.emplace(calorimeterHitsToken_, std::move(*products.calorimeterHits));
  e.emplace(magneticFieldToken_,
            std::vector<double>{m_masterThread->magneticFieldTesla()});
}

// Method called when starting to processes a run
void G4SimProducer::beginRun(edm::Run const &, edm::EventSetup const &es) {
  auto token = edm::ServiceRegistry::instance().presentToken();
  m_handoff.runAndWait([this, &es, token]() {
    edm::ServiceRegistry::Operate guard{token};
    m_workerInterface->initializeG4(m_masterThread->runManagerMasterPtr());
  });
}

// Method called when ending the processing of a run
void G4SimProducer::endRun(edm::Run const &, edm::EventSetup const &es) {
  auto token = edm::ServiceRegistry::instance().presentToken();
  m_handoff.runAndWait([this, &es, token]() {
    edm::ServiceRegistry::Operate guard{token};
    m_workerInterface->endRun();
  });
}

// define this as a plug-in
DEFINE_FWK_MODULE(G4SimProducer);
