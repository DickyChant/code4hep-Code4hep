#include "FWCore/TestProcessor/interface/TestProcessor.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "catch2/catch_all.hpp"

static constexpr auto s_tag = "[G4SimProducer]";

TEST_CASE("Standard checks of G4SimProducer", s_tag)
{
  const std::string baseConfig{
      R"_(from FWCore.TestProcessor.TestProcess import *
process = TestProcess()
process.toTest = cms.EDProducer(
    "G4SimProducer",
    generator = cms.InputTag("generator", "MCParticles"),
    randomSeed = cms.uint32(67890),
    Physics = cms.PSet(type = cms.string("FTFP_BERT")),
    Detector = cms.PSet(
        gdml = cms.string("unused-for-construction.gdml"),
        magneticFieldTesla = cms.double(1.2312434),
    ),
)
process.moduleToTest(process.toTest)
)_"};

  edm::test::TestProcessor::Config config{baseConfig};
  REQUIRE_NOTHROW(edm::test::TestProcessor(config));
}

// Add additional TEST_CASEs to exercise the modules capabilities
