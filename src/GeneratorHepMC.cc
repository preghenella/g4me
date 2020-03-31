#include "GeneratorHepMC.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4Event.hh"

#include "HepMC3/ReaderAsciiHepMC2.h"
#include "HepMC3/GenEvent.h"
#include "HepMC3/GenVertex.h"
#include "HepMC3/GenParticle.h"
#include "HepMC3/FourVector.h"

/*****************************************************************/

GeneratorHepMC::GeneratorHepMC()
{
  mHepMCDirectory = new G4UIdirectory("/hepmc/");

  mHepMCFileNameCmd = new G4UIcmdWithAString("/hepmc/filename", this);
  mHepMCFileNameCmd->SetGuidance("HepMC filename");
  mHepMCFileNameCmd->SetParameterName("filename", false);
  mHepMCFileNameCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  
  hepmc_event  = new HepMC3::GenEvent();
}

/*****************************************************************/

void
GeneratorHepMC::SetNewValue(G4UIcommand *command, G4String value)
{
  if (command == mHepMCFileNameCmd) {
    hepmc_reader = new HepMC3::ReaderAsciiHepMC2(value);
  }
}
  
/*****************************************************************/

GeneratorHepMC::~GeneratorHepMC()
{
  delete hepmc_reader;
  delete hepmc_event;
}

/*****************************************************************/

void
GeneratorHepMC::GeneratePrimaries(G4Event *event) {
  
  hepmc_event->clear();
  hepmc_reader->read_event(*hepmc_event);
  hepmc_event->set_units(HepMC3::Units::GEV, HepMC3::Units::CM);

  /** loop over vertices **/
  for (auto const &hepmc_vertex : hepmc_event->vertices()) {
    
    auto vertex = new G4PrimaryVertex(hepmc_vertex->position().x() * cm,
				      hepmc_vertex->position().y() * cm,
				      hepmc_vertex->position().z() * cm,
				      hepmc_vertex->position().t() * cm / c_light);

    /** loop over outgoing particles **/
    for (auto const &hepmc_particle : hepmc_vertex->particles_out()) {

      if (hepmc_particle->status() != 1) continue;
      
      auto particle = new G4PrimaryParticle(hepmc_particle->pid(),
					    hepmc_particle->momentum().px() * GeV,
					    hepmc_particle->momentum().py() * GeV,
					    hepmc_particle->momentum().pz() * GeV,
					    hepmc_particle->momentum().e()  * GeV);
      
      vertex->SetPrimary(particle);
	
    } /** end of loop over outgoing particles **/

    if (vertex->GetNumberOfParticle() > 0)
      event->AddPrimaryVertex(vertex);
    else
      delete vertex;
    
  } /** end of loop over vertices **/

}
