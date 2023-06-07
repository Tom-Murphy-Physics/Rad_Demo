/////////////////////////////////////////////////////////////
//                                                         //
//          This has become obsolete... I think            //
//                                                         //
/////////////////////////////////////////////////////////////
#include "G4Timer.hh"
 
#include "PhysicsList.hh"
#include "DetectorConstruction.hh"
#include "PhysicsListMessenger.hh"

#include "G4ProcessManager.hh"
#include "G4ProcessVector.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"
#include "G4Material.hh"
#include "G4EnergyLossTables.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"



PhysicsList::PhysicsList(DetectorConstruction* p)
:  G4VUserPhysicsList(), fMaxChargedStep(DBL_MAX)
{
  fDet = p;

  defaultCutValue = 1.000*mm;

  fCutForGamma = defaultCutValue;
  fCutForElectron = defaultCutValue;

  SetVerboseLevel(1);
  fPhysicsListMessenger = new PhysicsListMessenger(this);
}



PhysicsList::~PhysicsList()
{
  delete fPhysicsListMessenger;
}



void PhysicsList::ConstructParticle()
{
  // In this method, static member functions should be called
  // for all particles which you want to use.
  // This ensures that objects of these particle types will be
  // created in the program.

  ConstructBosons();
  ConstructLeptons();
  ConstructMesons();
  ConstructBarions();

  G4GenericIon::GenericIonDefinition();
}



void PhysicsList::ConstructBosons()
{
  // gamma

  G4Gamma::GammaDefinition();

  // charged geantino

  G4ChargedGeantino::ChargedGeantinoDefinition();

}



void PhysicsList::ConstructLeptons()
{
  // leptons

  G4Electron::ElectronDefinition();
  G4Positron::PositronDefinition();
  G4MuonPlus::MuonPlusDefinition();
  G4MuonMinus::MuonMinusDefinition();

  G4NeutrinoE::NeutrinoEDefinition();
  G4AntiNeutrinoE::AntiNeutrinoEDefinition();
  G4NeutrinoMu::NeutrinoMuDefinition();
  G4AntiNeutrinoMu::AntiNeutrinoMuDefinition();
}



void PhysicsList::ConstructMesons()
{
  //  mesons

  G4PionPlus::PionPlusDefinition();
  G4PionMinus::PionMinusDefinition();
  G4PionZero::PionZeroDefinition();
  G4KaonPlus::KaonPlusDefinition();
  G4KaonMinus::KaonMinusDefinition();
}



void PhysicsList::ConstructBarions()
{
  //  barions

  G4Proton::ProtonDefinition();
  G4AntiProton::AntiProtonDefinition();
}



void PhysicsList::ConstructProcess()
{
  AddTransportation();

  ConstructEM();
  ConstructGeneral();
}



#include "G4ComptonScattering.hh"
#include "G4GammaConversion.hh"
#include "G4PhotoElectricEffect.hh"

#include "G4eMultipleScattering.hh"
#include "G4MuMultipleScattering.hh"
#include "G4hMultipleScattering.hh"

#include "G4eIonisation.hh"
#include "G4eBremsstrahlung.hh"
#include "G4eplusAnnihilation.hh"

#include "G4MuIonisation.hh"
#include "G4MuBremsstrahlung.hh"
#include "G4MuPairProduction.hh"

#include "G4hIonisation.hh"

#include "StepCut.hh"

