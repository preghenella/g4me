/// @author: Roberto Preghenella
/// @email: preghenella@bo.infn.it

#include "RootIO.hh"
#include "G4SystemOfUnits.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4Run.hh"
#include "G4Event.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4ParticleDefinition.hh"
#include "G4VProcess.hh"
#include "G4ProcessType.hh"
#include "TFile.h"
#include "TTree.h"

namespace G4me {

RootIO *RootIO::mInstance = nullptr;

/*****************************************************************/

void
RootIO::InitMessenger()
{
  mDirectory = new G4UIdirectory("/io/");
  mFileNameCmd = new G4UIcmdWithAString("/io/prefix", this);
  mFileNameCmd->SetGuidance("Output file prefix.");
  mFileNameCmd->SetParameterName("prefix", false);
  mFileNameCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
};

/*****************************************************************/
  
void
RootIO::SetNewValue(G4UIcommand *command, G4String value)
{
  if (command == mFileNameCmd)
    mFilePrefix = value;
}

/*****************************************************************/
  
void
RootIO::BeginOfRunAction(const G4Run *aRun)
{
  std::string filename = Form("%s.%03d.root", mFilePrefix.c_str(), aRun->GetRunID());
  Open(filename);
}

/*****************************************************************/

void
RootIO::EndOfRunAction(const G4Run *aRun)
{
  Close();
}

/*****************************************************************/

void
RootIO::BeginOfEventAction(const G4Event *aEvent)
{
  ResetHits();
  ResetTracks();
}

/*****************************************************************/

void
RootIO::EndOfEventAction(const G4Event *aEvent)
{
  FillHits();
  FillTracks();
}

/*****************************************************************/

void
RootIO::Open(std::string filename) {
  
  mFile = TFile::Open(filename.c_str(), "RECREATE");
  
  mTreeHits = new TTree("Hits", "RootIO tree");
  mTreeHits->Branch("n"      , &mHits.n      , "n/I");
  mTreeHits->Branch("trkid"  , &mHits.trkid  , "trkid[n]/I");
  mTreeHits->Branch("trklen" , &mHits.trklen , "trklen[n]/F");
  mTreeHits->Branch("edep"   , &mHits.edep   , "edep[n]/F");
  mTreeHits->Branch("x"      , &mHits.x      , "x[n]/F");
  mTreeHits->Branch("y"      , &mHits.y      , "y[n]/F");
  mTreeHits->Branch("z"      , &mHits.z      , "z[n]/F");
  mTreeHits->Branch("t"      , &mHits.t      , "t[n]/F");
  mTreeHits->Branch("lyrid"  , &mHits.lyrid  , "lyrid[n]/I");
  
  mTreeTracks = new TTree("Tracks", "RootIO tree");
  mTreeTracks->Branch("n"      , &mTracks.n      , "n/I");
  mTreeTracks->Branch("proc"   , &mTracks.proc   , "proc[n]/B");
  mTreeTracks->Branch("sproc"  , &mTracks.sproc  , "sproc[n]/B");
  mTreeTracks->Branch("status" , &mTracks.status , "status[n]/I");
  mTreeTracks->Branch("parent" , &mTracks.parent , "parent[n]/I");
  mTreeTracks->Branch("pdg"    , &mTracks.pdg    , "pdg[n]/I");
  mTreeTracks->Branch("vt"     , &mTracks.vt     , "vt[n]/D");
  mTreeTracks->Branch("vx"     , &mTracks.vx     , "vx[n]/D");
  mTreeTracks->Branch("vy"     , &mTracks.vy     , "vy[n]/D");
  mTreeTracks->Branch("vz"     , &mTracks.vz     , "vz[n]/D");
  mTreeTracks->Branch("e"      , &mTracks.e      , "e[n]/D");
  mTreeTracks->Branch("px"     , &mTracks.px     , "px[n]/D");
  mTreeTracks->Branch("py"     , &mTracks.py     , "py[n]/D");
  mTreeTracks->Branch("pz"     , &mTracks.pz     , "pz[n]/D");
  
};

/*****************************************************************/

void
RootIO::Close()
{
  mFile->cd();
  mTreeHits->Write();
  mTreeTracks->Write();
  mFile->Close();
}

/*****************************************************************/

void
RootIO::ResetTracks()
{
  mTracks.n = 0;
}

/*****************************************************************/

void
RootIO::FillTracks()
{
  mTreeTracks->Fill();
}

/*****************************************************************/

void
RootIO::AddTrack(const G4Track *aTrack) {
  auto id = aTrack->GetTrackID() - 1;
  if (mTracks.n != id) {
    std::cout << "--- oh dear, this can lead to hard times later: " << mTracks.n << " " << aTrack->GetTrackID() << std::endl;
  }
  mTracks.proc[id]   = aTrack->GetCreatorProcess() ? aTrack->GetCreatorProcess()->GetProcessType() : -1;
  mTracks.sproc[id]  = aTrack->GetCreatorProcess() ? aTrack->GetCreatorProcess()->GetProcessSubType() : -1;
  mTracks.parent[id] = aTrack->GetParentID() - 1;
  mTracks.pdg[id]    = aTrack->GetParticleDefinition()->GetPDGEncoding();
  mTracks.vt[id]     = aTrack->GetGlobalTime() / ns;
  mTracks.vx[id]     = aTrack->GetPosition().x()  / cm;
  mTracks.vy[id]     = aTrack->GetPosition().y()  / cm;
  mTracks.vz[id]     = aTrack->GetPosition().z()  / cm;
  mTracks.e[id]      = aTrack->GetTotalEnergy()   / GeV;
  mTracks.px[id]     = aTrack->GetMomentum().x()  / GeV;
  mTracks.py[id]     = aTrack->GetMomentum().y()  / GeV;
  mTracks.pz[id]     = aTrack->GetMomentum().z()  / GeV;
  mTracks.n++;
}

/*****************************************************************/

void
RootIO::AddStatus(const G4Track *aTrack, ETrackStatus_t status) {
  auto id = aTrack->GetTrackID() - 1;
  mTracks.status[id] |= status;
}

/*****************************************************************/

void
RootIO::ResetHits()
{
  mHits.n = 0;
}

/*****************************************************************/

void
RootIO::FillHits()
{
  mTreeHits->Fill();
}

/*****************************************************************/

void
RootIO::AddHit(const G4Step *aStep)
{
  auto point = aStep->GetPreStepPoint();
  auto track = aStep->GetTrack();

  mHits.trkid[mHits.n]  = track->GetTrackID() - 1;
  mHits.trklen[mHits.n] = track->GetTrackLength()  / cm;
  mHits.x[mHits.n]      = track->GetPosition().x() / cm;
  mHits.y[mHits.n]      = track->GetPosition().y() / cm;
  mHits.z[mHits.n]      = track->GetPosition().z() / cm;
  mHits.t[mHits.n]      = track->GetGlobalTime()   / ns;
  mHits.lyrid[mHits.n]  = point->GetTouchableHandle()->GetCopyNumber();
  mHits.n++;
}

/*****************************************************************/

} /** namespace G4me **/
