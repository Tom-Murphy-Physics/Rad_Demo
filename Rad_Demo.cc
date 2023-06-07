#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "SteppingVerbose.hh"
#include "G4RunManager.hh"
#endif

#include "PhysicsList.hh"
#include "DetectorConstruction.hh"

#include "ActionInitialization.hh"

#include "SteppingAction.hh"

#include "G4UImanager.hh"

#include "Randomize.hh"

#include "G4VModularPhysicsList.hh"
#include "FTFP_BERT.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4OpticalPhysics.hh"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

int main(int argc,char** argv)
{
  // Choose the Random engine
  //
  G4Random::setTheEngine(new CLHEP::RanecuEngine);

  // Construct the default run manager
  //
#ifdef G4MULTITHREADED
  G4RunManager * runManager = new G4RunManager;
#else
  G4VSteppingVerbose::SetInstance(new SteppingVerbose);
  G4RunManager * runManager = new G4RunManager;
#endif

  // Set mandatory initialization classes
  //
  // Detector construction
  DetectorConstruction* detector = new DetectorConstruction();
  runManager->SetUserInitialization(detector);
  // Physics list

  G4VModularPhysicsList* physicsList = new FTFP_BERT;
  physicsList->ReplacePhysics(new G4EmStandardPhysics_option4());
  
  G4OpticalPhysics* opticalPhysics = new G4OpticalPhysics();
  //auto opticalParams = G4OpticalParameters::Instance();
  opticalPhysics ->SetWLSTimeProfile("delta");

  opticalPhysics->SetScintillationYieldFactor(1.0);
  opticalPhysics->SetScintillationExcitationRatio(0.126);

  opticalPhysics->SetMaxNumPhotonsPerStep(5);
  
  opticalPhysics->SetTrackSecondariesFirst(kScintillation,true);

  physicsList->RegisterPhysics(opticalPhysics);
  runManager->SetUserInitialization(physicsList);

  //runManager->SetUserInitialization(new PhysicsList(detector));

  // User action initialization
  runManager->SetUserInitialization(new ActionInitialization(detector));

  G4UserSteppingAction* stepping_action = new SteppingAction;
  runManager->SetUserAction(stepping_action);
  
  // Initialize G4 kernel
  //
  runManager->Initialize();

#ifdef G4VIS_USE
  // Initialize visualization
  //
  G4VisManager* visManager = new G4VisExecutive;
  // G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
  // G4VisManager* visManager = new G4VisExecutive("Quiet");
  visManager->Initialize();
#endif

  // Get the pointer to the User Interface manager
  //
  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  if (argc!=1)   // batch mode
    {
      G4String command = "/control/execute ";
      G4String fileName = argv[1];
      UImanager->ApplyCommand(command+fileName);
    }
  else
    {  // interactive mode : define UI session
#ifdef G4UI_USE
     G4UIExecutive* ui = new G4UIExecutive(argc, argv);
     if (ui->IsGUI())
       UImanager->ApplyCommand("/control/execute gui.mac");
     ui->SessionStart();
     delete ui;
#endif
    }

  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  //                 owned and deleted by the run manager, so they should not
  //                 be deleted in the main() program !

#ifdef G4VIS_USE
  delete visManager;
#endif
  delete runManager;

  return 0;
}
