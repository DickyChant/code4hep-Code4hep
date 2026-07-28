//---------------------------------------------------------------------------//
//! \file Code4hep/G4Application/interface/EventAction.h
//---------------------------------------------------------------------------//
#pragma once

#include "G4UserEventAction.hh"
#include "globals.hh"

class G4Event;

namespace c4h
{

class RunAction;

//---------------------------------------------------------------------------//
/*!
 * User event action class.
 */
class EventAction : public G4UserEventAction
{
  public:
    EventAction(RunAction* runAction);
    ~EventAction() override = default;

    void BeginOfEventAction(const G4Event* event) override;
    void EndOfEventAction(const G4Event* event) override;

    void AddEdep(G4double edep) { edep_ += edep; }

  private:
    RunAction* runAction_ = nullptr;
    G4double edep_ = 0.;
};

//---------------------------------------------------------------------------//
}  // namespace c4h
