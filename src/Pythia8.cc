/// @author: Roberto Preghenella
/// @email: preghenella@bo.infn.it

#include "Pythia8.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithoutParameter.hh"

namespace G4me
{

::Pythia8::Pythia *Pythia8::mPythia = nullptr;
  
/*****************************************************************/

::Pythia8::Pythia *
Pythia8::Instance()
{
  if (!mPythia) {
    mPythia = new ::Pythia8::Pythia;
    new Pythia8;
  }
  return mPythia;
}

/*****************************************************************/

Pythia8::Pythia8()
{
  mPythia8Directory = new G4UIdirectory("/pythia8/");

  mConfigFileNameCmd = new G4UIcmdWithAString("/pythia8/config", this);
  mConfigFileNameCmd->SetGuidance("Config filename");
  mConfigFileNameCmd->SetParameterName("filename", false);
  mConfigFileNameCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  
  mInitCmd = new G4UIcmdWithoutParameter("/pythia8/init", this);
  mInitCmd->SetGuidance("Initialise");
  mInitCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}

/*****************************************************************/

void
Pythia8::SetNewValue(G4UIcommand *command, G4String value)
{
  if (command == mConfigFileNameCmd) {
    mPythia->readFile(value, true);
  }
  if (command == mInitCmd) {
    mPythia->init();
  }
}
  
/*****************************************************************/

} /** namespace G4me **/
