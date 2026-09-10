#include "Code4hep/G4Application/TrackProvenance.h"
#include "Code4hep/Generators/MCParticlePrimaryInfo.h"

#include "G4DynamicParticle.hh"
#include "G4PrimaryParticle.hh"
#include "G4Track.hh"

namespace c4h {

int TrackProvenance::assign(const G4Track &track) {
  int mcParticleIndex = -1;
  if (track.GetParentID() == 0) {
    const auto *primary = track.GetDynamicParticle()->GetPrimaryParticle();
    if (primary != nullptr) {
      if (const auto *information = dynamic_cast<const MCParticlePrimaryInfo *>(
              primary->GetUserInformation())) {
        mcParticleIndex = information->mcParticleIndex();
      }
    }
  } else if (const auto parent = mcParticleByTrack_.find(track.GetParentID());
             parent != mcParticleByTrack_.end()) {
    mcParticleIndex = parent->second;
  }
  mcParticleByTrack_.insert_or_assign(track.GetTrackID(), mcParticleIndex);
  return mcParticleIndex;
}

} // namespace c4h
