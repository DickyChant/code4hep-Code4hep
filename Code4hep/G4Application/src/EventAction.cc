//---------------------------------------------------------------------------//
//! \file Code4hep/G4Application/src/EventAction.cc
//---------------------------------------------------------------------------//
#include "Code4hep/G4Application/EventAction.h"
#include "Code4hep/G4Application/RunAction.h"
#include "Code4hep/G4Application/TrackProvenance.h"

#include <utility>

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "G4Event.hh"

#include <iostream>

namespace c4h {
//---------------------------------------------------------------------------//
/*!
 * Construct with thread-local run action.
 */
EventAction::EventAction(RunAction *runAction,
                         std::shared_ptr<TrackProvenance> provenance)
    : runAction_(runAction), provenance_(std::move(provenance)) {}

void EventAction::BeginOfEventAction(const G4Event *) {
  edep_ = 0.;
  provenance_->clear();
}

void EventAction::EndOfEventAction(const G4Event *event) {
  runAction_->AddEdep(edep_);

  edm::LogVerbatim("Code4hepG4Application")
      << "EventAction::EndOfEventAction Event " << event->GetEventID()
      << ", Total E_dep = " << edep_ << " MeV";

  // Check collection
  auto *hit_cols = event->GetHCofThisEvent();
  if (!hit_cols) {
    return;
  }

  // Note: Write sensitive hits
}

//---------------------------------------------------------------------------//
} // namespace c4h
