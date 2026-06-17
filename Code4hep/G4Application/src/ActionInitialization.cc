//---------------------------------------------------------------------------//
//! \file Code4hep/G4Application/src/ActionInitialization.cc
//---------------------------------------------------------------------------//
#include "Code4hep/G4Application/interface/ActionInitialization.h"
#include "Code4hep/G4Application/interface/EventAction.h"
#include "Code4hep/G4Application/interface/PrimaryGeneratorAction.h"
#include "Code4hep/G4Application/interface/RunAction.h"
#include "Code4hep/G4Application/interface/SteppingAction.h"

namespace c4h
{
//---------------------------------------------------------------------------//
/*!
 * Construct actions on the manager thread.
 */
void ActionInitialization::BuildForMaster() const
{
  SetUserAction(new RunAction);
}

//---------------------------------------------------------------------------//
/*!
 * Construct actions on each worker thread.
 */
void ActionInitialization::Build() const
{
  SetUserAction(new PrimaryGeneratorAction);

  auto runAction = new RunAction;
  SetUserAction(runAction);

  auto eventAction = new EventAction(runAction);
  SetUserAction(eventAction);

  SetUserAction(new SteppingAction(eventAction));
}

//---------------------------------------------------------------------------//
}  // namespace c4h
