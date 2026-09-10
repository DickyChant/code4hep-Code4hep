import FWCore.ParameterSet.Config as cms
import os

process = cms.Process("TEST")

from FWCore.Modules.modules import EmptySource

process.source = EmptySource()

process.maxEvents.input = int(os.environ.get("C4H_MAX_EVENTS", "10"))

#Setup FWK for multithreaded
process.options.numberOfThreads = 4
process.options.numberOfStreams = 4

from Code4hep.G4Application.modules import G4SimProducer
from Code4hep.Generators.modules import GenProducer

process.gen = GenProducer(
    generatorType = cms.string("HepMC3Generator"),
    generator = cms.InputTag("MCParticles"),
    initialSeed = cms.uint32(12345),
        Verbosity = cms.untracked.int32(0),
        PartID = cms.untracked.int32(13),
        MinPt = cms.double(10.0), ## the cut is in GeV 
        MaxPt = cms.double(10.0), 
        MinEta = cms.double(-2.5),
        MaxEta = cms.double(2.5),
        MinPhi = cms.double(-3.14159265359), ## (radians)
        MaxPhi = cms.double(3.14159265359),  ## according to CMS conventions

)

process.sim = G4SimProducer(
    generator = cms.InputTag("gen", "MCParticles"),
    randomSeed = cms.uint32(67890),
    Physics = cms.PSet(
        type = cms.string('FTFP_BERT')
    ),
    Detector = cms.PSet(
        gdml = cms.string('Code4hep/G4Application/test/simple-cms.gdml')
    )
)

process.output = cms.OutputModule(
    "PodioOutputModule",
    fileName = cms.untracked.string(
        os.environ.get("C4H_OUTPUT", "hepmc3-sim.edm4hep.root")
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
