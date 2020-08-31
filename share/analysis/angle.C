#include "io.C"

#include "style.C"

double n1 = 1.03; // refractive index of radiator
double n2 = 1.00; // refractive index of expansion

double D = 2.; // [cm] thickness of radiator
double L = 20.; // [cm] thickness of expansion
//double emissionR = 107.; // [cm] radius of emission point
double emissionR = 105.5; // [cm] radius of emission point
double sensorR = 129.; // [cm] radius of sensor surface

TAxis sensorZ_axis(2000, -300., 300.);
TAxis sensorRPhi_axis(3000, -450., 450.);

bool usePDE = true;
bool cheatOnEmission = false;
bool nominalEmission = true;
bool usePixel = true;
double filterLambda = 0.; // [nm] wavelength filter cutoff

// double thetaMin = 0.213; // 4 cm
// double thetaMax = 0.265; // 4 cm
//double thetaMin = 0.223; // 2 cm
//double thetaMax = 0.255; // 2 cm
//double thetaMin = 0.229; // 1 cm
//double thetaMax = 0.250; // 1 cm
double thetaMin = 0.225;//2.43621e-01 - 3. * 5.65526e-03;
//double thetaMax = 2.43621e-01 + 3. * 5.65526e-03;
double thetaMax = 0.260;

