#include "catch2/catch_all.hpp"

#include "Code4hep/G4Application/CalorimeterHit.h"
#include "Code4hep/G4Application/EventProducts.h"
#include "Code4hep/G4Application/RandomSeed.h"
#include "Code4hep/G4Application/TrackerHit.h"

#include "G4Event.hh"
#include "G4HCofThisEvent.hh"
#include "G4SystemOfUnits.hh"

#include <cstdint>

TEST_CASE("Geant4 hits become EDM4hep event products", "[G4SimProducer]") {
  G4Event event{7};
  auto *eventHits = new G4HCofThisEvent{2};

  auto *tracker = new c4h::TrackerHitsCollection{"tracker", "tracker_HC"};
  tracker->insert(new c4h::TrackerHit{
      11,
      42,
      2.5 * CLHEP::MeV,
      4.0 * CLHEP::ns,
      7.0 * CLHEP::mm,
      {1.0 * CLHEP::mm, 2.0 * CLHEP::mm, 3.0 * CLHEP::mm},
      {4.0 * CLHEP::GeV, 5.0 * CLHEP::GeV, 6.0 * CLHEP::GeV}});
  eventHits->AddHitsCollection(0, tracker);

  auto *calorimeter =
      new c4h::CalorimeterHitsCollection{"calorimeter", "calorimeter_HC"};
  calorimeter->insert(new c4h::CalorimeterHit{
      12,
      1.25 * CLHEP::GeV,
      8.0 * CLHEP::ns,
      {9.0 * CLHEP::mm, 10.0 * CLHEP::mm, 11.0 * CLHEP::mm}});
  eventHits->AddHitsCollection(1, calorimeter);
  event.SetHCofThisEvent(eventHits);

  auto products = c4h::makeEventProducts(event);
  REQUIRE(products.trackerHits->size() == 1);
  REQUIRE(products.calorimeterHits->size() == 1);

  const auto trackerHit = products.trackerHits->at(0);
  REQUIRE(trackerHit.getCellID() == 11);
  REQUIRE(trackerHit.getEDep() == Catch::Approx(0.0025));
  REQUIRE(trackerHit.getTime() == Catch::Approx(4.0));
  REQUIRE(trackerHit.getPathLength() == Catch::Approx(7.0));
  REQUIRE(trackerHit.getPosition().z == Catch::Approx(3.0));
  REQUIRE(trackerHit.getMomentum().y == Catch::Approx(5.0));

  const auto calorimeterHit = products.calorimeterHits->at(0);
  REQUIRE(calorimeterHit.getCellID() == (std::uint64_t{1} << 32U) + 12U);
  REQUIRE(calorimeterHit.getEnergy() == Catch::Approx(1.25));
  REQUIRE(calorimeterHit.getPosition().x == Catch::Approx(9.0));
}

TEST_CASE("Geant4 event seeds are stable and event-specific",
          "[G4SimProducer]") {
  constexpr auto first = c4h::geantEventSeed(67890, 1, 1);
  static_assert(first == c4h::geantEventSeed(67890, 1, 1));
  REQUIRE(first > 0);
  REQUIRE(first <= 900000000);
  REQUIRE(first != c4h::geantEventSeed(67890, 1, 2));
  REQUIRE(first != c4h::geantEventSeed(67890, 2, 1));
}
