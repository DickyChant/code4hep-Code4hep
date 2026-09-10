#include "Code4hep/G4Application/TrackingAction.h"

#include "Code4hep/G4Application/TrackProvenance.h"

#include "G4Track.hh"

#include <stdexcept>

namespace c4h {

void TrackingAction::PreUserTrackingAction(const G4Track *track) {
  if (track == nullptr || provenance_ == nullptr) {
    throw std::runtime_error("missing Geant4 track provenance state");
  }
  const auto mcParticleIndex = provenance_->assign(*track);
  const_cast<G4Track *>(track)->SetUserInformation(
      new TrackInformation(mcParticleIndex));
}

} // namespace c4h
