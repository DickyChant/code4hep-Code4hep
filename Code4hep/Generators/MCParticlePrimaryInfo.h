#ifndef Code4hep_Generators_MCParticlePrimaryInfo_h
#define Code4hep_Generators_MCParticlePrimaryInfo_h

#include "G4VUserPrimaryParticleInformation.hh"

namespace c4h {

class MCParticlePrimaryInfo final : public G4VUserPrimaryParticleInformation {
public:
  explicit MCParticlePrimaryInfo(int mcParticleIndex)
      : mcParticleIndex_(mcParticleIndex) {}

  int mcParticleIndex() const { return mcParticleIndex_; }
  void Print() const override {}

private:
  int mcParticleIndex_{};
};

} // namespace c4h

#endif
