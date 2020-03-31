#ifndef _GeneratorHepMC_h_
#define _GeneratorHepMC_h_

#include "G4VPrimaryGenerator.hh"
#include "G4UImessenger.hh"

class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcommand;

namespace HepMC3
{
  class Reader;
  class GenEvent;
}

namespace G4me {

class GeneratorHepMC : public G4VPrimaryGenerator,
		       public G4UImessenger
{
  
public:
  
  GeneratorHepMC();
  ~GeneratorHepMC() override;
  
  void GeneratePrimaryVertex(G4Event *event) override;
  
protected:

  void SetNewValue(G4UIcommand *command, G4String value);

  std::string mFileName;
  HepMC3::Reader   *hepmc_reader;
  HepMC3::GenEvent *hepmc_event;

  G4UIdirectory *mHepMCDirectory;
  G4UIcmdWithAString *mHepMCFileNameCmd;
  G4UIdirectory *mHepMCCutsDirectory;
  G4UIcommand *mHepMCCutsEta;

  double fCutsEtaMin = -DBL_MAX;
  double fCutsEtaMax = DBL_MAX;

};

} /** namespace G4me **/
  
#endif /** _GeneratorHepMC_h_ **/
