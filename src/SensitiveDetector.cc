#include "SensitiveDetector.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"
#include "G4ParticleDefinition.hh"

#include "RootIO.hh"

/*****************************************************************/

G4bool
SensitiveDetector::ProcessHits(G4Step *aStep, G4TouchableHistory *ROhist)
{

  if (aStep->GetTotalEnergyDeposit() == 0.) return true;
  if (!aStep->IsFirstStepInVolume()) return true;
  
  auto track = aStep->GetTrack();
  auto particleDefinition = track->GetParticleDefinition();
  auto momentumDirection = track->GetMomentumDirection();
  auto position = track->GetPosition();
  auto angle = momentumDirection.angle(position);
  if (angle > 0.92729522) return true; // asin(0.8)
  
  RootIO::Instance()->AddHit(aStep);
  
  return true;
}

/*****************************************************************/
