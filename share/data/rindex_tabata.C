void
rindex_tabata()
{

  const double hc = 197.3269788 * 2. * M_PI; // [eV nm]
  
  double a0 = 0.0616;
  double lambda0 = 56.5;
  
  double energy[18] = {1.23984, 1.3051, 1.3776, 1.45864, 1.5498, 1.65312, 1.7712, 1.90745, 2.0664, 2.25426, 2.47968, 2.7552, 3.0996, 3.54241, 4.13281, 4.95937, 6.19921, 8.26561};

  auto g = new TGraph();
  
  for (int i = 0; i < 18; ++i) {
    auto lambda = hc / energy[i];
    double n = sqrt(1. + a0 * lambda * lambda / (lambda * lambda - lambda0 * lambda0));
    g->SetPoint(i, lambda, n);
    std::cout << n << ", ";
  }
  std::cout << std::endl;

  auto lambda = 400.;
  std::cout << " --- value at 400 nm: " << sqrt(1. + a0 * lambda * lambda / (lambda * lambda - lambda0 * lambda0)) << std::endl;

  g->Draw("ap*");
}
