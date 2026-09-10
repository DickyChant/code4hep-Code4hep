import FWCore.ParameterSet.Config as cms
import os

process = cms.Process("TEST")

from FWCore.Modules.modules import EmptySource

process.source = EmptySource()

process.maxEvents.input = int(os.environ.get("C4H_MAX_EVENTS", "10"))

#Setup FWK for multithreaded
process.options.numberOfThreads = 4
process.options.numberOfStreams = 0

from Code4hep.G4Application.modules import G4SimProducer
from Code4hep.Generators.modules import GenProducer


process.gen = GenProducer(
    generatorType = cms.string("Pythia8Generator"),
    generator = cms.InputTag("MCParticles"),
    initialSeed = cms.uint32(12345),
        Pythia8Parameters = cms.vstring(
            "Beams:idA = 2212",
            "Beams:idB = 2212",
            "Beams:eCM = 14000.",
            "HardQCD:all = on",
            "PhaseSpace:pTHatMin = 20."
        )
)

process.sim = G4SimProducer(
    generator = cms.InputTag("gen", "MCParticles"),
    randomSeed = cms.uint32(67890),
    Physics = cms.PSet(
        type = cms.string('FTFP_BERT')
    ),
    Detector = cms.PSet(
        gdml = cms.string('Code4hep/G4Application/test/simple-cms.gdml'),
        magneticFieldTesla = cms.double(0.1),
    )
)

process.output = cms.OutputModule(
    "PodioOutputModule",
    fileName = cms.untracked.string(
        os.environ.get("C4H_OUTPUT", "pythia8-sim.edm4hep.root")
    ),
)

process.generation_step = cms.Path(process.gen)
process.simulation_step = cms.Path(process.sim)
process.output_step = cms.EndPath(process.output)

process.schedule = cms.Schedule(
    process.generation_step,
    process.simulation_step,
    process.output_step,
)
