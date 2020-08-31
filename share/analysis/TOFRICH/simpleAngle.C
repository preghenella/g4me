#include "io.C"

#include "style.C"

double n1 = 1.03; // refractive index of radiator
double n2 = 1.00; // refractive index of expansion

double radiatorRin = 105.; // [cm]
double radiatorL   = 4.; // [cm]
double expansionL  = 20.; // [cm]

double D = 4.; // [cm] thickness of radiator
double L = 20.; // [cm] thickness of expansion
//double emissionR = 107.; // [cm] radius of emission point
double emissionR = 109.; // [cm] radius of emission point
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
double thetaMin = 0.05;//2.43621e-01 - 3. * 5.65526e-03;
//double thetaMax = 2.43621e-01 + 3. * 5.65526e-03;
double thetaMax = 0.5;

void
simpleAngle(const char *fname)
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

  TVector3 emissionV(emission, 0., 0.);
  TVector3 photonHitV, apparentV, trackV, cherenkovV, perpV, surfaceV;
  TVector3 trackingHit[2];

  TMarker m(0., 0., 20);
  
  /** loop over events **/
  for (int iev = 0; iev < nevents; ++iev) {

    io.event(iev);

    /** loop over hits **/
    for (int ihit = 0; ihit < io.hits.n; ++ihit) {
      
      /** skip hit if parent is not optical photon **/
      auto trkid = io.hits.trkid[ihit];
      if (io.tracks.pdg[trkid] != 0) continue;
      
      /** apply PDE **/
      auto eV = io.hits.e[ihit] * 1.e9;
      auto lambda = hc / eV;
      double pde = usePDE ? gEfficiency.Eval(lambda) : 1.;
      if (pde < 0) continue;
      if (pde < gRandom->Uniform()) continue;
      
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
    
}
