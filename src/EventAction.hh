/// @author: Roberto Preghenella
/// @email: preghenella@bo.infn.it

#ifndef _EventAction_h_
#define _EventAction_h_

#include "G4UserEventAction.hh"

namespace G4me {

class EventAction : public G4UserEventAction
{

public:
  
  EventAction() = default;
  ~EventAction() = default;
  
  void BeginOfEventAction(const G4Event *aEvent) override;
  void EndOfEventAction(const G4Event *aEvent) override;
  
protected:

};

} /** namespace G4me **/
  
#endif /** _EventAction_h_ **/
