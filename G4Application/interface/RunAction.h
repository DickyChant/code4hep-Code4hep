//---------------------------------------------------------------------------//
//! \file Code4hep/G4Application/interface/RunAction.h
//---------------------------------------------------------------------------//
#pragma once

#include "G4UserRunAction.hh"
#include "globals.hh"

class G4Run;

namespace c4h
{
//---------------------------------------------------------------------------//
/*!
 *  Run action class
 */
class RunAction : public G4UserRunAction
{
  public:
    RunAction();
    ~RunAction() override = default;

    void BeginOfRunAction(const G4Run*) override;
    void EndOfRunAction(const G4Run*) override;

    G4Run* GenerateRun();
    inline void AddEdep(G4double edep) { edep_ += edep; } 

  private:
    G4double edep_ = 0.;
    G4Run* currentRun_;
};

//---------------------------------------------------------------------------//
}  // namespace c4h
