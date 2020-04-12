/// @author: Roberto Preghenella
/// @email: preghenella@bo.infn.it

#include "SteppingAction.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"
#include "G4ProcessType.hh"
#include "G4EmProcessSubType.hh"
#include "G4TrackStatus.hh"
#include "RootIO.hh"

namespace G4me {

/******************************************************************************/

void
SteppingAction::UserSteppingAction(const G4Step *aStep)
{

  auto track = aStep->GetTrack();
  auto process = aStep->GetPostStepPoint()->GetProcessDefinedStep();
  auto type = process->GetProcessType();

  if (type == fTransportation)
    return;
  else if (type == fElectromagnetic) {
    RootIO::Instance()->AddStatus(track, RootIO::kElectromagnetic);
    auto subtype = process->GetProcessSubType();
    if (subtype == fComptonScattering) RootIO::Instance()->AddStatus(track, RootIO::kCompton);
    if (subtype == fGammaConversion) RootIO::Instance()->AddStatus(track, RootIO::kConversion);
  } else if (type == fHadronic) {
    RootIO::Instance()->AddStatus(track, RootIO::kHadronic);
  } else if (type == fDecay) {
    RootIO::Instance()->AddStatus(track, RootIO::kDecay);
  }
  
}

/******************************************************************************/

} /** namespace G4me **/
