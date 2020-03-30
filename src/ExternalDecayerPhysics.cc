/// @author: Roberto Preghenella
/// @email: preghenella@bo.infn.it

#include "ExternalDecayerPhysics.hh"
#include "ExternalDecayer.hh"
#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"
#include "G4Decay.hh"

/*****************************************************************/

void
ExternalDecayerPhysics::SetNewValue(G4UIcommand *command, G4String value)
{}
  
/*****************************************************************/

void
ExternalDecayerPhysics::ConstructProcess()
{
  std::cout << " --- constructing external decayer physics" << std::endl;

  /** create external decayer **/
  auto externalDecayer = new ExternalDecayer();
  
  auto particleIterator = GetParticleIterator();
  particleIterator->reset();
  while ((*particleIterator)())
  {    
    auto particle = particleIterator->value();
    auto pmanager = particle->GetProcessManager();
        
    auto processVector = pmanager->GetProcessList();
    for (int i = 0; i < processVector->length(); ++i) {
      auto decay = dynamic_cast<G4Decay *>((*processVector)[i]);
      if (decay) {
	std::cout << " --- setting external decayer for: " <<  particleIterator->value()->GetParticleName() << std::endl;
	decay->SetExtDecayer(externalDecayer);
      }
    }              
  }


  std::cout << " --- external decayer physics constructed" << std::endl;
}
