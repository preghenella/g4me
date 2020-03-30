#include "StackingAction.hh"
#include "G4Track.hh"
#include "RootIO.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"

/*****************************************************************/

StackingAction::StackingAction()
{
  mStackingDirectory = new G4UIdirectory("/stacking/");

  mStackingTransportCmd = new G4UIcmdWithAString("/stacking/transport", this);
  mStackingTransportCmd->SetGuidance("Select particles to transport");
  mStackingTransportCmd->SetParameterName("transport", false);
  mStackingTransportCmd->SetCandidates("none all gamma unstable");
  mStackingTransportCmd->AvailableForStates(G4State_PreInit, G4State_Idle);


}

/*****************************************************************/

void
StackingAction::SetNewValue(G4UIcommand *command, G4String value)
{
  if (command == mStackingTransportCmd) {
    if (value.compare("none") == 0) {
      mTransportAll = false;
      mTransportGamma = false;
      mTransportUnstable = false;
    }
    if (value.compare("all") == 0) mTransportAll = true;
    if (value.compare("gamma") == 0) mTransportGamma = true;
    if (value.compare("unstable") == 0) mTransportUnstable = true;
  }
}

/*****************************************************************/

G4ClassificationOfNewTrack
StackingAction::ClassifyNewTrack(const G4Track *aTrack)
{

  bool doTransport = false;
  
  // skip ionisation electrons
  
  // add track
  RootIO::Instance()->AddTrack(aTrack);

  // transport everything
  if (mTransportAll) {
    RootIO::Instance()->AddStatus(aTrack, RootIO::kTransport);
    return fUrgent;
  }
    
  // transport gamma
  if (mTransportGamma &&
      aTrack->GetDefinition()->GetPDGEncoding() == 22) {
    RootIO::Instance()->AddStatus(aTrack, RootIO::kTransport);
    return fUrgent;
  }
    
  // transport unstable particles (depending on lifetime)
  if (mTransportUnstable &&
      !aTrack->GetDefinition()->GetPDGStable() && 
      aTrack->GetDefinition()->GetPDGLifeTime() < mTransportUnstableLifeTime) {
    RootIO::Instance()->AddStatus(aTrack, RootIO::kTransport);
    return fUrgent;
  }
    
  // transport track
  if (doTransport) {
  }

  // kill track
  return fKill;
  
}

/******************************************************************************/

