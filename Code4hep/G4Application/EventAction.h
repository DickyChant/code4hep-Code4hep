//---------------------------------------------------------------------------//
//! \file Code4hep/G4Application/EventAction.h
//---------------------------------------------------------------------------//
#ifndef Code4hep_G4Application_EventAction_h
#define Code4hep_G4Application_EventAction_h

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
#endif
