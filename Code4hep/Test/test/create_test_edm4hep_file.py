#!/usr/bin/env python3

import edm4hep
import podio


def create_frame():
    frame = podio.Frame()

    headers = edm4hep.EventHeaderCollection()
    header = headers.create()
    header.setEventNumber(42)
    header.setRunNumber(43)
    frame.put(headers, "EventHeader")

    particles = edm4hep.MCParticleCollection()
    particle = particles.create()
    particle.setPDG(42)
    frame.put(particles, "MCParticleCollection")

    tracks = edm4hep.TrackCollection()
    track = tracks.create()
    track.setType(42)
    track.addToTracks(track)
    frame.put(tracks, "TrackCollection")

    dqdxs = edm4hep.RecDqdxCollection()
    dqdx = dqdxs.create()
    dqdx.setTrack(track)
    frame.put(dqdxs, "RecDqdxCollection")

    reconstructed = edm4hep.ReconstructedParticleCollection()
    reco = reconstructed.create()
    reco.addToTracks(track)
    frame.put(reconstructed, "ReconstructedParticleCollection")

    links = edm4hep.TrackMCParticleLinkCollection()
    link = links.create()
    link.setFrom(track)
    link.setTo(particle)
    link.setWeight(1.0)
    frame.put(links, "TrackMCParticleLinkCollection")

    return frame


writer = podio.root_io.Writer("edm4hep.root")
for _ in range(3):
    writer.write_frame(create_frame(), "events")
