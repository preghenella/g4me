/// @author: Roberto Preghenella
/// @email: preghenella@bo.infn.it

#ifndef _RunAction_h_
#define _RunAction_h_

#include "G4UserRunAction.hh"

class TFile;
class TTree;

namespace G4me {

class RunAction : public G4UserRunAction
{

public:
  
  RunAction() = default;
  ~RunAction() = default;
  
  void BeginOfRunAction(const G4Run *run) override;
  void EndOfRunAction(const G4Run *run) override;
  
protected:

};

} /** namespace G4me **/
  
#endif /** _RunAction_h_ **/
