/// @author: Roberto Preghenella
/// @email: preghenella@bo.infn.it

#include "EventAction.hh"
#include "G4Event.hh"

#include "RootIO.hh"

namespace G4me {

/******************************************************************************/

void
EventAction::BeginOfEventAction(const G4Event *aEvent)
{
  RootIO::Instance()->BeginOfEventAction(aEvent);
}

/******************************************************************************/

void
EventAction::EndOfEventAction(const G4Event *aEvent)
{
  RootIO::Instance()->EndOfEventAction(aEvent);
}

/******************************************************************************/

} /** namespace G4me **/
