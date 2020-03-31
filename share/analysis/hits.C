#include "io.C"

void
hits(const char *fname)
{
  io.open(fname);
  auto nevents = io.nevents();

  const int nlayers = 10;
  TH1 *hNhits[nlayers];
  for (int ilayer = 0; ilayer < nlayers; ++ilayer)
    hNhits[ilayer] = new TH1F(Form("hNhits_%02d", ilayer), "", 10000, 0., 10000.);
  
  for (int iev = 0; iev < nevents; ++iev) {

    io.event(iev);

    // count number of hits on layers
    int nhits[nlayers] = {0};
    for (int ihit = 0; ihit < io.hits.n; ++ihit) {
      auto layer = io.hits.lyrid[ihit];
      nhits[layer]++;
    }

    // fill histograms
    for (int ilayer = 0; ilayer < nlayers; ++ilayer)
      hNhits[ilayer]->Fill(nhits[ilayer]);
    
  }

  auto fout = TFile::Open("hits.root", "RECREATE");
  for (int ilayer = 0; ilayer < nlayers; ++ilayer)
    hNhits[ilayer]->Write();
  fout->Close();
    
}
