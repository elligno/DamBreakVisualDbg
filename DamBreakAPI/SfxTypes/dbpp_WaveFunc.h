#pragma once

// file        : WaveFunc.h
// description : contains class WaveFunc and subclasses. WaveFunc is an
//               abstract class that defines the interface of a function
//               to be used in the wave simulation.
//               the subclasses implement specific functions describing
//               an initial wave.

#include <math.h>
#include <string>

// debugging purpose
#define M_PI 3.141592

//#include "real.h"
// i am not sure yet what for (de bugging purpose)
typedef double real;

namespace dbpp {
/** class hierarchy WaveFunc is used to represent
 * various choices of H(x,y,t) and I(x,y) functions:
 */
class WaveFunc {
public:
  virtual ~WaveFunc() = default;

  virtual real valuePt(real x, real y, real t = 0) = 0;
  virtual void scan() = 0;            // read parameters
  virtual std::string &formula() = 0; // function label
};

// subclasses of WaveFunc implement specific H and I functions

class GaussianBell : public virtual WaveFunc {
protected:
  real A, sigma_x, sigma_y, xc, yc;
  char funcname;
  std::string formula_str; // for ASCII output of function
public:
  GaussianBell(char funcname_ = ' ');
  virtual real valuePt(real x, real y, real t = 0);
  virtual void scan();
  virtual std::string &formula();
};

inline real GaussianBell::valuePt(real x, real y, real) {
  auto r = A * exp(-(::sqrt(x - xc) / (2 * ::sqrt(sigma_x)) +
                     ::sqrt(y - yc) / (2 * ::sqrt(sigma_y))));
  return r;
  // return 0.;  debugging purpose
}

//	class Plug : public virtual WaveFunc
//	{
//	protected:
//		real A;                   // amplitude of plug
//		real sigma;               // steepness parameter
//		char funcname;            // ... to be completed
//		std::string formula_str;  // for ASCII output of function
//	public:
//		Plug(char funcname_ = ' ');
//		virtual real valuePt(real x, real y, real t = 0);
//		virtual void scan();
//		virtual std::string& formula();
//	};

//	inline real Plug:: valuePt(real x, real y, real)
//	{
//		real r;
//		if (x > 0)
//			r = 0.5 - atan(sigma*(x - 2))/M_PI;
//		else
//			r = 0.5 + atan(sigma*(x + 2))/M_PI;
//		return r;
//	}

class Flat : public virtual WaveFunc {
protected:
  std::string formula_str; // for ASCII output of function
  real m_z;

public:
  Flat() {}
  virtual real valuePt(real, real, real = 0) { return m_z; }
  virtual void scan();
  virtual std::string &formula();
};

// we are considering variable topography in one-dimension (z!=0)
class ZFunc1D : public virtual WaveFunc {
public:
  virtual real valuePt(real, real, real = 0) { return 0.; }
  virtual void scan() { /*not implemented yet*/
  }
  virtual std::string &formula();

protected:
  std::string formula_str; // for ASCII output of function
                           // real m_z; a set of values
};

// to be completed (Jean Belanger)
class Step1D : public virtual WaveFunc {
protected:
  std::string formula_str; // for ASCII output of function, default value?
  real m_Phi1;
  real m_Phi0;
  real m_shockPos;
  char funcname;

public:
  Step1D(char funcname_ = ' ')
      : m_Phi1(10.),      // E.McNeil as default
        m_Phi0(1.),       // E.McNeil as default
        m_shockPos(500.), // E.McNeil as default
        funcname{funcname_} {}
  real valuePt(real x, real y = 0., real t = 0.) override;
  void scan() override;
  // don't understand what for?
  std::string &formula() override { return formula_str; }
  // some method of the class to change value of step function
  void setPhi1(const double aPhi1) { m_Phi1 = aPhi1; }
  double getPhi1() const { return m_Phi1; }
  void setPhi0(const double aPhi0) { m_Phi0 = aPhi0; }
  double getPhi0() const { return m_Phi0; }
  void setShockLocation(double aShockLoc) { m_shockPos = aShockLoc; }
  double getShockLocation() const { return m_shockPos; }
};
} // namespace dbpp
