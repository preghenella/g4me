#ifndef _StackingAction_h_
#define _StackingAction_h_

#include "G4UserStackingAction.hh"
#include "G4UImessenger.hh"

class G4UIdirectory;
class G4UIcmdWithAString;

class StackingAction : public G4UserStackingAction,
		       public G4UImessenger
{

public:
  
  StackingAction();
  ~StackingAction() = default;
  
  G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track *aTrack) override;
  
protected:

  bool mTransportAll = false;
  bool mTransportGamma = false;
  bool mTransportUnstable = false;
  
  double mTransportUnstableLifeTime = 1.; // ns

  void SetNewValue(G4UIcommand *command, G4String value);
  
  G4UIdirectory *mStackingDirectory;
  G4UIcmdWithAString *mStackingTransportCmd;
};

#endif /** _StackingAction_h_ **/
