#ifndef _PrimaryGeneratorAction_h_
#define _PrimaryGeneratorAction_h_

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4UImessenger.hh"


class G4event;
class G4VPrimaryGenerator;
class G4UIdirectory;
class G4UIcmdWithAString;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction,
			       public G4UImessenger
{
  
public:
  
  PrimaryGeneratorAction();
  ~PrimaryGeneratorAction() override;
  
  void GeneratePrimaries(G4Event *event) override;
  
protected:

  void SetNewValue(G4UIcommand *command, G4String value);
  
  G4VPrimaryGenerator *mParticleSource;

  G4UIdirectory *mGeneratorDirectory;
  G4UIcmdWithAString *mGeneratorSelectCmd;  
  
};

#endif /** _PrimaryGeneratorAction_h_ **/
