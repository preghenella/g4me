#include "io.C"
#include "style.C"

#define MAXARRAY 65536 //1048576


const double hc = 197.3269788 * 2. * M_PI; // [eV nm]
double emission_radius = 126.; // [cm]
double sensor_radius = 127.; // [cm]
double time_resolution_sptr = 0.05; // [ns]
double time_resolution_mip = 0.02; // [ns]

bool usePDE = true;
bool usePixel = true;

void
display(const char *ifname, const char *ofname, int showevent = -1)
{
  style();

  /** open input **/
  io.open(ifname);
  auto nevents = io.nevents();

  /** open output file **/
  auto of = TFile::Open(ofname, "RECREATE");

  /** output tracks tree **/
  int otrack_n;
  float otrack_x[MAXARRAY], otrack_y[MAXARRAY], otrack_z[MAXARRAY], otrack_t[MAXARRAY];
  float otrack_px[MAXARRAY], otrack_py[MAXARRAY], otrack_pz[MAXARRAY], otrack_l[MAXARRAY];
  auto otrack_tree = new TTree("tracks", "tracks tree");
  otrack_tree->Branch("n", &otrack_n, "n/I");
  otrack_tree->Branch("x", &otrack_x, "x[n]/F");
  otrack_tree->Branch("y", &otrack_y, "y[n]/F");
  otrack_tree->Branch("z", &otrack_z, "z[n]/F");
  otrack_tree->Branch("t", &otrack_t, "t[n]/F");
  otrack_tree->Branch("px", &otrack_px, "px[n]/F");
  otrack_tree->Branch("py", &otrack_py, "py[n]/F");
  otrack_tree->Branch("pz", &otrack_pz, "pz[n]/F");
  otrack_tree->Branch("l", &otrack_l, "l[n]/F");

  /** output hits tree **/
  auto ohit_tree = new TTree("hits", "hits tree");
  int ohit_n;
  float ohit_x[MAXARRAY], ohit_y[MAXARRAY], ohit_z[MAXARRAY], ohit_t[MAXARRAY], ohit_vx[MAXARRAY], ohit_vy[MAXARRAY], ohit_vz[MAXARRAY];  
  ohit_tree->Branch("n", &ohit_n, "n/I");
  ohit_tree->Branch("x", &ohit_x, "x[n]/F");
  ohit_tree->Branch("y", &ohit_y, "y[n]/F");
  ohit_tree->Branch("z", &ohit_z, "z[n]/F");
  ohit_tree->Branch("t", &ohit_t, "t[n]/F");
  ohit_tree->Branch("vx", &ohit_vx, "vx[n]/F");
  ohit_tree->Branch("vy", &ohit_vy, "vy[n]/F");
  ohit_tree->Branch("vz", &ohit_vz, "vz[n]/F");
  
  auto hMap = new TH2F("hMap", ";r#varphi (cm);z (cm)", 3001, -450.15, 450.15, 1401, -210.15, 210.15);

  /** read PDE from file **/
  TGraph gEfficiency("hamamatsu_S13360-xx50CS.txt");
  for (int i = 0; i < gEfficiency.GetN(); ++i) {
    gEfficiency.GetY()[i] *= 0.01;
  }

  /** PDG database **/
  auto pdgdb = TDatabasePDG::Instance();

  /** loop over events **/
  for (int iev = 0; iev < nevents; ++iev) {
    io.event(iev);

    if (showevent >= 0 && iev != showevent) continue;

    /** reset **/
    hMap->Reset();
    otrack_n = 0;
    ohit_n = 0;
    
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

      /** must have reached the aerogel layer **/
      bool goodTrack = false;
      for (int ihit = 0; ihit < io.hits.n; ++ihit) {
	int trkid = io.hits.trkid[ihit];
	if (itrk != trkid) continue;
	if (io.hits.lyrid[ihit] != 9) continue;
	goodTrack = true;
	otrack_x[otrack_n] = io.hits.x[ihit];
	otrack_y[otrack_n] = io.hits.y[ihit];
	otrack_z[otrack_n] = io.hits.z[ihit];
	otrack_t[otrack_n] = io.hits.t[ihit];
	otrack_px[otrack_n] = io.hits.px[ihit];
	otrack_py[otrack_n] = io.hits.py[ihit];
	otrack_pz[otrack_n] = io.hits.pz[ihit];
	otrack_l[otrack_n] = io.hits.trklen[ihit];
	break;
      }
      if (!goodTrack) continue;

      /** popagate the track to emission radius (straight approximation) **/
      auto p = sqrt(otrack_px[otrack_n] * otrack_px[otrack_n] +
		    otrack_py[otrack_n] * otrack_py[otrack_n] +
		    otrack_pz[otrack_n] * otrack_pz[otrack_n]);
      auto deltax = 0.01 * otrack_px[otrack_n] / p;
      auto deltay = 0.01 * otrack_py[otrack_n] / p;
      auto deltaz = 0.01 * otrack_pz[otrack_n] / p;
      auto deltal = sqrt(deltax * deltax + deltay * deltay + deltaz * deltaz);
      auto deltat = otrack_t[otrack_n] / otrack_l[otrack_n] * deltal;
      while (hypot(otrack_x[otrack_n], otrack_y[otrack_n]) < emission_radius) {
	otrack_x[otrack_n] += deltax;
	otrack_y[otrack_n] += deltay;
	otrack_z[otrack_n] += deltaz;
	otrack_l[otrack_n] += deltal;
	otrack_t[otrack_n] += deltat;
      }

#if 0
      std::cout << " --- this is a " << pdg
		<< " that has reached r = " << hypot(otrack_x[otrack_n], otrack_y[otrack_n])
		<< " and has p = ("
		<< otrack_px[otrack_n] << ", "
		<< otrack_py[otrack_n] << ", "
		<< otrack_pz[otrack_n] << ")"
		<< std::endl;
#endif
      
      otrack_n++;
      
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
      
      /** apply PDE and SPTR if optical photon **/
      int trkid = io.hits.trkid[ihit];
      int pdg = io.tracks.pdg[trkid]; // optical photons have pdg = 0
      if (pdg == 0) {
	auto eV = io.hits.e[ihit] * 1.e9;
	auto lambda = hc / eV;
	double pde = usePDE ? gEfficiency.Eval(lambda) : 1.;
	if (pde < 0) continue;
	if (pde < gRandom->Uniform()) continue;
	t += gRandom->Gaus(0., time_resolution_sptr);
      } else { // else treat it as MIP	
	t += gRandom->Gaus(0., time_resolution_mip);
      }
      
      /** check sensor not hit already **/
      int irphi = hMap->GetXaxis()->FindBin(rphi);
      int iz = hMap->GetYaxis()->FindBin(z);
      if (hMap->GetBinContent(irphi, iz) != 0.) continue;

      /** set time on hit map **/
      hMap->SetBinContent(irphi, iz, t);

      /** get coordinates from sensor center **/
      rphi = hMap->GetXaxis()->GetBinCenter(irphi);
      ohit_x[ohit_n] = sensor_radius * cos(rphi / sensor_radius);
      ohit_y[ohit_n] = sensor_radius * sin(rphi / sensor_radius);
      ohit_z[ohit_n] = hMap->GetYaxis()->GetBinCenter(iz);
      ohit_t[ohit_n] = t;

      /** restore true coordinates **/
      if (!usePixel) {
	ohit_x[ohit_n] = io.hits.x[ihit];
	ohit_y[ohit_n] = io.hits.y[ihit];
	ohit_z[ohit_n] = io.hits.z[ihit];
      }

      /** to cheat on emission point **/
      ohit_vx[ohit_n] = io.tracks.vx[trkid];
      ohit_vy[ohit_n] = io.tracks.vy[trkid];
      ohit_vz[ohit_n] = io.tracks.vz[trkid];
      
      ohit_n++;
      
    }

    /** fill output trees **/
    otrack_tree->Fill();
    ohit_tree->Fill();
    
    
    if (showevent >= 0) {
      hMap->Draw("colz");
      return;
    }
    
  }

  /** write output trees **/
  of->cd();
  otrack_tree->Write();
  ohit_tree->Write();
  of->Close();
  
}
