//---------------------------------------------------------------------------//
//! \file Code4hep/Generators/src/MCParticlesToG4.cc
//---------------------------------------------------------------------------//
#include "Code4hep/Generators/MCParticlesToG4.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "G4Event.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"

namespace c4h
{
//---------------------------------------------------------------------------//
/* 
 * Convert an edm4hep::MCParticleCollection to a Geant4 event (G4Event).
 */
std::unique_ptr<G4Event>
MCParticlesToG4(const edm4hep::MCParticleCollection& mcParticles, int eventID)
{
  auto g4event = std::make_unique<G4Event>(eventID);

  // Cache vertices by (x,y,z,t)
  // Optional optimization to reuse identical vertices

  struct VertexKey
  {
    double x, y, z, t;

    bool operator==(const VertexKey& other) const
    {
      return x == other.x && y == other.y && z == other.z && t == other.t;
    }
  };

  struct VertexKeyHash
  {
    std::size_t operator()(const VertexKey& k) const
    {
      return std::hash<double>()(k.x) ^
	     std::hash<double>()(k.y) ^
	     std::hash<double>()(k.z) ^
	     std::hash<double>()(k.t);
    }
  };
  
  std::unordered_map<VertexKey, G4PrimaryVertex*, VertexKeyHash> vertexMap;

  auto* particleTable = G4ParticleTable::GetParticleTable();  

  for (const auto& p : mcParticles)
  {
    // Select only stable final-state particles
    if (!p.getGeneratorStatus())
    {
      continue;
    }
    
    // Lookup Geant4 particle definition
    int pdg = p.getPDG();
    
    auto* definition = particleTable->FindParticle(pdg);

    if (!definition)
    {
      edm::LogWarning("Code4hepGenerators")
	<< "MCParticlesToG4: Unknown PDG ID: " << pdg;
      // TODO: Refactor this loop to use an external decayer
      continue;
    }

    // Momentum: edm4hep momentum is in GeV, convert to Geant4 internal units
    auto mom = p.getMomentum();
    double px = mom.x * CLHEP::GeV;
    double py = mom.y * CLHEP::GeV;
    double pz = mom.z * CLHEP::GeV;

    // Vertex: EDM4hep vertex is in mm
    auto vtx = p.getVertex();
    double vx = vtx.x;
    double vy = vtx.y;
    double vz = vtx.z;

    // Time: EDM4hep time is usually in ns
    double vt = p.getTime();    

    // Create Geant4 primary particle    
    auto* primaryParticle = new G4PrimaryParticle(definition, px, py, pz);

    // Reuse identical vertices if possible
    VertexKey key{vx, vy, vz, vt};

    G4PrimaryVertex* primaryVertex = nullptr;
    auto it = vertexMap.find(key);

    if (it == vertexMap.end())
    {
       primaryVertex = new G4PrimaryVertex(vx, vy, vz, vt);
       vertexMap[key] = primaryVertex;
       g4event->AddPrimaryVertex(primaryVertex);
    }
    else
    {
       primaryVertex = it->second;
    }

    // Attach particle to vertex
    primaryVertex->SetPrimary(primaryParticle);
  }

  return g4event;
}  

//---------------------------------------------------------------------------//
}  // namespace c4h
