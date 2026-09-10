//---------------------------------------------------------------------------//
//! \file Code4hep/G4Application/src/ActionInitialization.cc
//---------------------------------------------------------------------------//
#include "Code4hep/G4Application/ActionInitialization.h"
#include "Code4hep/G4Application/EventAction.h"
#include "Code4hep/G4Application/PrimaryGeneratorAction.h"
#include "Code4hep/G4Application/RunAction.h"
#include "Code4hep/G4Application/SteppingAction.h"
#include "Code4hep/G4Application/TrackProvenance.h"
#include "Code4hep/G4Application/TrackingAction.h"

#include <memory>
#include <utility>

namespace c4h {
//---------------------------------------------------------------------------//
/*!
 * Construct actions on the manager thread.
 */
void ActionInitialization::BuildForMaster() const {
  SetUserAction(new RunAction);
}

//---------------------------------------------------------------------------//
/*!
 * Construct actions on each worker thread.
 */
void ActionInitialization::Build() const {
  SetUserAction(new PrimaryGeneratorAction);

  auto runAction = new RunAction;
  SetUserAction(runAction);

  auto provenance = std::make_shared<TrackProvenance>();
  auto eventAction = new EventAction(runAction, provenance);
  SetUserAction(eventAction);

  SetUserAction(new SteppingAction(eventAction));
  SetUserAction(new TrackingAction(std::move(provenance)));
}

//---------------------------------------------------------------------------//
} // namespace c4h
