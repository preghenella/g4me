/// @author: Roberto Preghenella
/// @email: preghenella@bo.infn.it

#include "GeneratorPythia8.hh"
#include "Pythia8.hh"
#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4Event.hh"

#include "TVector3.h"

/*****************************************************************/

GeneratorPythia8::GeneratorPythia8()
{
  G4me::Pythia8::Instance();
  
  mPythia8CutsDirectory = new G4UIdirectory("/pythia8/cuts/");
  
  mPythia8CutsEta = new G4UIcommand("/pythia8/cuts/eta", this);
  mPythia8CutsEta->SetGuidance("Pseudorapidity selection cuts");
  mPythia8CutsEta->SetParameter(new G4UIparameter("min", 'd', false));
  mPythia8CutsEta->SetParameter(new G4UIparameter("max", 'd', false));
  mPythia8CutsEta->AvailableForStates(G4State_PreInit, G4State_Idle);
}

/*****************************************************************/

void
GeneratorPythia8::SetNewValue(G4UIcommand *command, G4String value)
{
  if (command == mPythia8CutsEta) {
    G4String eta_min, eta_max;
    std::istringstream iss(value);
    iss >> fCutsEtaMin >> fCutsEtaMax;
  }
}
  
/*****************************************************************/

GeneratorPythia8::~GeneratorPythia8()
{
}

/*****************************************************************/

void
GeneratorPythia8::GeneratePrimaryVertex(G4Event *event)
{

  auto pythia = G4me::Pythia8::Instance();
  pythia->next();
  
  auto nParticles = pythia->event.size();
  for (int iparticle = 0; iparticle < nParticles; iparticle++) { // first particle is system
    auto aParticle = pythia->event[iparticle];
    
    if (aParticle.statusHepMC() != 1) continue;
    if (aParticle.eta() < fCutsEtaMin ||
	aParticle.eta() > fCutsEtaMax) continue;

    auto pdg = aParticle.id();
    auto px = aParticle.px() * GeV;
    auto py = aParticle.py() * GeV;
    auto pz = aParticle.pz() * GeV;
    auto et = aParticle.e() * GeV;
    auto vx = aParticle.xProd() * mm;
    auto vy = aParticle.yProd() * mm;
    auto vz = aParticle.zProd() * mm;
    auto vt = aParticle.tProd() * mm / c_light;

    auto particle = new G4PrimaryParticle(pdg, px, py, pz, et);
    auto vertex = new G4PrimaryVertex(vx, vy, vz, vt);      
    vertex->SetPrimary(particle);
    event->AddPrimaryVertex(vertex);
  }
  
}
