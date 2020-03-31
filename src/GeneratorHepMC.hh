#ifndef _GeneratorHepMC_h_
#define _GeneratorHepMC_h_

#include "G4VPrimaryGenerator.hh"
#include "G4UImessenger.hh"

class G4UIdirectory;
class G4UIcmdWithAString;

namespace HepMC3
{
  class Reader;
  class GenEvent;
}

class GeneratorHepMC : public G4VPrimaryGenerator,
		       public G4UImessenger
{
  
public:
  
  GeneratorHepMC();
  ~GeneratorHepMC() override;
  
  void GeneratePrimaries(G4Event *event) override;
  
protected:

  std::string mFileName;
  HepMC3::Reader   *hepmc_reader;
  HepMC3::GenEvent *hepmc_event;

  G4UIdirectory *mHepMCDirectory;
  G4UIcmdWithAString *mHepMCFileNameCmd;


};

#endif /** _GeneratorHepMC_h_ **/
