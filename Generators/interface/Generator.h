//---------------------------------------------------------------------------//
//! \file Code4hep/Generator3/interface/Generator.h
//---------------------------------------------------------------------------//
#pragma once

#include <edm4hep/MCParticleCollection.h>
#include "CLHEP/Random/RandFlat.h"

#include <memory>

namespace c4h
{
//---------------------------------------------------------------------------//
/*!
 * Abstract base class for Monte Carlo event generators.
 *
 * Concrete implementations must generate and return an event of
 * edm4hep::MCParticleCollection.
 */
class Generator
{
public:
  //!@{
  //! \name Type aliases
  using UPMCParticle = std::unique_ptr<edm4hep::MCParticleCollection>;
  //!@}  

public:
  Generator() = default;
  virtual ~Generator() = default;

  Generator(const Generator&) = default;
  Generator& operator=(const Generator&) = default;
  Generator(Generator&&) = default;
  Generator& operator=(Generator&&) = default;

  // Generate an event and return an edm4hep MC particle collection.  
  virtual UPMCParticle operator()(CLHEP::HepRandomEngine* rng) = 0;
};

//---------------------------------------------------------------------------//
}  // namespace c4h
