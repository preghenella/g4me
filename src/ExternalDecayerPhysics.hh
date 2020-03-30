/// @author: Roberto Preghenella
/// @email: preghenella@bo.infn.it

#ifndef _ExternalDecayerPhysics_h_
#define _ExternalDecayerPhysics_h_

#include "G4VPhysicsConstructor.hh"
#include "G4UImessenger.hh"

class ExternalDecayerPhysics : public G4VPhysicsConstructor,
			       public G4UImessenger
{
  
public:
  
  ExternalDecayerPhysics() = default;
  ~ExternalDecayerPhysics() = default;
  
protected:

  virtual void ConstructParticle() override {};
  virtual void ConstructProcess() override;

  void SetNewValue(G4UIcommand *command, G4String value);
  
};

#endif /** _ExternalDecayerPhysics_h_ **/
