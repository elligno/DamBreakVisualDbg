#pragma once

// STL include
#include <vector>
// library include
#include "dbpp_HLL_RK2_Algo_Vec.h"
// Discretization package
#include "../Discretization/dbpp_NodalVariables.h"

// forward declarations
namespace dbpp {
class GlobalDiscretization;
class TestCalculFF;
} // namespace dbpp

namespace dbpp {
/** Design Note: we experiment the concept of global discretization
 *   and the relationship between the numerical method and the global
 *   discretization. For now it's more a proof-of-concept
 *
 *  Usage:
 *  ------
 *    Omega w_om;
 *    U_h w_u12(w_om);
 *    GlobalFVDiscr w_gDiscr(w_U12,w_om,w_gam);
 *    EMcNeil1D* w_testAlgo=new = new TestNewAlgo(w_gDiscr);
 *    w_testAlgo->advance(...);
 *    ... to be continued!!!
 */
class TestNewAlgo : public EMcNeil1D {
public:
  void setInitSln(const StateVector &aU,
                  ListSectFlow *aListofSect = nullptr) override final;

  // ...
  void setState() override final;

protected:
  // new signature to avoid to have an attribute
  void timeStep(
      /*GlobalDiscretization* aGblDiscr, PhysicalSystem aPhySys*/) override;
  // ???
  //  void copyUpScalar2Vec(std::vector<double> &aA, std::vector<double> &aQ);

  void setBC();

  // not overriding base class (be carefull because these
  // methods are declared virtual in base class, which will be used)
  // i think base class will selected, not sure
  // Anyway, in the next version, it's the way we shall declare
  void predictor();
  void corrector();

private:
  // this state vector class need a serious re-factoring
  StateVector m_U12;  // initial cnd
  StateVector m_U12p; // intermediate

  // temporary variables for debugging purpose (not usre about all of these)
  std::vector<double> m_U1;
  std::vector<double> m_U2;
  std::vector<double> U1;
  std::vector<double> U2;

  // 101 sections flow and used in the reconstruction procedure
  GlobalDiscretization *m_gdisr; // global discretization concept!!! NO
  ListSectFlow *m_ListSectFlow;  // need it as an attribute? don't get it!!
  // std::unique_ptr<TestCalculFF> m_numTreat;  need as an attribute? no, just
  // create it inside the method!!
  // 		Nodal_Value m_amontBC; // not in use
  // 		Nodal_Value m_avalBC;  // not in use
  //		void setBCNode();
  void setSectionFlowBC(); //??
};
} // namespace dbpp
