#include "io.C"

using O2Track = o2::track::TrackParCov;

void makeO2Track(const O2Track &o2track,
		 std::array<float, 3> xyz,
		 std::array<float, 3> ptetaphi,
		 std::array<float, 15> covm,
		 int charge);

void propagateO2Track(const O2Track &o2track, float x);

const float errYZ = 0., errSlp = 0., errQPT = 2e-2;
std::array<float, 15> covm = {
  errYZ * errYZ,
  0., errYZ * errYZ,
  0, 0., errSlp * errSlp,
  0., 0., 0., errSlp * errSlp,
  0., 0., 0., 0., errQPT * errQPT
};

void
smearing(const char *fname)
{
  io.open(fname);
  auto nevents = io.nevents();

  O2Track o2track;
  auto dbpdg = TDatabasePDG::Instance();
  
  // loop over events
  for (int iev = 0; iev < nevents; ++iev) {
    
    io.event(iev);

    // loop over tracks
    for (int itrk = 0; itrk < io.tracks.n; ++itrk) {

      auto ppdg = dbpdg->GetParticle(io.tracks.pdg[itrk]);
      if (!ppdg) continue;
      
      auto x = io.tracks.vx[itrk];
      auto y = io.tracks.vy[itrk];
      auto z = io.tracks.vz[itrk];
      auto pt = hypot(io.tracks.px[itrk], io.tracks.py[itrk]);
      auto theta = atan2(pt, io.tracks.pz[itrk]);
      auto eta = -log(tan(0.5 * theta));
      auto phi = atan2(io.tracks.py[itrk], io.tracks.px[itrk]);
      auto charge = ppdg->Charge() / 3.;
      
      std::array<float, 3> xyz = {x, y, z};
      std::array<float, 3> ptetaphi = {pt, eta, phi};
      makeO2Track(o2track, xyz, ptetaphi, covm, io.tracks.charge[itrk]);
      
    }
  }
  
}

/*****************************************************************/

void makeO2Track(const O2Track &o2track,
		 std::array<float, 3> xyz,
		 std::array<float, 3> ptetaphi,
		 std::array<float, 15> covm,
		 int charge)
{
  float s, c, x;
  std::array<float, 5> params;
  o2::utils::sincosf(ptetaphi[2], s, c);
  o2::utils::rotateZInv(xyz.[0], xyz[1], x, params[0], s, c);
  params[1] = vz;
  params[2] = 0.; // since alpha = phi
  auto theta = 2. * atan(exp(-ptetaphi[1]));  
  params[3] = 1. / tan(theta);
  params[4] = charge / ptetaphi[0];
  
  covm[14] = errQPT * errQPT * params[4] * params[4];

  new (&o2track) O2Track(x, ptetaphi[2], params, covm);

}

