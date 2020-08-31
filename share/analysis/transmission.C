#include "io.C"
#include "style.C"

const double hc = 197.3269788 * (2. * M_PI); // [eV nm]

bool usePDE = true;

TH1 *
transmission(const char *fname, int what = 0)
{

  /** read PDE from file **/
  TGraph gEfficiency("hamamatsu_S13360-xx50CS.txt");
  for (int i = 0; i < gEfficiency.GetN(); ++i) {
    gEfficiency.GetY()[i] *= 0.01;
  }

  style();
    
  io.open(fname);
  auto nevents = io.nevents();

  auto hGen = new TH1F("hGen", "", 1500, 0., 1500.); 
  auto hHit = new TH1F("hHit", "", 1500, 0., 1500.); 
  auto hRec = new TH1F("hRec", "", 1500, 0., 1500.); 
  
  for (int iev = 0; iev < nevents; ++iev) {

    io.event(iev);

    for (int itrk = 0; itrk < io.tracks.n; ++itrk) {
      auto eV = io.tracks.e[itrk] * 1.e9;
      auto lambda = hc / eV;
      hGen->Fill(lambda);
    }
      
    for (int ihit = 0; ihit < io.hits.n; ++ihit) {
      
      auto x = io.hits.x[ihit];
      auto y = io.hits.y[ihit];
      auto z = io.hits.z[ihit];

      auto trkid = io.hits.trkid[ihit];
      auto eV = io.tracks.e[trkid] * 1.e9;
      auto lambda = hc / eV;

      if (usePDE && gRandom->Uniform() > gEfficiency.Eval(lambda)) continue;
      hHit->Fill(lambda);

      if (y != 0. || z != 0.) continue;
      
      hRec->Fill(lambda);

    }

  }

  hHit->Sumw2();
  hRec->Sumw2();

  hHit->Divide(hHit, hGen, 1., 1., "B");
  hRec->Divide(hRec, hGen, 1., 1., "B");

  switch (what) {
  case 0: return hHit;
  case 1: return hRec;
  }

  return hHit;
}
