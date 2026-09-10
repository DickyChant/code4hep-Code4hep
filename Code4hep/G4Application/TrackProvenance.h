#ifndef Code4hep_G4Application_TrackProvenance_h
#define Code4hep_G4Application_TrackProvenance_h

#include "G4VUserTrackInformation.hh"

#include <unordered_map>

class G4Track;

namespace c4h {

class TrackInformation final : public G4VUserTrackInformation {
public:
  explicit TrackInformation(int mcParticleIndex)
      : mcParticleIndex_(mcParticleIndex) {}

  int mcParticleIndex() const { return mcParticleIndex_; }
  void Print() const override {}

private:
  int mcParticleIndex_{};
};

// Per-worker, per-event mapping from every Geant4 descendant to the stable
// generator particle which seeded its primary ancestry.
class TrackProvenance {
public:
  void clear() { mcParticleByTrack_.clear(); }
  int assign(const G4Track &track);

private:
  std::unordered_map<int, int> mcParticleByTrack_;
};

} // namespace c4h

#endif
