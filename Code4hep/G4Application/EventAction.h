//---------------------------------------------------------------------------//
//! \file Code4hep/G4Application/EventAction.h
//---------------------------------------------------------------------------//
#ifndef Code4hep_G4Application_EventAction_h
#define Code4hep_G4Application_EventAction_h

#include "G4UserEventAction.hh"
#include "globals.hh"

#include <memory>

class G4Event;

namespace c4h {

class RunAction;
class TrackProvenance;

//---------------------------------------------------------------------------//
/*!
 * User event action class.
 */
class EventAction : public G4UserEventAction {
public:
  EventAction(RunAction *runAction,
              std::shared_ptr<TrackProvenance> provenance);
  ~EventAction() override = default;

  void BeginOfEventAction(const G4Event *event) override;
  void EndOfEventAction(const G4Event *event) override;

  void AddEdep(G4double edep) { edep_ += edep; }

private:
  RunAction *runAction_ = nullptr;
  std::shared_ptr<TrackProvenance> provenance_;
  G4double edep_ = 0.;
};

//---------------------------------------------------------------------------//
} // namespace c4h
#endif