void PhysicsList::ConstructEM()
{/*
  auto particleIterator=GetParticleIterator();
  particleIterator->reset();

  while( (*particleIterator)() )
  {
    G4ParticleDefinition* particle = particleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();

    if (particleName == "gamma")
    {
      // Construct processes for gamma

      G4PhotoElectricEffect* fPhotoElectricEffect = new G4PhotoElectricEffect();
      G4ComptonScattering* fComptonScattering   = new G4ComptonScattering();
      G4GammaConversion* fGammaConversion     = new G4GammaConversion();

      pmanager->AddDiscreteProcess(fPhotoElectricEffect);
      pmanager->AddDiscreteProcess(fComptonScattering);
      pmanager->AddDiscreteProcess(fGammaConversion);
    }
    else if (particleName == "e-")
    {
      // Construct processes for electron

      G4eIonisation* feminusIonisation = new G4eIonisation();
      G4eBremsstrahlung* feminusBremsstrahlung = new G4eBremsstrahlung();
      StepCut* feminusStepCut = new StepCut();
      feminusStepCut->SetMaxStep(fMaxChargedStep);

      pmanager->AddProcess(feminusIonisation,-1,2,2);
      pmanager->AddProcess(feminusBremsstrahlung,-1,-1,3);
      pmanager->AddProcess(feminusStepCut,-1,-1,4);
    }
    else if (particleName == "e+")
    {
      // Construct processes for positron

      G4eIonisation* feplusIonisation = new G4eIonisation();
      G4eBremsstrahlung* feplusBremsstrahlung = new G4eBremsstrahlung();
      StepCut* feplusStepCut = new StepCut();
      feplusStepCut->SetMaxStep(fMaxChargedStep);

      pmanager->AddProcess(feplusIonisation,-1,2,2);
      pmanager->AddProcess(feplusBremsstrahlung,-1,-1,3);
      pmanager->AddProcess(feplusStepCut,-1,-1,5);
    }
    else if( particleName == "mu+" || particleName == "mu-" )
    {
      // Construct processes for muon+

      StepCut* muonStepCut = new StepCut();
      muonStepCut->SetMaxStep(fMaxChargedStep);
      G4MuIonisation* muIonisation = new G4MuIonisation();

      pmanager->AddProcess(new G4MuMultipleScattering(),-1,1,1);
      pmanager->AddProcess(muIonisation,-1,2,2);
      pmanager->AddProcess(new G4MuBremsstrahlung(),-1,-1,3);
      pmanager->AddProcess(new G4MuPairProduction(),-1,-1,4);
      pmanager->AddProcess(muonStepCut,-1,-1,3);
    }
    else if ( particleName == "proton"
              || particleName == "antiproton"
              || particleName == "pi+"
              || particleName == "pi-"
              || particleName == "kaon+"
              || particleName == "kaon-"
            )
    {
      StepCut* theHadronStepCut = new StepCut();
      theHadronStepCut->SetMaxStep(fMaxChargedStep);

      G4hIonisation* thehIonisation = new G4hIonisation();
      G4hMultipleScattering* thehMultipleScattering =
                                                  new G4hMultipleScattering();

      pmanager->AddProcess(thehMultipleScattering,-1,1,1);
      pmanager->AddProcess(thehIonisation,-1,2,2);
      pmanager->AddProcess(theHadronStepCut,-1,-1,3);
    }
  }*/
}



#include "G4Decay.hh"

void PhysicsList::ConstructGeneral()
{/*
  // Add Decay Process

  G4Decay* theDecayProcess = new G4Decay();
  auto particleIterator=GetParticleIterator();
  particleIterator->reset();

  while( (*particleIterator)() )
  {
    G4ParticleDefinition* particle = particleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();

    if (theDecayProcess->IsApplicable(*particle))
    {
      pmanager ->AddProcess(theDecayProcess);

      // set ordering for PostStepDoIt and AtRestDoIt

      pmanager ->SetProcessOrdering(theDecayProcess, idxPostStep);
      pmanager ->SetProcessOrdering(theDecayProcess, idxAtRest);
    }
  }*/
}



void PhysicsList::SetCuts()
{
  G4Timer theTimer;
  theTimer.Start();
  if (verboseLevel >0)
  {
    G4cout << "PhysicsList::SetCuts:";
    G4cout << "CutLength : " << G4BestUnit(defaultCutValue,"Length") << G4endl;
  }
  // set cut values for gamma at first and for e- second and next for e+,
  // because some processes for e+/e- need cut values for gamma
 
  SetCutValue(fCutForGamma,"gamma");

  SetCutValue(fCutForElectron,"e-");
  SetCutValue(fCutForElectron,"e+");

  if (verboseLevel>1)     DumpCutValuesTable();

  theTimer.Stop();
  G4cout.precision(6);
  G4cout << G4endl;
  G4cout << "total time(SetCuts)=" << theTimer.GetUserElapsed()
                                   << " s " <<G4endl;

}



void PhysicsList::SetGammaCut(G4double val)
{
  fCutForGamma = val;
}



void PhysicsList::SetElectronCut(G4double val)
{
  fCutForElectron = val;
}



void PhysicsList::SetMaxStep(G4double step)
{
  fMaxChargedStep = step;
  G4cout << " MaxChargedStep=" << fMaxChargedStep << G4endl;
  G4cout << G4endl;
}
