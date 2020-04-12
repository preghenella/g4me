#include "io.C"

void
Dmesons(const char *fname)
{
  io.open(fname);
  auto nevents = io.nevents();

  auto hMass = new TH1F("hMass", "", 3000, 0., 3.);
    
  for (int iev = 0; iev < nevents; ++iev) {

    io.event(iev);

    std::map<int, std::vector<int>> Dmesons;

    // loop over tracks
    for (int itrk = 0; itrk < io.tracks.n; ++itrk) {

      // get mother id
      auto imoth = io.tracks.parent[itrk];

      // skip primary tracks
      if (imoth == -1)	continue;
      
      // skip if track creator process is not decay
      if (io.tracks.proc[itrk] != io.fDecay) continue;
      
      // save if track mother has requested PDG code
      if (true && abs(io.tracks.pdg[imoth]) == 411) Dmesons[imoth].push_back(itrk); // D+
      if (true && abs(io.tracks.pdg[imoth]) == 421) Dmesons[imoth].push_back(itrk); // D0
      if (true && abs(io.tracks.pdg[imoth]) == 431) Dmesons[imoth].push_back(itrk); // Ds+

    }

    // loop over all D mesons
    for (auto &Dmeson : Dmesons) {
      auto imoth = Dmeson.first;

      TLorentzVector LV;
      for (auto &idau : Dmeson.second) {
	TLorentzVector lv(io.tracks.px[idau], io.tracks.py[idau], io.tracks.pz[idau], io.tracks.e[idau]);
	LV += lv;
      }

      hMass->Fill(LV.Mag());
    }
    
  }

  hMass->Draw();
  
}
