#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include "TRandom.h"
#include "TDatabasePDG.h"
#include "TH2.h"

#include "io.C"

#define MAXARRAY 65536 //1048576


const double hc = 197.3269788 * 2. * M_PI; // [eV nm]
double emission_radius = 101.; // [cm]
double sensor_radius = 122.; // [cm]
double time_resolution_sptr = 0.05; // [ns]
double time_resolution_mip = 0.02; // [ns]

double noise_window = 25.e-9; // [s]
double noise_rate = 500.e3; // [Hz/channel]

bool usePDE = true;
bool usePixel = true;
bool useNoise = true;

double cutOnLambda = 0.;

void
tofrich_process(const char *fname, int maxev = kMaxInt)
{

  /** open input **/
  io.open(fname);
  auto nevents = io.nevents();

  /** open output file **/
  auto fout = TFile::Open(std::string("tofrich_process." + std::string(fname)).c_str(), "RECREATE");

  /** output tracks tree **/
  int track_n, track_pdg[MAXARRAY], track_label[MAXARRAY];
  float track_x[MAXARRAY], track_y[MAXARRAY], track_z[MAXARRAY], track_t[MAXARRAY];
  float track_px[MAXARRAY], track_py[MAXARRAY], track_pz[MAXARRAY], track_l[MAXARRAY];
  auto track_tree = new TTree("tracks", "tracks tree");
  track_tree->Branch("n", &track_n, "n/I");
  track_tree->Branch("label", &track_label, "label[n]/I");
  track_tree->Branch("pdg", &track_pdg, "pdg[n]/I");
  track_tree->Branch("x", &track_x, "x[n]/F");
  track_tree->Branch("y", &track_y, "y[n]/F");
  track_tree->Branch("z", &track_z, "z[n]/F");
  track_tree->Branch("t", &track_t, "t[n]/F");
  track_tree->Branch("px", &track_px, "px[n]/F");
  track_tree->Branch("py", &track_py, "py[n]/F");
  track_tree->Branch("pz", &track_pz, "pz[n]/F");
  track_tree->Branch("l", &track_l, "l[n]/F");

  /** output hits tree **/
  auto hit_tree = new TTree("hits", "hits tree");
  int hit_n, hit_label[MAXARRAY];
  float hit_x[MAXARRAY], hit_y[MAXARRAY], hit_z[MAXARRAY], hit_t[MAXARRAY], hit_vx[MAXARRAY], hit_vy[MAXARRAY], hit_vz[MAXARRAY];  
  hit_tree->Branch("n", &hit_n, "n/I");
  hit_tree->Branch("label", &hit_label, "label[n]/I");
  hit_tree->Branch("x", &hit_x, "x[n]/F");
  hit_tree->Branch("y", &hit_y, "y[n]/F");
  hit_tree->Branch("z", &hit_z, "z[n]/F");
  hit_tree->Branch("t", &hit_t, "t[n]/F");
  hit_tree->Branch("vx", &hit_vx, "vx[n]/F");
  hit_tree->Branch("vy", &hit_vy, "vy[n]/F");
  hit_tree->Branch("vz", &hit_vz, "vz[n]/F");
  
  /** read PDE from file **/
  TGraph gEfficiency("hamamatsu_S13360-xx50CS.txt");
  for (int i = 0; i < gEfficiency.GetN(); ++i) {
    gEfficiency.GetY()[i] *= 0.01;
  }

  /** pixellisation map **/
  auto hMap = new TH2I("hMap", ";r#varphi (cm);z (cm)", 2440, -383.27430, 383.27430, 1280, -201.06193, 201.06193);

  /** PDG database **/
  auto pdgdb = TDatabasePDG::Instance();

  /** loop over events **/
  for (int iev = 0; iev < nevents && iev < maxev; ++iev) {
    io.event(iev);
    if (iev % 1000 == 0) std::cout << " --- processing event: " << iev << std::endl; 
    
    /** reset **/
    hMap->Reset();
    track_n = 0;
    hit_n = 0;
    
    /** loop over tracks **/
    for (int itrk = 0; itrk < io.tracks.n; ++itrk) {

      /** only primary particles **/
      if (io.tracks.parent[itrk] != -1) continue;

      /** with a valid PDG code **/
      int pdg = io.tracks.pdg[itrk];
      auto pdgp = pdgdb->GetParticle(pdg);
      if (!pdgp) continue;

      /** charged particles **/
      if (pdgp->Charge() == 0.) continue;

      /** must have a hit in the last tracking layer **/
      bool goodTrack = false;
      float x, y, z, t, px, py, pz, l;
      for (int ihit = 0; ihit < io.hits.n; ++ihit) {
	int trkid = io.hits.trkid[ihit];
	if (itrk != trkid) continue;
	if (io.hits.lyrid[ihit] != 9) continue;
	goodTrack = true;
	x = io.hits.x[ihit];
	y = io.hits.y[ihit];
	z = io.hits.z[ihit];
	t = io.hits.t[ihit];
	px = io.hits.px[ihit];
	py = io.hits.py[ihit];
	pz = io.hits.pz[ihit];
	l = io.hits.trklen[ihit];
	break;
      }
      if (!goodTrack) continue;

      /** popagate to emission radius (straight approximation) **/
      auto p = sqrt(px * px + py * py + pz * pz);
      auto dl = 0.01;
      auto dx = dl * px / p;
      auto dy = dl * py / p;
      auto dz = dl * pz / p;
      auto dt = dl / l * t;
      while (hypot(x, y) < emission_radius) {
	x += dx; 
	y += dy;
	z += dz;
	l += dl;
	t += dt;
      }

      /** save track **/
      auto n = track_n;
      track_label[n] = itrk;
      track_pdg[n] = pdg;
      track_x[n] = x;
      track_y[n] = y;
      track_z[n] = z;
      track_t[n] = t;
      track_px[n] = px;
      track_py[n] = py;
      track_pz[n] = pz;
      track_l[n] = l;

      /** increment tracks **/
      track_n++;
      
    }

    /** loop over hits **/
    for (int ihit = 0; ihit < io.hits.n; ++ihit) {

      /** only hits on optical sensor **/
      double x = io.hits.x[ihit];
      double y = io.hits.y[ihit];
      double z = io.hits.z[ihit];
      double t = io.hits.t[ihit];
      double r = hypot(x, y);
      double phi = atan2(y, x);
      double rphi = r * phi;
      if (r < sensor_radius - 1.) continue;
      
      /** get coordinates of sensor centre **/
      int irphi = hMap->GetXaxis()->FindBin(rphi);
      int iz = hMap->GetYaxis()->FindBin(z);
      double sensor_rphi = hMap->GetXaxis()->GetBinCenter(irphi);
      double sensor_x = sensor_radius * cos(sensor_rphi / sensor_radius);
      double sensor_y = sensor_radius * sin(sensor_rphi / sensor_radius);
      double sensor_z = hMap->GetYaxis()->GetBinCenter(iz);

      /** check distance wrt. sensor centre **/
      if (fabs(rphi - sensor_rphi) > 0.15) continue;
      if (fabs(z - sensor_z) > 0.15) continue;

      /** apply PDE and SPTR if optical photon **/
      int trkid = io.hits.trkid[ihit];
      int pdg = io.tracks.pdg[trkid]; // optical photons have pdg = 0
      if (pdg == 0) {
	auto eV = io.hits.e[ihit] * 1.e9;
	auto lambda = hc / eV;
	if (lambda < cutOnLambda) continue;
	double pde = usePDE ? gEfficiency.Eval(lambda) : 1.;
	if (pde < 0) continue;
	if (pde < gRandom->Uniform()) continue;
	t += gRandom->Gaus(0., time_resolution_sptr);
      } else { // else treat it as MIP
	t += gRandom->Gaus(0., time_resolution_mip);
      }

      /** check sensor not hit already **/
      if (hMap->GetBinContent(irphi, iz) != 0.) continue;

      /** set hit number on hit map **/
      hMap->SetBinContent(irphi, iz, hit_n + 1);

      /** save hit **/
      auto n = hit_n;
      
      /** get coordinates from sensor center **/
      if (usePixel) {
	hit_x[n] = sensor_x;
	hit_y[n] = sensor_y;
	hit_z[n] = sensor_z;
	hit_t[n] = t;
      }
      /** use true coordinates **/
      else { 
	hit_x[hit_n] = io.hits.x[ihit];
	hit_y[hit_n] = io.hits.y[ihit];
	hit_z[hit_n] = io.hits.z[ihit];
	hit_t[n] = t;
      }

      /** to cheat on emission point **/
      hit_vx[n] = io.tracks.vx[trkid];
      hit_vy[n] = io.tracks.vy[trkid];
      hit_vz[n] = io.tracks.vz[trkid];

      /** to cheat on matching **/
      if (pdg == 0) // optical photons have pdg = 0
	hit_label[n] = io.tracks.parent[trkid]; // we must get the parent
      else
	hit_label[n] = -trkid; // use negative label for MIP
	
      hit_n++;
      
    }

    /** generate noise **/
    auto nChannels = 2440 * 1280;
    auto nNoise = useNoise ? gRandom->Poisson(noise_window * noise_rate * nChannels) : 0;
    for (int inoise = 0; inoise < nNoise; ++inoise) {
      auto irphi = gRandom->Integer(2440);
      auto iz = gRandom->Integer(1280);
      auto t = gRandom->Uniform(0., noise_window * 1.e9);
      
      /** check sensor not hit already, replace hit in case smaller time **/
      int ihit = hMap->GetBinContent(irphi, iz);
      if (ihit != 0. && t < hit_t[ihit]) {
	hit_t[ihit] = t;
	hit_label[ihit] = -999999999; // to flag noise
	hit_vx[ihit] = -999999999.;
	hit_vy[ihit] = -999999999.;
	hit_vz[ihit] = -999999999.;
	continue;
      }
      
      /** set hit number on hit map **/
      hMap->SetBinContent(irphi, iz, hit_n + 1);

      /** save hit **/
      auto n = hit_n;
      
      /** get coordinates of sensor centre **/
      double sensor_rphi = hMap->GetXaxis()->GetBinCenter(irphi);
      double sensor_x = sensor_radius * cos(sensor_rphi / sensor_radius);
      double sensor_y = sensor_radius * sin(sensor_rphi / sensor_radius);
      double sensor_z = hMap->GetYaxis()->GetBinCenter(iz);

      hit_x[n] = sensor_x;
      hit_y[n] = sensor_y;
      hit_z[n] = sensor_z;
      hit_t[n] = t;

      /** to cheat on emission point **/
      hit_vx[n] = -999999999.;
      hit_vy[n] = -999999999.;
      hit_vz[n] = -999999999.;

      /** to cheat on matching **/
      hit_label[n] = -999999999;
	
      hit_n++;
    }
    
    /** fill output trees **/
    track_tree->Fill();
    hit_tree->Fill();
    
  }

  /** write output trees **/
  fout->cd();
  track_tree->Write();
  hit_tree->Write();
  fout->Close();
  
}
