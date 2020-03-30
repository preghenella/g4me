/// @author: Roberto Preghenella
/// @email: preghenella@bo.infn.it

#include "RunAction.hh"
#include "G4Run.hh"

#include "RootIO.hh"

/******************************************************************************/

void
RunAction::BeginOfRunAction(const G4Run *aRun)
{
  /** begin of run action **/

  std::cout << "--- start of run: " << aRun->GetRunID() << std::endl;  
  RootIO::Instance()->BeginOfRunAction(aRun);

}

/******************************************************************************/

void
RunAction::EndOfRunAction(const G4Run *aRun)
{
  /** end of run action **/

  std::cout << "--- end of run: " << aRun->GetRunID() << std::endl;
  RootIO::Instance()->Close();
}

/******************************************************************************/
