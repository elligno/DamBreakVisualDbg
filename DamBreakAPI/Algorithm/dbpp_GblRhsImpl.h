#pragma once

#include <algorithm>

// package includes
#include "../Algorithm/dbpp_SweRhsAlgorithm.h"
#include "../Discretization/dbpp_EmcilNumTreatment.h"
#include "../Numerics/dbpp_SourceTermsAlgorithms.h"
#include "../SfxTypes/dbpp_DamBreakData.h"
#include "../Utility/dbpp_AppConstant.hpp"
#include "../Utility/dbpp_TestLogger.h"

namespace dbpp {
class ListSectFlow;
}
namespace dbpp {

/** @brief Rhs algorithm implementation using second signature
 *
 *  more comment coming ...
 *
 */
class GblRhgsImpl : public SweRhsAlgorithm {
public:
  /** @brief Use implementation BaseNumTreatment is just a component
   *  POM (Physics Object Model). POM provides an implementation
   */
  void calculate(const StateVector &aStateVec,
                 const GlobalDiscretization *aGblDiscr) override {

    using namespace std;
    using namespace std::placeholders;

    // sanity check (debugging purpose)
    assert(aStateVec.first->values().size() == DIM::value);

    auto w_vU1 = aStateVec.first->values().to_stdVector();  // A
    auto w_vU2 = aStateVec.second->values().to_stdVector(); // Q

    // boundary condition to applied at both end
    const Gamma &w_bc = aGblDiscr->gamma();
    w_bc.applyBC(); // not sure??
    const auto [Am, Qm, Hm] = w_bc.getBCNodeAmont().Values();
    const auto [Av, Qv, Hv] = w_bc.getBCNodeAval().Values();

    // apply B.C. upstream
    w_vU1[0] = Am; // get<0>(w_bcUpstream.Values()); // A
    w_vU2[0] = Qm; // get<1>(w_bcUpstream.Values()); // Q

    // apply B.C. downstream (Ghost node)
    w_vU1.push_back(Av); // A
    w_vU2.push_back(Qv); // Q

    // for debugging purpose (temporary fix) shall be set by the simulation
    // i am no sure about that or read from an xml file with the property
    // tree from boost
    DamBreakData w_dbDat(DamBreakData::DiscrTypes::emcneil);

    // Use the default implementation of the Base class
    std::vector<double> w_vDU1(EMCNEILNbSections::value);
    std::vector<double> w_vDU2(EMCNEILNbSections::value);

    // temporary fix (computational domain)
    std::vector<double> w_FF1(DIM::value);
    std::vector<double> w_FF2(DIM::value);

    BaseNumTreatmemt w_baseTreatment; // original algorithm (deprecated)
    // compute flux at cell face according to Riemann problem
    // MUSCL recconstruction is done (need the ghost node)
    w_baseTreatment.CalculFF(w_FF1, w_FF2, w_vU1, w_vU2, w_vDU1, w_vDU2,
                             w_dbDat.nbSections(), w_dbDat.getWidth());

    // if(m_listSectionFlow->isFlatBed())

    // bathymetry
    const std::vector<double> &w_Z = w_dbDat.getIC().m_Z;
    std::vector<double> w_vH;
    w_vH.reserve(w_vU1.size());

    // now compute the water level
    std::transform(w_vU1.cbegin(), w_vU1.cend(), w_Z.cbegin(),
                   std::back_inserter(w_vH),
                   std::bind(&HydroUtils::Evaluation_H_fonction_A, _1, 1., _2));

    // Will be deprecated in the near future
    std::vector<double> w_S(DIM::value); // computational domain

    // source term discretization (this is the original signature of the
    // function)
    w_baseTreatment.TraitementTermeSource2(
        w_S, w_vU2, w_vU1, w_vH, // ranges
        w_dbDat.getManning(), aStateVec.first->grid().Delta(1),
        w_dbDat.nbSections(), w_dbDat.getWidth());

    // use the new signature by returning valarray (want to check)
    SourceTermsTreatment w_testImpl;
    auto w_srcArray = // this is more understanable
        w_testImpl.TraitementTermeSource2(aStateVec, w_bc, m_listSectFlow);

    dbpp::Logger::instance()->OutputSuccess(
        std::string{"Exiting of the GBLRhsImpl::calculate() method\n"}.data());

    std::cout << "Exiting of the SweRhsAlgorithm::calculate method\n";
  }

private:
  ListSectFlow *m_listSectFlow;
};
} // namespace dbpp
