struct IO_t {
  
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
    int    lyrid[kMaxHits];
  } hits;
  
  static const int kMaxTracks = 1048576;

    enum ETrackStatus_t {
    kTransport = 1 << 0,
    kElectromagnetic = 1 << 1,
    kHadronic = 1 << 2,
    kDecay = 1 << 3,
    kConversion = 1 << 4,
    kCompton = 1 << 5
  };
  
  struct Tracks_t {
    int    n;
    int    id[kMaxTracks];
    int    status[kMaxTracks];
    int    parent[kMaxTracks];
    int    pdg[kMaxTracks];
    float  vt[kMaxTracks];
    float  vx[kMaxTracks];
    float  vy[kMaxTracks];
    float  vz[kMaxTracks];
    float  e[kMaxTracks];
    float  px[kMaxTracks];
    float  py[kMaxTracks];
    float  pz[kMaxTracks];
  } tracks;
  
  TTree *tree_hits, *tree_tracks;
    
  bool
  open(std::string filename) {
    auto fin = TFile::Open(filename.c_str());  
    std::cout << " io.open: reading data from " << filename << std::endl;
    
    tree_hits = (TTree *)fin->Get("Hits");
    tree_hits->SetBranchAddress("n"      , &hits.n);
    tree_hits->SetBranchAddress("trkid"  , &hits.trkid);
    tree_hits->SetBranchAddress("trklen" , &hits.trklen);
    tree_hits->SetBranchAddress("edep"   , &hits.edep);
    tree_hits->SetBranchAddress("x"      , &hits.x);
    tree_hits->SetBranchAddress("y"      , &hits.y);
    tree_hits->SetBranchAddress("z"      , &hits.z);
    tree_hits->SetBranchAddress("t"      , &hits.t);
    tree_hits->SetBranchAddress("lyrid"  , &hits.lyrid);
    auto tree_hits_nevents = tree_hits->GetEntries();
    
    tree_tracks = (TTree *)fin->Get("Tracks");
    tree_tracks->SetBranchAddress("n"      , &tracks.n);
    tree_tracks->SetBranchAddress("id"     , &tracks.id);
    tree_tracks->SetBranchAddress("status" , &tracks.status);
    tree_tracks->SetBranchAddress("parent" , &tracks.parent);
    tree_tracks->SetBranchAddress("pdg"    , &tracks.pdg);
    tree_tracks->SetBranchAddress("vt"     , &tracks.vt);
    tree_tracks->SetBranchAddress("vx"     , &tracks.vx);
    tree_tracks->SetBranchAddress("vy"     , &tracks.vy);
    tree_tracks->SetBranchAddress("vz"     , &tracks.vz);
    tree_tracks->SetBranchAddress("e"      , &tracks.e);
    tree_tracks->SetBranchAddress("px"     , &tracks.px);
    tree_tracks->SetBranchAddress("py"     , &tracks.py);
    tree_tracks->SetBranchAddress("pz"     , &tracks.pz);
    auto tree_tracks_nevents = tree_tracks->GetEntries();
    
    if (tree_hits_nevents != tree_tracks_nevents) {
      std::cout << " io.open: entries mismatch in trees " << std::endl
		<< "          " << tree_hits_nevents   << " events in \'Hits\' tree "   << std::endl
		<< "          " << tree_tracks_nevents << " events in \'tracks\' tree " << std::endl;
	return true;
    }
    std::cout << " io.open: successfully retrieved " << tree_tracks_nevents << " events " << std::endl;
    return false;
  }

  auto nevents() { return tree_tracks->GetEntries(); }
  void event(int iev) { tree_tracks->GetEntry(iev); tree_hits->GetEntry(iev); }
  
} io;
