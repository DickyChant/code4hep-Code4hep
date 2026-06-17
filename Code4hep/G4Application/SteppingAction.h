//---------------------------------------------------------------------------//
//! \file Code4hep/G4Application/interface/SteppingAction.h
//---------------------------------------------------------------------------//
#ifndef Code4hep_G4Application_SteppingAction_h
#define Code4hep_G4Application_SteppingAction_h

#include "G4UserSteppingAction.hh"

class G4Step;

namespace c4h
{
class EventAction;

//---------------------------------------------------------------------------//
/*!
 * User stepping action class.
 */
class SteppingAction : public G4UserSteppingAction
{
  public:
    SteppingAction(EventAction* eventAction);
    ~SteppingAction() override = default;

    // method from the base class
    void UserSteppingAction(const G4Step*) override;

  private:
    EventAction* eventAction_;
};

//---------------------------------------------------------------------------//
}  // namespace c4h
#endif
