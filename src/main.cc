/// @author: Roberto Preghenella
/// @email: preghenella@bo.infn.it

#include "RootIO.hh"
#include "G4RunManager.hh"
#include "G4UIExecutive.hh"
#include "G4UImanager.hh"
#include "FTFP_BERT.hh"
#include "ExternalDecayerPhysics.hh"
#include "PrimaryGeneratorAction.hh"
#include "DetectorConstruction.hh"
#include "G4VisManager.hh"
#include "G4VisExecutive.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "StackingAction.hh"
#include "SteppingAction.hh"

int
main(int argc, char **argv)
{

  std::vector<G4String> physicsList = {
    "G4EmStandardPhysics",
    "G4DecayPhysics"
  };
  
  auto run = new G4RunManager;
  auto physics = new FTFP_BERT;
  physics->RegisterPhysics(new G4me::ExternalDecayerPhysics());
  auto detector = new G4me::DetectorConstruction();
  run->SetUserInitialization(detector);
  run->SetUserInitialization(physics);
  
  //  run->Initialize();

  auto action_generator = new G4me::PrimaryGeneratorAction();
  auto action_run = new G4me::RunAction();
  auto action_event = new G4me::EventAction();
  auto action_stacking = new G4me::StackingAction();
  auto action_stepping = new G4me::SteppingAction();
  
  run->SetUserAction(action_generator);
  run->SetUserAction(action_run);
  run->SetUserAction(action_event);
  run->SetUserAction(action_stacking);
  run->SetUserAction(action_stepping);

  // initialize RootIO messenger
  G4me::RootIO::Instance()->InitMessenger();

  // start interative session
  if (argc == 1) {
    auto ui = new G4UIExecutive(argc, argv, "tcsh");
    ui->SessionStart();
    delete ui;  
    return 0;
  }

  auto uiManager = G4UImanager::GetUIpointer();
  std::string command = "/control/execute ";
  std::string fileName = argv[1];
  uiManager->ApplyCommand(command + fileName);
  
  //  G4VisManager* visManager = new G4VisExecutive;
  // G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
  // G4VisManager* visManager = new G4VisExecutive("Quiet");
  //  visManager->Initialize();

  return 0;
}
