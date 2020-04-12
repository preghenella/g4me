/// @author: Roberto Preghenella
/// @email: preghenella@bo.infn.it

#ifndef _GeneratorPythia8_h_
#define _GeneratorPythia8_h_

#include "G4VPrimaryGenerator.hh"
#include "G4UImessenger.hh"

class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithoutParameter;
class G4UIcommand;

namespace G4me {

class GeneratorPythia8 : public G4VPrimaryGenerator,
			 public G4UImessenger
{
  
public:
  
  GeneratorPythia8();
  ~GeneratorPythia8() override;
  
  void GeneratePrimaryVertex(G4Event *event) override;
  
protected:

  void SetNewValue(G4UIcommand *command, G4String value);

  G4UIdirectory *mPythia8CutsDirectory;
  G4UIcommand *mPythia8CutsEta;

  double fCutsEtaMin = -DBL_MAX;
  double fCutsEtaMax = DBL_MAX;
  
};

} /** namespace G4me **/
  
#endif /** _GeneratorPythia8_h_ **/
