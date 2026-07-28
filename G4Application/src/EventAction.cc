//---------------------------------------------------------------------------//
//! \file Code4hep/G4Application/src/EventAction.cc
//---------------------------------------------------------------------------//
#include "Code4hep/G4Application/interface/EventAction.h"
#include "Code4hep/G4Application/interface/RunAction.h"

#include "G4Event.hh"

#include <iostream>

namespace c4h
{
//---------------------------------------------------------------------------//
/*!
 * Construct with thread-local run action.
 */
EventAction::EventAction(RunAction* runAction)
  : runAction_(runAction) {}

void EventAction::BeginOfEventAction(const G4Event*)
{
  edep_ = 0.;
}

void EventAction::EndOfEventAction(const G4Event* event)
{
  runAction_->AddEdep(edep_);

  std::cout << "EventAction::EndOfEventAction Event " << event->GetEventID()
            << ", Total E_dep = " << edep_ << " MeV" << std::endl;
  
  // Check collection
  auto* hit_cols = event->GetHCofThisEvent();
  if (!hit_cols)
  {
    return;
  }

  // Note: Write sensitive hits
}

//---------------------------------------------------------------------------//
}  // namespace c4h
