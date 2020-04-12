/// @author: Roberto Preghenella
/// @email: preghenella@bo.infn.it

#ifndef _SteppingAction_h_
#define _StepppingAction_h_

#include "G4UserSteppingAction.hh"

namespace G4me {

class SteppingAction : public G4UserSteppingAction
{

public:
  
  SteppingAction() = default;
  ~SteppingAction() = default;
  
  void UserSteppingAction(const G4Step *astep) override;
  
protected:

};

} /** namespace G4me **/

#endif /** _SteppingAction_h_ **/
