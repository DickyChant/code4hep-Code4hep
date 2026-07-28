import FWCore.ParameterSet.Config as cms

process = cms.Process("TEST")

from FWCore.Modules.modules import EmptySource

process.source = EmptySource()

process.maxEvents.input = 10

from Code4hep.G4Application.modules import G4SimProducer

process.sim = G4SimProducer()

process.p = cms.Path(process.sim)
