//---------------------------------------------------------------------------//
//! \file Code4hep/Generator3/interface/MCParticlesToG4.h
//---------------------------------------------------------------------------//
#pragma once

#include <edm4hep/MCParticleCollection.h>

#include <memory>

class G4Event;

namespace c4h
{
//---------------------------------------------------------------------------//
/*!
 * A helper function to convert an EDM4hep MC particle collection into a 
 * Geant4 event.
 */
std::unique_ptr<G4Event>
MCParticlesToG4(const edm4hep::MCParticleCollection& p, int eventID = 0);

//---------------------------------------------------------------------------//
}  // namespace c4h
