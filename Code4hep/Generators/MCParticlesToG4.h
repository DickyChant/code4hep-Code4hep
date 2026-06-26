//---------------------------------------------------------------------------//
//! \file Code4hep/Generators/MCParticlesToG4.h
//---------------------------------------------------------------------------//
#ifndef Code4hep_Generators_MCParticlesToG4_h
#define Code4hep_Generators_MCParticlesToG4_h

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
#endif
