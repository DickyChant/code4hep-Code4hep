//---------------------------------------------------------------------------//
//! \file Code4hep/G4Application/interface/ActionInitialization.h
//---------------------------------------------------------------------------//
#pragma once

#include "G4VUserActionInitialization.hh"

namespace c4h
{
//---------------------------------------------------------------------------//
/*!
 * Action initialization class.
 */
class ActionInitialization : public G4VUserActionInitialization
{
  public:
    ActionInitialization() = default;
    ~ActionInitialization() override = default;

    void BuildForMaster() const override;
    void Build() const override;
};

//---------------------------------------------------------------------------//
}  // namespace c4h
