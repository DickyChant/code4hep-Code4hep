#include "Code4hep/G4Application/EventProducts.h"

#include "Code4hep/G4Application/CalorimeterHit.h"
#include "Code4hep/G4Application/TrackerHit.h"

#include "G4Event.hh"
#include "G4HCofThisEvent.hh"
#include "G4SystemOfUnits.hh"

#include <cstdint>

namespace c4h {
namespace {

std::uint64_t cellID(std::size_t collectionIndex, unsigned int copyNumber) {
  return (static_cast<std::uint64_t>(collectionIndex) << 32U) |
         static_cast<std::uint64_t>(copyNumber);
}

} // namespace

EventProducts
makeEventProducts(G4Event &event,
                  const edm4hep::MCParticleCollection &particles) {
  EventProducts products{
      std::make_unique<edm4hep::SimTrackerHitCollection>(),
      std::make_unique<edm4hep::SimCalorimeterHitCollection>()};

  auto *eventHits = event.GetHCofThisEvent();
  if (eventHits == nullptr)
    return products;

  for (std::size_t index = 0; index < eventHits->GetCapacity(); ++index) {
    auto *collection = eventHits->GetHC(static_cast<G4int>(index));
    if (auto *tracker = dynamic_cast<TrackerHitsCollection *>(collection)) {
      for (const auto *source : *tracker->GetVector()) {
        auto hit = products.trackerHits->create();
        hit.setCellID(
            source->explicitCellID()
                ? source->id()
                : cellID(index, static_cast<unsigned int>(source->id())));
        hit.setEDep(static_cast<float>(source->energyDeposit() / CLHEP::GeV));
        hit.setTime(static_cast<float>(source->time() / CLHEP::ns));
        hit.setPathLength(static_cast<float>(source->pathLength() / CLHEP::mm));
        hit.setPosition({source->pos().x() / CLHEP::mm,
                         source->pos().y() / CLHEP::mm,
                         source->pos().z() / CLHEP::mm});
        hit.setMomentum(
            {static_cast<float>(source->momentum().x() / CLHEP::GeV),
             static_cast<float>(source->momentum().y() / CLHEP::GeV),
             static_cast<float>(source->momentum().z() / CLHEP::GeV)});
        if (source->mcParticleIndex() >= 0 &&
            static_cast<std::size_t>(source->mcParticleIndex()) <
                particles.size()) {
          hit.setParticle(particles[source->mcParticleIndex()]);
        }
      }
    } else if (auto *calorimeter =
                   dynamic_cast<CalorimeterHitsCollection *>(collection)) {
      for (const auto *source : *calorimeter->GetVector()) {
        auto hit = products.calorimeterHits->create();
        hit.setCellID(cellID(index, source->id()));
        hit.setEnergy(static_cast<float>(source->edep() / CLHEP::GeV));
        hit.setPosition({static_cast<float>(source->pos().x() / CLHEP::mm),
                         static_cast<float>(source->pos().y() / CLHEP::mm),
                         static_cast<float>(source->pos().z() / CLHEP::mm)});
      }
    }
  }

  return products;
}

} // namespace c4h
