//---------------------------------------------------------------------------//
//! \file Code4hep/G4Application/src/SteppingAction.cc
//---------------------------------------------------------------------------//
#include "Code4hep/G4Application/SteppingAction.h"
#include "Code4hep/G4Application/EventAction.h"

#include "G4Step.hh"

namespace c4h
{
//---------------------------------------------------------------------------//
/*!
 * Construct with thread-local user event action.
 */
SteppingAction::SteppingAction(EventAction* eventAction)
  : eventAction_(eventAction) {}

//---------------------------------------------------------------------------//
/*!
 * Perform user action per step
 */
void SteppingAction::UserSteppingAction(const G4Step* step)
{
  // Collect the total energy deposited in this step
  G4double edepStep = step->GetTotalEnergyDeposit();
  if(edepStep > 0.0)
  {
    eventAction_->AddEdep(edepStep);
  }
}

//---------------------------------------------------------------------------//
}  // namespace c4h
