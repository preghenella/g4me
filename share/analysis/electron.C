#include "io.C"

void
electron(const char *fname)
{
  io.open(fname);
  auto nevents = io.nevents();

  auto hPrim = new TH1F("hPrim", "", 60, -3., 3.);
  auto hComp = new TH1F("hComp", "", 60, -3., 3.);
  auto hConv = new TH1F("hConv", "", 60, -3., 3.);
  auto hElse = new TH1F("hElse", "", 60, -3., 3.);
    
  for (int iev = 0; iev < nevents; ++iev) {

    io.event(iev);
    
    for (int itrk = 0; itrk < io.tracks.n; ++itrk) {

      // select electrons
      if (io.tracks.pdg[itrk] != 11) continue;

      auto e = io.tracks.e[itrk];

      // primary electrons
      if (io.tracks.parent[itrk] == -1) {
	hPrim->Fill(log10(e));
	continue;
      }

      // secondary electrons
      auto ipar = io.tracks.parent[itrk];

      // parent is not gamma
      if (io.tracks.pdg[ipar] != 22) {
	hElse->Fill(log10(e));
	continue;
      }

      // parent had conversion
      if (io.tracks.status[ipar] & io.kConversion) {
	hConv->Fill(log10(e));
	continue;
      }
      
      // parent had Compton
      if (io.tracks.status[ipar] & io.kCompton) {
	hComp->Fill(log10(e));
	continue;
      }
      
    }
  }
  
  hPrim->Sumw2();
  hConv->Sumw2();
  hComp->Sumw2();
  hElse->Sumw2();

  hPrim->Scale(1. / nevents);
  hConv->Scale(1. / nevents);
  hComp->Scale(1. / nevents);
  hElse->Scale(1. / nevents);
  
  auto c = new TCanvas("c", "c", 800, 800);
  hPrim->SetLineColor(kBlack);
  hConv->SetLineColor(kAzure-3);
  hComp->SetLineColor(kRed+1);
  hElse->SetLineColor(kGreen+2);

  hPrim->Draw();
  hConv->Draw("same");
  hComp->Draw("same");
  hElse->Draw("same");
  
  
}
