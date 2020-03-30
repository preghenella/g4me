#ifndef _EventAction_h_
#define _EventAction_h_

#include "G4UserEventAction.hh"

class EventAction : public G4UserEventAction
{

public:
  
  EventAction() = default;
  ~EventAction() = default;
  
  void BeginOfEventAction(const G4Event *aEvent) override;
  void EndOfEventAction(const G4Event *aEvent) override;
  
protected:

};

#endif /** _EventAction_h_ **/
