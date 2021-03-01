#include "io.C"
#include "style.C"

// Check PVIDMapFile.dat to make sure that's the ID you are looking for
const int absoPreLayerID = 300;
const int absoPostLayerID = 302;

const int pdgSearch = 13;


void muon(const char *ifname, const char *ofname)
{
  style();

  // open simulation file
  io.open(ifname);
  auto nEvents = io.nevents();

  // open output file
  TFile of(ofname, "RECREATE");
  TH1F histPMuonsPrimary("histPMuonsPrimary", "histPMuonsPrimary", 100, 0., 20.);
  TH1F histPMuonsPreAbso("histPMuonsPreAbso", "histPMuonsPreAbso", 100, 0., 20.);
  TH1F histPMuonsPostAbso("histPMuonsPostAbso", "histPMuonsPostAbso", 100, 0., 20.);

  // loop over events
  for(int i = 0; i < nEvents; i++) {
    io.event(i);

    for(int itrack = 0; itrack < io.tracks.n; itrack++) {
      if(TMath::Abs(io.tracks.pdg[itrack]) == pdgSearch) {
        histPMuonsPrimary.Fill(TMath::Sqrt(io.tracks.px[itrack]*io.tracks.px[itrack] + io.tracks.py[itrack]*io.tracks.py[itrack] + io.tracks.pz[itrack]*io.tracks.pz[itrack]));
      }
    }

    for(int ihit = 0; ihit < io.hits.n; ihit++) {
      if(io.hits.lyrid[ihit] != absoPostLayerID && io.hits.lyrid[ihit] != absoPreLayerID) {
        continue;
      }
      auto trackID = io.hits.trkid[ihit];
      // only catch primary muons
      if(TMath::Abs(io.tracks.pdg[trackID]) != pdgSearch || io.tracks.parent[trackID] >= 0) {
        continue;
      }
      if(io.hits.lyrid[ihit] == absoPostLayerID) {
        histPMuonsPostAbso.Fill(TMath::Sqrt(io.hits.px[ihit]*io.hits.px[ihit] + io.hits.py[ihit]*io.hits.py[ihit] + io.hits.pz[ihit]*io.hits.pz[ihit]));
      } else {
        histPMuonsPreAbso.Fill(TMath::Sqrt(io.hits.px[ihit]*io.hits.px[ihit] + io.hits.py[ihit]*io.hits.py[ihit] + io.hits.pz[ihit]*io.hits.pz[ihit]));
      }
    }
  }
  of.cd();
  histPMuonsPrimary.Write();
  histPMuonsPreAbso.Write();
  histPMuonsPostAbso.Write();
  of.Close();

}
