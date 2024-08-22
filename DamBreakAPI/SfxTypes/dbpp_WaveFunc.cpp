// file        : jb_WaveFunc.cpp
// description : implementation of classes in WaveFunc.h

// C include
#include <stdlib.h>
// App. includes
#include "../Utility/dbpp_CommandLineArgs.h"
#include "dbpp_WaveFunc.h"

namespace dbpp {
//
// Gaussian Bell
//
GaussianBell::GaussianBell(char funcname_) : funcname(funcname_) {}

std::string &GaussianBell::formula() { return formula_str; }

void GaussianBell::scan() {
  using namespace std::literals;
  auto w_A = "A"s;
  w_A += funcname; // add at the end of the string
  A = CmdLineArgs::read(w_A.data(), 0.1);
  auto w_sigma_x = "-sigma_x_"s;
  w_sigma_x += funcname;
  sigma_x = CmdLineArgs::read(w_sigma_x.data(), 0.5);
  auto w_sigma_y = "-sigma_y_"s;
  w_sigma_y += funcname;
  sigma_y = CmdLineArgs::read(w_sigma_y.data(), 0.5);
  auto w_xc = "-xc_"s;
  w_xc += funcname;
  xc = CmdLineArgs::read(w_xc.data(), 0.0);
  auto w_yc = "-yc_"s;
  w_yc += funcname;
  yc = CmdLineArgs::read(w_yc.data(), 0.0);
}

//
// Plug
//
//	Plug:: Plug(char funcname_)
//	: funcname(funcname_)
//	{}

//	void Plug:: scan ()
//	{
//		A = CmdLineArgs::read("-A", 0.1);
//		sigma = CmdLineArgs::read("-sigma", 1000.0);
//	}

//	std::string& Plug::formula()
//	{ return formula_str; }

//
// Flat
//
std::string &Flat::formula() { return formula_str; }

void Flat::scan() {
  // read something to specify that we are in a flat topography
  // flag or something ...
  m_z = 0.; // CmdLineArgs::read("-z", 0.);
}

//
// Step1D
//

// step function water level and shock position for this simulation
// shock location shall read from simulation config? i am not sure?
// let's say that we read it from command line for now
// If these flags are not specified in the cmd line args,
// then it takes the default values specified in the read parameters
void Step1D::scan() {
  // water level each side of the wall (in this project we are using E McNeil)
  // values and they are set in the cmd arg line (loaded at the startup).
  m_Phi1 = 10.; // CmdLineArgs::read("-phi1", 1.);  E. McNeil = 10.
  m_Phi0 = 1.;  // CmdLineArgs::read("-phi0", 1.);  E. McNeil = 1.
  // shock position (default value is set)
  m_shockPos = 500.; // CmdLineArgs::read("-shock", 5.);  E. McNeil = 500.
}

// Step function for dam-break
//
// E. McNeil code
// for (j = 0; j < NbSections; j++) 101 points [0,...,100]
//{
//	if (X[j] < 500) shock location
//		H[j] = 10.;
//	else
//		H[j] = 1.;
//
// NOTE
// In the original E. McNeil code, x is a separate variable
// by using the more structured type such as gridLattice
// we encapsulate all the information about the grid in a
// class and its easier to manage. min and max x position,
// delta x and so on. In E. McNeil code X is a global variable
// dx is another variable calculated somewhere else, it is not
// clear what is the min and max of the grid extent.
//
real Step1D::valuePt(real x, real y, real t /* = 0 */) {
  if (y < 0 || t < 0) // actually this is a patch, since inherit from
    return -1.;       // WaveFunc no choice to have 3 args

  double r{};
  if (x < m_shockPos) // shock location (the shock location shall be ...)
    r = m_Phi1;
  else
    r = m_Phi0;

  return r;
}
} // namespace dbpp