TH1 *
angle(const char *fname, double rindex = 1.01, double emission = 105., double refraction = 1.02)
{

  const double hc = 197.3269788 * 2. * M_PI; // [eV nm]
  
  /** read PDE from file **/
  TGraph gEfficiency("hamamatsu_S13360-xx50CS.txt");
  for (int i = 0; i < gEfficiency.GetN(); ++i) {
    gEfficiency.GetY()[i] *= 0.01;
  }

  
  style();
  
  io.open(fname);
  auto nevents = io.nevents();

  //  auto hMap = new TH2F("hMap", ";r#varphi (cm);z (cm)", 1000, -500., 500., 400, -200., 200.);
  auto hMap = new TH2F("hMap", ";r#varphi (cm);z (cm)", 100, -15., 15., 100, -15., 15.);
  auto hAngle = new TH1F("hAngle", ";#theta_{Ch} (rad)", 2000, 0., 1.);
  auto hhhAngle = new TH1F("hhhAngle", ";#theta_{Ch} (rad)", 10000, 0., 1.);
  auto hTime = new TH1F("hTime", ";time (ns)", 10000, 0., 10.);
  auto hhhTime = new TH1F("hhhTime", ";time (ns)", 10000, 0., 10.);
  auto hhhTimeP = new TH2F("hhhTimeP", ";time (ns)", 1000, 0., 10., 1000, 0., 10.);
  auto hBeta = new TH1F("hBeta", ";#beta = v/c", 1000, 0.95, 1.05);
  auto hhhBeta = new TH1F("hhhBeta", ";#beta = v/c", 1000, 0.95, 1.05);
  auto hhhBetaP = new TH2F("hAngleP", ";#beta = v/c", 1000, 0., 10., 1000, 0.95, 1.05);
  auto hAngleP = new TH2F("hAngleP", ";#beta = v/c", 1000, 0., 10., 1000, 0., 1.);

  auto hhhN = new TH1F("hhhN", "N_{pe}", 100, 0., 100.);

  
  auto h2 = new TH2F("h2", "", 200, 0., 10., 1000, 0., 1.);

  auto hhAngle = new TH1F("hhAngle", ";#theta_{Ch} (rad)", 500, 0., 0.5);
  auto hhMap = new TH2F("hhMap", ";r#varphi (cm);z (cm)", 1000, -500., 500., 400, -200., 200.);

  auto hEnergy = new TH1F("hEnergy", "", 800, 200., 1000.);
  auto hEnergy2 = new TH1F("hEnergy2", "", 800, 200., 1000.);

  auto hAngleLambda = new TH2F("hAngleLambda", ";#theta_{Ch} (rad)", 800, 200., 1000., 2000, 0., 1.);

  
  auto ccAngle = new TCanvas("ccAngle", "ccAngle", 800, 800);
  ccAngle->Divide(1, 2);
  
  TVector3 emissionV(emission, 0., 0.);
  TVector3 photonHitV, apparentV, trackV, cherenkovV, perpV, surfaceV;
  TVector3 trackingHit[2];

  TMarker m(0., 0., 20);
  
  for (int iev = 0; iev < nevents; ++iev) {

    io.event(iev);

    for (int itrk = 0; itrk < io.tracks.n; ++itrk) {

      if (io.tracks.parent[itrk] != -1) continue;

      auto px = io.tracks.px[itrk];
      auto py = io.tracks.py[itrk];
      auto pz = io.tracks.pz[itrk];
      auto p = sqrt(px * px + py * py + pz * pz);
      
#if 0
      // calculate track emission point and track direction
      int nTrackingHits = 0;
      for (int ihit = 0; ihit < io.hits.n; ++ihit) {
	auto trkid = io.hits.trkid[ihit];
	if (trkid != itrk) continue;
	auto x = io.hits.x[ihit];
	auto y = io.hits.y[ihit];
	auto z = io.hits.z[ihit];
	auto r = hypot(x, y);
	auto phi = atan2(y, x);
	
	if (io.hits.lyrid[ihit] == 9) {
	  trackingHit[nTrackingHits].SetXYZ(x, y, z);
	  nTrackingHits++;
	}
	if (io.hits.lyrid[ihit] == 0 && r > 120.) {
	  trackingHit[nTrackingHits].SetXYZ(x, y, z);
	  nTrackingHits++;
	} 
      }

      if (nTrackingHits != 2) continue;
      
      
      TVector3 trackingDir = trackingHit[1] - trackingHit[0];
      auto trackingDirUnit = trackingDir.Unit();
      trackV = trackingDir;
      //      trackingDir.Print();

      // find the emission point
      double ex = trackingHit[0][0];
      double ey = trackingHit[0][1];
      double ez = trackingHit[0][2];
      double er = hypot(ex, ey);
      while (er < emissionR) {
	ex += 0.01 * trackingDirUnit[0];
	ey += 0.01 * trackingDirUnit[1];
	ez += 0.01 * trackingDirUnit[2];
	er = hypot(ex, ey);
      }
      //      std::cout << " --- got emission radius: " << er << std::endl;
      emissionV.SetXYZ(ex, ey, ez);

      //      trackingHit[0].Print();
      //      trackingHit[1].Print();
      //      emissionV.Print();
      
      hhAngle->Reset();
      hhMap->Reset();

      auto mr = hypot(trackingHit[1][0], trackingHit[1][1]);
      auto mphi = atan2(trackingHit[1][1], trackingHit[1][0]);
      auto mz = trackingHit[1][2];
      m.SetX(mr * mphi);
      m.SetY(mz);
#endif

      emissionV.SetXYZ(emissionR, 0., 0.);
      trackV.SetXYZ(1., 0., 0.);
      
      int theAngles = 0;
      double theAngle = 0.;
      double theTime = 0.;
      double theBeta = 0.;
      for (int ihit = 0; ihit < io.hits.n; ++ihit) {

	// apply PDE
	auto eV = io.hits.e[ihit] * 1.e9;
	auto lambda = hc / eV;
	double pde = usePDE ? gEfficiency.Eval(lambda) : 1.;
	//	std::cout << " --- pde: " << pde << std::endl;
	if (pde < 0) continue;
	if (pde < gRandom->Uniform()) continue;
	
	// kill hits randomly
	//      if (gRandom->Uniform() < 0.9) continue;
	
	// get track id
	auto trkid = io.hits.trkid[ihit];
	
	// skip mother hits
	//      if (trkid == 0) continue;
	
	// only hits on optical sensor
	double x = io.hits.x[ihit];
	double y = io.hits.y[ihit];
	double z = io.hits.z[ihit];
	double t = io.hits.t[ihit] + gRandom->Gaus(0., 0.05); // 50 ps smearing
	double r = hypot(x, y);
	double phi = atan2(y, x);
	double rphi = r * phi;
	if (r < 120.) continue;

	// simulate pixellisation
	if (usePixel) {
	  int iz = sensorZ_axis.FindBin(z);
	  int irphi = sensorRPhi_axis.FindBin(rphi);
	  z = sensorZ_axis.GetBinCenter(iz);
	  rphi = sensorRPhi_axis.GetBinCenter(irphi);
	  x = r * cos(rphi / r);
	  y = r * sin(rphi / r);	 
	}
	
	hMap->Fill(rphi, z);

	// cheat on the emission point
	//	auto trkid = io.hits.trkid[ihit];
	double vx = io.tracks.vx[trkid];
	double vy = io.tracks.vy[trkid];
	double vz = io.tracks.vz[trkid];
	double vr = hypot(vx, vy);
	if (cheatOnEmission) emissionV.SetXYZ(vx, vy, vz);
	else if (nominalEmission) emissionV.SetXYZ(emissionR, 0., 0.);
	
	// WARNING: we need to take care of refraction in air!

	// compute Cherenkov angle
	photonHitV.SetXYZ(x, y, z);
       	apparentV = photonHitV - emissionV;

	surfaceV = emissionV;
	surfaceV.SetZ(0);
	
	double d = D;//109. - surfaceV.Mag();
	double apparentTheta = apparentV.Angle(surfaceV);
	//	auto correctedTheta = asin(n2 / n1 * sin(apparentTheta));
	double correctedTheta = asin((d + L) / (d + L * n1 / n2) * sin(apparentTheta));
	double deltaTheta = apparentTheta - correctedTheta;
	perpV = apparentV.Cross(surfaceV);
	cherenkovV = apparentV;
	cherenkovV.Rotate(deltaTheta, perpV);
	

	//	auto trackTheta = trackV.Angle(emissionV);

	//	auto angle = correctedTheta - trackTheta;
	
	auto angle = cherenkovV.Angle(trackV);
	auto beta = 1. / n1 / cos(angle);
	
	//	std::cout << " --- angle: " << angle << std::endl;
	//	cherenkovV.Print();
	//	trackingDir.Print();
	
	// hit position wrt. emission point
	x -= emission;
	y -= 0.;
	z -= 0.;
	auto costheta = (x * 1. + y * 0. + z * 0.) / sqrt(x * x + y * y + z * z);
	//      std::cout << "angle = " << angle << "   " << acos(costheta) << std::endl;
	
	// hit time wrt. emission time
	t -= apparentV.Mag() / 29.979246;
	//      t -= sqrt(x * x + y * y + z * z) / 29.979246;
	
	// approximate correction for refraction
	costheta = sqrt(1. - (1. - costheta * costheta) / (refraction * refraction));
	
	//      angle = acos(costheta);
	//      beta = 1. / rindex / costheta;

	if (angle < 1.e-6) continue;
	
	hAngleLambda->Fill(lambda, angle);
	hAngle->Fill(angle);
	hhAngle->Fill(angle);
	hTime->Fill(t);
	hBeta->Fill(beta);

	hAngleP->Fill(p, angle);
	
	h2->Fill(t, angle);

	if (angle < 0.5)
	  hhMap->Fill(rphi, z);

	
	if (angle > thetaMin && angle < thetaMax) {
	  hEnergy->Fill(lambda);
	  theAngle += angle;
	  theTime += t;
	  theBeta += beta;
	  theAngles++;
	}
	if (angle > thetaMin - (thetaMax - thetaMin) && angle < thetaMin) {
	  hEnergy2->Fill(lambda);
	}
	if (angle > thetaMax && angle < thetaMax + (thetaMax - thetaMin)) {
	  hEnergy2->Fill(lambda);
	}
	
      }

      theAngle /= (double)theAngles;
      theTime /= (double)theAngles;
      theBeta /= (double)theAngles;

      hhhAngle->Fill(theAngle);
      hhhTime->Fill(theTime);
      hhhTimeP->Fill(p, theTime);
      hhhBeta->Fill(theBeta);
      hhhBetaP->Fill(p, theBeta);

      hhhN->Fill(theAngles);
      
      continue;
      
      ccAngle->cd(1);
      hhAngle->Draw();
      ccAngle->cd(2);
      hhMap->Draw("colz");
      m.DrawClone("same");
      ccAngle->Modified();
      ccAngle->Update();

      getchar();
      //      break;


    }

    //    break;
  }
    
  auto c = new TCanvas("c", "c", 1800, 600);
  c->Divide(3, 1);
  c->cd(1);
  hMap->Draw("colz");
  c->cd(2);
  hAngle->Draw();
  c->cd(3);
  hBeta->Draw();

  auto ct = new TCanvas("ct", "ct", 800, 800);
  hTime->Draw();

  m.Print();
  
  hMap->SetStats(0);
  auto cring = new TCanvas("cring", "cring", 800, 800);
  hMap->Draw("colz");
  m.DrawClone("same");
  cring->SaveAs("ring.png");

  new TCanvas("c2");
  h2->Draw("colz");
  
  new TCanvas("c3");
  hAngleP->Draw("colz");

  new TCanvas("c4");
  hEnergy->Draw();
  hEnergy2->Scale(0.5);
  hEnergy2->Draw("same");

  new TCanvas("c5");
  hhhAngle->Draw();
  
  new TCanvas("c6");
  hhhTime->Draw();
  
  new TCanvas("c7");
  hhhBeta->Draw();
  
  new TCanvas("c8");
  hhhN->Draw();

  new TCanvas("c9");
  hhhBetaP->Draw("colz");
  
  new TCanvas("c10");
  hhhTimeP->Draw("colz");

  new TCanvas("c11");
  hAngleLambda->Draw("colz");
  
  //  return hEnergy;
  return hAngle;
}
