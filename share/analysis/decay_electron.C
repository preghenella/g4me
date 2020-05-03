#include "io.C"

void
decay_electron(const char *fname)
{
  io.open(fname);
  auto nevents = io.nevents();

  /** histogram map for mothers **/
  std::map<int, TH1*> hMother;

  /** loop over events **/
  for (int iev = 0; iev < nevents; ++iev) {

    io.event(iev);

    /** loop over tracks **/
    for (int itrk = 0; itrk < io.tracks.n; ++itrk) {

      // electron tracks
      if (io.tracks.pdg[itrk] != 11) continue;

      auto e = io.tracks.e[itrk];
      auto imother = io.tracks.parent[itrk];
      int motherpdg = 0;

      if (imother == -1) { /** this is a primary electron injected in Geant4, we need to get the
			       particle index and find the mother in the tree of particles **/
	
	auto iparticle = io.tracks.particle[itrk];  // get electron particle index
	imother = io.particles.parent[iparticle];   // get mother index in the particle tree
	motherpdg = abs(io.particles.pdg[imother]); // get electron mother PDG code
	std::cout << " --- I am a primary electron, my mother abs(pdg) is " << motherpdg << std::endl;
      }
      
      else if (io.tracks.proc[itrk] == io.fDecay &&
	       io.tracks.parent[imother] == -1) { /** this is a secondary electron created in Geant4
						      from the decay of a primary particle **/

	motherpdg = abs(io.tracks.pdg[imother]); // get electron mother PDG code
	std::cout << " --- I am a secondary electron, my mother abs(pdg) is " << motherpdg << std::endl;
      }

      else { /** this is a secondary electron created in Geant4
		 from a process that is not a decay or 
		 from the decay of a secondary particle **/
	continue;
      }

      
      // create histogram for this mother, if it does not exist
      if (!hMother.count(motherpdg))
	hMother[motherpdg] = new TH1F(Form("hMother_%d", motherpdg), "", 60, -3., 3.);
      hMother[motherpdg]->Fill(log10(e));
      
    }
  }

  /** normalise and write to file **/
  auto fout = TFile::Open("decay_electron.root", "RECREATE");
  for (auto &h : hMother) {  
    h.second->Sumw2();
    h.second->Scale(1. / nevents);
    h.second->Write();
  }
  fout->Close();
  
}
