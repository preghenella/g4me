/// @author: Roberto Preghenella
/// @email: preghenella@bo.infn.it

#ifndef _Pythia8_h_
#define _Pythia8_h_

#include "G4UImessenger.hh"
#include "Pythia8/Pythia.h"

class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithoutParameter;
class G4UIcommand;

namespace G4me
{

class Pythia8 : public G4UImessenger
{
  
public:
  
  static ::Pythia8::Pythia *Instance(); 
  
private:

  Pythia8();
  ~Pythia8() = default;

  void SetNewValue(G4UIcommand *command, G4String value);

  static ::Pythia8::Pythia *mPythia;
  
  G4UIdirectory *mPythia8Directory;
  G4UIcmdWithAString *mConfigFileNameCmd;
  G4UIcmdWithoutParameter *mInitCmd;
  G4UIcmdWithoutParameter *mStatCmd;
};

} /** namespace G4me **/
  
#endif /** _Pythia8_h_ **/
