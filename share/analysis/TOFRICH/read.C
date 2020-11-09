#define MAXARRAY 65536 //1048576

float sensor_r = 122.; // [cm] radius of sensor surface
float emission_r = 101.; // [cm] radius of emission point

float radiator_n = 1.03; // refractive index of radiator
float expansion_n = 1.0003; // refractive index of expansion

float radiator_dr = 2.; // [cm] radial thickness of radiator
float expansion_dr = 20.; // [cm] radial thickness of expansion

bool useTiming = true;
bool cheatEmission = false;
bool cheatMatch = false;
bool cheatMIP = false;

void
read(const char *fname, int startev = 0, int maxev = kMaxInt)
{

  std::cout << " --- " << (useTiming ? "" : "not ") << "using timing" << std::endl;
  
  /** open input file **/
  auto fin = TFile::Open(fname);

  /** get tracks tree **/
  auto track_tree = (TTree *)fin->Get("tracks");
  int track_n, track_pdg[MAXARRAY], track_label[MAXARRAY];
  float track_x[MAXARRAY], track_y[MAXARRAY], track_z[MAXARRAY], track_t[MAXARRAY];
  float track_px[MAXARRAY], track_py[MAXARRAY], track_pz[MAXARRAY], track_l[MAXARRAY];
  track_tree->SetBranchAddress("n", &track_n);
  track_tree->SetBranchAddress("pdg", &track_pdg);
  track_tree->SetBranchAddress("label", &track_label);
  track_tree->SetBranchAddress("x", &track_x);
  track_tree->SetBranchAddress("y", &track_y);
  track_tree->SetBranchAddress("z", &track_z);
  track_tree->SetBranchAddress("t", &track_t);
  track_tree->SetBranchAddress("px", &track_px);
  track_tree->SetBranchAddress("py", &track_py);
  track_tree->SetBranchAddress("pz", &track_pz);
  track_tree->SetBranchAddress("l", &track_l);

  /** get hits tree **/
  auto hit_tree = (TTree *)fin->Get("hits");
  int hit_n, hit_label[MAXARRAY];
  float hit_x[MAXARRAY], hit_y[MAXARRAY], hit_z[MAXARRAY], hit_t[MAXARRAY], hit_vx[MAXARRAY], hit_vy[MAXARRAY], hit_vz[MAXARRAY];
  hit_tree->SetBranchAddress("n", &hit_n);
  hit_tree->SetBranchAddress("label", &hit_label);
  hit_tree->SetBranchAddress("x", &hit_x);
  hit_tree->SetBranchAddress("y", &hit_y);
  hit_tree->SetBranchAddress("z", &hit_z);
  hit_tree->SetBranchAddress("t", &hit_t);
  hit_tree->SetBranchAddress("vx", &hit_vx);
  hit_tree->SetBranchAddress("vy", &hit_vy);
  hit_tree->SetBranchAddress("vz", &hit_vz);

  /** histograms **/
  auto hMap = new TH2F("hMap", ";r#varphi (cm);z (cm)", 2440, -383.27430, 383.27430, 1280, -201.06193, 201.06193);
  auto hAngle = new TH1F("hAngle", ";#theta_{Ch} (rad)", 200, 0., 1.);
  auto hAngleP = new TH2F("hAngleP", ";p (GeV/c);#theta_{Ch} (rad)", 10000, 0., 100., 200, 0., 1.);
  auto hAllAngle = new TH1F("hAllAngle", ";#theta_{Ch} (rad)", 2000, 0., 1.);
  auto hAllAngle_tof = new TH1F("hAllAngle_tof", ";#theta_{Ch} (rad)", 2000, 0., 1.);
  auto hTime = new TH1F("hTime", ";time (ns)", 100, 0., 10.);

  auto hAngleTime = new TH2F("hAngleTime", ";#theta_{Ch} (rad);time (ns)", 200, 0., 1., 100, 0., 10.);

  auto hBetaBeta = new TH2F("hBetaBeta", ";#beta_{Ch};#beta_{TOF}", 1000, 0.5, 1.5, 1000, 0.5, 1.5);

  auto hDeltaBeta = new TH1F("hDeltaBeta", "", 1000, -0.5, 0.5);

  auto hDeltaT = new TH1F("hDeltaT", "", 1000, -5., 5.);
  auto hDeltaAngle = new TH1F("hDeltaAngle", "", 1000, -0.5, 0.5);
  auto hDeltaAngle_tof = new TH1F("hDeltaAngle_tof", "", 1000, -0.5, 0.5);
  
  auto hAverageAngle = new TH1F("hAverageAngle", ";#theta_{Ch} (rad)", 20000, 0., 1.);
  auto hAverageAngleP = new TH2F("hAverageAngleP", ";p (GeV/c);#theta_{Ch} (rad)", 1000, 0., 10., 2000, 0., 1.);
  auto hBeta = new TH1F("hBeta", ";#beta", 10000, 0.5, 1.5);
  auto hAverageBeta = new TH1F("hAverageBeta", ";#beta", 10000, 0.5, 1.5);
  auto hAverageBetaP = new TH2F("hAverageBetaP", ";p (GeV/c);#beta", 1000, 0., 10., 10000, 0.5, 1.5);
  auto gEmission = new TGraph();
  auto gMIP = new TGraph();

  auto hGenPt = new TH1F("hGenPt", "", 1000, 0., 10.);
  auto hDeltaBetaRecPt = new TH2F("hDeltaBetaRecPt", "", 1000, 0., 10., 100, -0.01, 0.01);
  auto hDeltaAngleRecP = new TH2F("hDeltaAngleRecP", "", 1000, 0., 10., 200, -0.1, 0.1);
  
  auto cDisplay = new TCanvas("cDisplay");
  cDisplay->Divide(3, 2);
  
  /** useful suff **/
  TVector3 emissionV, directionV, photonHitV, apparentV, surfaceV, perpV, cherenkovV;

  bool stopMe = false;
  
  /** loop over events **/
  auto nevents = track_tree->GetEntries();
  std::cout << nevents << " events" << std::endl;
  for (int iev = startev; (iev < nevents) && (iev < maxev); ++iev) {

    if (iev % 1000 == 0) std::cout << iev << std::endl;
    
    track_tree->GetEntry(iev);
    hit_tree->GetEntry(iev);

    /** loop over tracks **/
    for (int itrk = 0; itrk < track_n; ++itrk) {

      //      if (abs(track_pdg[itrk]) != 11) continue;

      //      if (fabs(track_z[itrk]) > 50.) continue;
      
      /** track information **/
      double p = sqrt(track_px[itrk] * track_px[itrk] +
		      track_py[itrk] * track_py[itrk] +
		      track_pz[itrk] * track_pz[itrk]);

      double track_p = sqrt(track_px[itrk] * track_px[itrk] +
		      track_py[itrk] * track_py[itrk] +
		      track_pz[itrk] * track_pz[itrk]);

      double track_pt = sqrt(track_px[itrk] * track_px[itrk] +
			     track_py[itrk] * track_py[itrk]);
      
      double track_eta = std::atanh(track_pz[itrk] / track_p);

      if (std::fabs(track_eta) > 1.0) continue;

      hGenPt->Fill(track_p);
      
      //      if (p < 0.2) stopMe = true;
      //      else stopMe = false;
      //      if (p > 0.2) continue;
      
      /** set emission point **/
      emissionV.SetXYZ(track_x[itrk], track_y[itrk], track_z[itrk]);

      /** set track direction vector **/
      directionV.SetXYZ(track_px[itrk], track_py[itrk], track_pz[itrk]);

      double average_angle = 0., average_beta = 0.;
      int n_angle = 0;

      double_t track_beta = track_l[itrk] / track_t[itrk] / 29.979246;
      double_t track_angle = acos(1. / radiator_n / track_beta);
      
      /** skip if track is below Cherenkov threshold **/
      //      if (isnan(track_angle)) continue;

      //      std::cout << track_angle << std::endl;
      
      //      hMap->Reset();
      hAngle->Reset();
      hTime->Reset();
      hAngleTime->Reset();
      //      hBetaBeta->Reset();
      //      hDeltaBeta->Reset();
      
      /** loop over hits **/
      for (int ihit = 0; ihit < hit_n; ++ihit) {

	/** cheat on MIP **/
	if (cheatMIP && hit_label[ihit] < 0)
	  continue;
	
	/** cheat on match **/
	if (cheatMatch && hit_label[ihit] != track_label[itrk])
	  continue;

	double x = hit_x[ihit];
	double y = hit_y[ihit];
	double z = hit_z[ihit];
	double t = hit_t[ihit];
	double r = hypot(x, y);
	double phi = atan2(y, x);
	double rphi = r * phi;
	//	hMap->Fill(rphi, z);

	/** cheat on emission **/
	if (cheatEmission)
	  emissionV.SetXYZ(hit_vx[ihit], hit_vy[ihit], hit_vz[ihit]);
	
	/** calculate Cherenkov angle **/
	photonHitV.SetXYZ(hit_x[ihit], hit_y[ihit], hit_z[ihit]);	
	apparentV = photonHitV - emissionV;
	surfaceV = emissionV;
	surfaceV.SetZ(0);

	auto n1 = radiator_n;
	auto n2 = expansion_n;
	auto d = radiator_dr;
	auto L = expansion_dr;
	double apparentTheta = apparentV.Angle(surfaceV);
	double correctedTheta = asin((d + L) / (d + L * n1 / n2) * sin(apparentTheta));
	double deltaTheta = apparentTheta - correctedTheta;

	perpV = apparentV.Cross(surfaceV);
	cherenkovV = apparentV;
	cherenkovV.Rotate(deltaTheta, perpV);

	auto angle = cherenkovV.Angle(directionV);
	auto beta = 1. / n1 / cos(angle);

	//	if (isnan(angle)) continue;
	
	hAngle->Fill(angle);
	hAllAngle->Fill(angle);
	hBeta->Fill(beta);
	hDeltaAngle->Fill(angle - track_angle);

	/** calculate Cherekov emission time **/
	auto emission_t = t - apparentV.Mag() / 29.979246;
	auto beta_tof = track_l[itrk] / emission_t / 29.979246;

	/** remove MIP-related angles **/
	//	if (angle < 0.1) continue;
	
	hDeltaT->Fill(emission_t - track_t[itrk]);
	hTime->Fill(emission_t);
	hAngleTime->Fill(angle, emission_t);

	/** beta-beta **/

	hBetaBeta->Fill(beta, beta_tof);
	hDeltaBeta->Fill(beta - beta_tof);

	/** remove hits that are not consistent with expectations **/
	if (useTiming && fabs(emission_t - track_t[itrk]) > 3. * 0.05) continue;
	//	if (fabs(angle - track_angle) > 5. * 0.01) continue;
	//	if (angle - track_angle < 10. * 0.01) continue;

	hMap->Fill(rphi, z);

	hAllAngle_tof->Fill(angle);
	hDeltaAngle_tof->Fill(angle - track_angle);
	
	/** remove if cherenkov angle not consistent with time-of-flight **/
	if (useTiming && fabs(beta - beta_tof) > 0.05) continue;

	hAngleP->Fill(p, angle);

	/** poor man ring angle **/

	/** cut on beta **/
	//	if (beta < 0.97 || beta > 1.01) continue;
	
	/** remove unphysically large angles **/
	if (angle > 0.26) continue;

	/** remove MIP hit **/
	if (angle < 0.04) continue;
	

	
	average_angle += angle;
	average_beta += beta;
	n_angle++;
	
      } /** end of loop over hits **/

      if (n_angle < 3) continue;

      average_angle /= n_angle;
      average_beta /= n_angle;
      
      hAverageAngle->Fill(average_angle);
      hAverageAngleP->Fill(p, average_angle);
      
      hAverageBeta->Fill(average_beta);
      hAverageBetaP->Fill(p, average_beta);

      hDeltaBetaRecPt->Fill(track_p, average_beta - track_beta);
      hDeltaAngleRecP->Fill(track_p, average_angle - track_angle);
      
      if (stopMe) break;
      
      continue;
      
      cDisplay->cd(1);
      hMap->Draw("box");
      gEmission->Draw("same,p*");
      gMIP->Draw("same,p*");
      cDisplay->cd(2);
      hAngle->Draw();
      cDisplay->cd(3);
      hTime->Draw();
      cDisplay->cd(4);
      //      hAngleTime->Draw("col");
      hBetaBeta->Draw("box");
      cDisplay->cd(5);
      hDeltaBeta->Draw();
      
      cDisplay->Update();
      getchar();
    
      
    } /** end of loop over tracks **/

    if (stopMe) break;
    
  } /** end of loop over events **/

  new TCanvas("cMap");
  hMap->Draw("box");
  new TCanvas("cAllAngle");
  hAllAngle->Draw();
  hAllAngle_tof->Draw("same");
  new TCanvas("cAverageAngle");
  hAverageAngle->Draw();
  new TCanvas("cAverageAngleP");
  hAverageAngleP->Draw("colz");
  new TCanvas("cBeta");
  hBeta->Draw();
  new TCanvas("cAverageBeta");
  hAverageBeta->Draw();
  new TCanvas("cAverageBetaP");
  hAverageBetaP->Draw("colz");

  new TCanvas("cDeltaBeta");
  hDeltaBeta->Draw();
  new TCanvas("cBetaBeta");
  hBetaBeta->Draw("colz");

  new TCanvas("cDeltaT");
  hDeltaT->Draw();
  new TCanvas("cDeltaAngle");
  hDeltaAngle->Draw();
  hDeltaAngle_tof->Draw("same");

  new TCanvas("pAngleP");
  hAngleP->Draw("colz");

  new TCanvas("cGenPt");
  hGenPt->Draw();
  
  new TCanvas("cDeltaBetaRecPt");
  hDeltaBetaRecPt->Draw("colz");
  
  new TCanvas("cDeltaAngleRecP");
  hDeltaAngleRecP->Draw("colz");
  
}
