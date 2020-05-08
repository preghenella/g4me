#include "io.C"

#include "style.C"

TH1 *
angle(const char *fname, double emission = 105., double refraction = 1.02)
{

  style();
  
  io.open(fname);
  auto nevents = io.nevents();

  auto hMap = new TH2F("hMap", ";r#varphi (cm);z (cm)", 300, -15., 15., 300, -15., 15.);
  auto hAngle = new TH1F("hAngle", ";#theta_{Ch} (rad)", 10000, 0., 1.);
  auto hTime = new TH1F("hTime", ";time (ns)", 10000, 0., 10.);
  auto hBeta = new TH1F("hBeta", ";#beta = v/c", 1000, 0.95, 1.05);

  auto h2 = new TH2F("h2", "", 10, 0., 10., 10, 0., 10.);
  
  TVector3 emiV(emission, 0., 0.);
  TVector3 hitV, cheV;
  
  for (int iev = 0; iev < nevents; ++iev) {

    io.event(iev);
    
    for (int ihit = 0; ihit < io.hits.n; ++ihit) {

      // get track id
      auto trkid = io.hits.trkid[ihit];
      
      // skip mother hits
      if (trkid == 0) continue;

      // only hits on optical sensor
      auto x = io.hits.x[ihit];
      auto y = io.hits.y[ihit];
      auto z = io.hits.z[ihit];
      auto t = io.hits.t[ihit];
      auto r = hypot(x, y);
      auto phi = atan2(y, x);
      auto rphi = r * phi;
      if (r < 120.) continue;

      hMap->Fill(rphi, z);

      // WARNING: we need to take care of refraction in air!
      
      // compute apparent Cherenkov angle
      hitV.SetXYZ(x, y, z);
      cheV = emiV - hitV;
      auto angle = cheV.Angle(emiV) * TMath::DegToRad();
      auto beta = 1. / 1.02 / cos(angle);

      
      // hit position wrt. emission point
      x -= emission;
      y -= 0.;
      z -= 0.;
      auto costheta = (x * 1. + y * 0. + z * 0.) / sqrt(x * x + y * y + z * z);
      //      std::cout << "angle = " << angle << "   " << acos(costheta) << std::endl;

      // hit time wrt. emission time
      t -= sqrt(x * x + y * y + z * z) / 29.979246;
      
      // approximate correction for refraction
      costheta = sqrt(1. - (1. - costheta * costheta) / (refraction * refraction));
      
      angle = acos(costheta);
      beta = 1. / 1.02 / costheta;
      
      hAngle->Fill(angle);
      hTime->Fill(t);
      hBeta->Fill(beta);
      
    }
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

  hMap->SetStats(0);
  auto cring = new TCanvas("cring", "cring", 800, 800);
  hMap->Draw("col");
  cring->SaveAs("ring.png");

  return hAngle;
}
