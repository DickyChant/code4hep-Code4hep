#ifndef Code4hep_G4Application_TrackingAction_h
#define Code4hep_G4Application_TrackingAction_h

#include "G4UserTrackingAction.hh"

#include <memory>
#include <utility>

namespace c4h {

class TrackProvenance;

class TrackingAction final : public G4UserTrackingAction {
public:
  explicit TrackingAction(std::shared_ptr<TrackProvenance> provenance)
      : provenance_(std::move(provenance)) {}

  void PreUserTrackingAction(const G4Track *track) override;

private:
  std::shared_ptr<TrackProvenance> provenance_;
};

} // namespace c4h

#endif
