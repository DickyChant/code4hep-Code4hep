//---------------------------------------------------------------------------//
//! \file Code4hep/G4Application/interface/G4MasterInterface.h
//---------------------------------------------------------------------------//
#pragma once

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>

class G4MTRunManager;

namespace c4h
{
//---------------------------------------------------------------------------//
/*!
 * Serves as the main entry point for the Geant4 multithreaded application
 * and coordinates with G4SimProducer to manage Geant4 mandatory user actions.
 *
 * \note This class follows the same workflow as OscarMTMasterThread in CMSSW.
 */
class G4MasterInterface  {
public:
  explicit G4MasterInterface(edm::ParameterSet const& p);
  ~G4MasterInterface();

  void beginRun() const;
  void endRun() const;
  void stopThread();

  inline G4MTRunManager& runManagerMaster() const 
  { 
    return *runManagerMaster_; 
  }

  inline G4MTRunManager* runManagerMasterPtr() const 
  { 
    return runManagerMaster_.get();
  }

private:

  enum class ThreadState 
  { 
    NotExist = 0, BeginRun = 1, EndRun = 2, Destruct = 3 
  };

  std::shared_ptr<G4MTRunManager> runManagerMaster_;
  std::thread masterThread_;

  // status flags
  mutable std::mutex protectMutex_;
  mutable std::mutex threadMutex_;
  mutable std::condition_variable notifyMasterCv_;
  mutable std::condition_variable notifyMainCv_;
  mutable ThreadState masterThreadState_{ThreadState::NotExist};

  mutable bool masterCanProceed_ = false;
  mutable bool mainCanProceed_ = false;
  mutable bool firstRun_ = true;
  mutable bool stopped_ = false;

  edm::ParameterSet detectorParam_{""};
  edm::ParameterSet physicsParam_{""};
};

//---------------------------------------------------------------------------//
}  // namespace c4h
