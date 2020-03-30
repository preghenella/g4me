/// @author: Roberto Preghenella
/// @email: preghenella@bo.infn.it

#include "ExternalDecayer.hh"
#include "Pythia8.hh"
#include "G4Track.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4ParticleTable.hh"

/*****************************************************************/

void
ExternalDecayer::SetNewValue(G4UIcommand *command, G4String value)
{}
  
/*****************************************************************/

G4DecayProducts *
ExternalDecayer::ImportDecayProducts(const G4Track &aTrack)
{
  auto pdg = aTrack.GetDefinition()->GetPDGEncoding();
  auto px = aTrack.GetMomentum().x() / GeV;
  auto py = aTrack.GetMomentum().y() / GeV;
  auto pz = aTrack.GetMomentum().z() / GeV;
  auto e = aTrack.GetTotalEnergy() / GeV;
  auto m = aTrack.GetDefinition()->GetPDGMass() / GeV;  

  /** decay track in Pythia8 **/
  auto pythia = G4me::Pythia8::Instance();
  auto mayDecay = pythia->particleData.mayDecay(pdg);
  pythia->particleData.mayDecay(pdg, true);
  pythia->event.reset();
  pythia->event.append(pdg, 11, 0, 0, px, py, pz, e, m);
  pythia->moreDecays();
  //  pythia->event.list();
  pythia->particleData.mayDecay(pdg, mayDecay);

  /** prepare decay products **/
  auto decayProducts = new G4DecayProducts();
  decayProducts->SetParentParticle(*aTrack.GetDynamicParticle());
  auto nParticles = pythia->event.size();
  for (int iparticle = 0; iparticle < nParticles; iparticle++) {
    auto aParticle = pythia->event[iparticle];
    if (aParticle.statusHepMC() != 1) continue;

    auto pdg = aParticle.id();
    auto px = aParticle.px() * GeV;
    auto py = aParticle.py() * GeV;
    auto pz = aParticle.pz() * GeV;
    auto et = aParticle.e() * GeV;
    auto vx = aParticle.xProd() * mm;
    auto vy = aParticle.yProd() * mm;
    auto vz = aParticle.zProd() * mm;
    auto vt = aParticle.tProd() * mm / c_light;

    // [WARNING]: position and time is not set
    auto dynamicParticle = new G4DynamicParticle(G4ParticleTable::GetParticleTable()->FindParticle(pdg),
						 G4ThreeVector(px, py, pz));

    decayProducts->PushProducts(dynamicParticle);
  }
  

  
  return decayProducts;
}

/*****************************************************************/
