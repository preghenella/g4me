/// @author: Roberto Preghenella
/// @email: preghenella@bo.infn.it

#ifndef _ExternalDecayer_h_
#define _ExternalDecayer_h_

#include "G4VExtDecayer.hh"
#include "G4UImessenger.hh"

namespace G4me {

class ExternalDecayer : public G4VExtDecayer,
			public G4UImessenger
{
  
public:
  
  ExternalDecayer() = default;
  ~ExternalDecayer() = default;

  G4DecayProducts *ImportDecayProducts(const G4Track &aTrack);
  
protected:

  void SetNewValue(G4UIcommand *command, G4String value);
  
};

} /** namespace G4me **/

#endif /** _ExternalDecayer_h_ **/
