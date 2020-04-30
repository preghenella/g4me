/// @author: Roberto Preghenella
/// @email: preghenella@bo.infn.it

#ifndef _RootIO_h_
#define _RootIO_h_

#include "G4UImessenger.hh"

class G4UIcommand;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithABool;
class G4Event;
class G4Run;
class G4Track;
class G4Step;
class TFile;
class TTree;

namespace G4me {

class RootIO : public G4UImessenger
{
  
public:

  static RootIO *Instance() {
    if (!mInstance) mInstance = new RootIO();
    return mInstance;
  };

  void InitMessenger();
  void SetNewValue(G4UIcommand *command, G4String value);
  
  void BeginOfRunAction(const G4Run *aRun);
  void EndOfRunAction(const G4Run *aRun);
  void BeginOfEventAction(const G4Event *aEvent);
  void EndOfEventAction(const G4Event *aEvent);

  void Open(std::string filename);
  void Close();

  enum ETrackStatus_t {
    kTransport = 1 << 0,
    kElectromagnetic = 1 << 1,
    kHadronic = 1 << 2,
    kDecay = 1 << 3,
    kConversion = 1 << 4,
    kCompton = 1 << 5
  };
  
  void ResetTracks();
  void FillTracks();
  void AddTrack(const G4Track *aTrack);
  void AddStatus(const G4Track *aTrack, ETrackStatus_t status);
  
  void ResetHits();
  void FillHits();
  void AddHit(const G4Step *aStep);

  void ResetParticles();
  void FillParticles();
  void AddParticle(int id, int pdg, int parent,
		   double px, double py, double pz, double et,
		   double vx, double vy, double vz, double vt);
  
  private:

  RootIO() = default;
  
  static RootIO *mInstance;
  std::string mFilePrefix = "tracker";
  TFile *mFile = nullptr;
  TTree *mTreeHits = nullptr;
  TTree *mTreeTracks = nullptr;
  TTree *mTreeParticles = nullptr;

  G4UIdirectory *mDirectory;
  G4UIcmdWithAString *mFileNameCmd;
  G4UIcmdWithABool *mSaveParticlesCmd;

  bool mSaveParticles = true;
  
  static const int kMaxHits = 1048576;
  
  struct Hits_t {
    int    n;
    int    trkid[kMaxHits];
    float  trklen[kMaxHits];
    float  edep[kMaxHits];
    float  x[kMaxHits];
    float  y[kMaxHits];
    float  z[kMaxHits];
    float  t[kMaxHits];
    double  e[kMaxHits];
    double px[kMaxHits];
    double py[kMaxHits];
    double pz[kMaxHits];
    int    lyrid[kMaxHits];
  } mHits; //!

  static const int kMaxTracks = 1048576;

  struct Tracks_t {
    int    n;
    char   proc[kMaxTracks]; // creator process type 
    char   sproc[kMaxTracks]; // creator process subtype
    int    status[kMaxTracks];
    int    parent[kMaxTracks];
    int    particle[kMaxTracks];
    int    pdg[kMaxTracks];
    double vt[kMaxTracks];
    double vx[kMaxTracks];
    double vy[kMaxTracks];
    double vz[kMaxTracks];
    double  e[kMaxTracks];
    double px[kMaxTracks];
    double py[kMaxTracks];
    double pz[kMaxTracks];
  } mTracks; //!
  
  static const int kMaxParticles = 1048576;

  struct Particles_t {
    int    n;
    int    parent[kMaxParticles];
    int    pdg[kMaxParticles];
    double vt[kMaxParticles];
    double vx[kMaxParticles];
    double vy[kMaxParticles];
    double vz[kMaxParticles];
    double  e[kMaxParticles];
    double px[kMaxParticles];
    double py[kMaxParticles];
    double pz[kMaxParticles];
  } mParticles; //!

};

} /** namespace G4me **/
  
#endif /** _RootIO_h_ **/
