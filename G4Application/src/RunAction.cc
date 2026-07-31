//---------------------------------------------------------------------------//
//! \file Code4hep/G4Application/src/RunAction.cc
//---------------------------------------------------------------------------//
#include "Code4hep/G4Application/interface/RunAction.h"

#include "G4Run.hh"

namespace c4h
{
//---------------------------------------------------------------------------//
/*!
 * Construct with a shared run.
 */
RunAction::RunAction()
{
  currentRun_ = new G4Run();
}

void RunAction::BeginOfRunAction(const G4Run*)
{
  // Note: Place holder for BeginOfRunAction
}

void RunAction::EndOfRunAction(const G4Run* run)
{
  if (run->GetNumberOfEvent() == 0)
  {
    return;
  }

  // Note: Place holder for EndOfRunAction
}

G4Run* RunAction::GenerateRun()
{
  return currentRun_;
}

//---------------------------------------------------------------------------//
}  // namespace c4h
