#pragma once

// C++ incliude
#include <cassert>
// stl include
#include <list>
// Algorithm package includes
#include "../Algorithm/dbpp_IFluxAlgoImpl.h"
// discretization package
#include "../Discretization/dbpp_EmcilNumTreatment.h"
#include "../Discretization/dbpp_NodalVariables.h"
// algorithm package
#include "../SfxTypes/dbpp_ListSectFlow.h"

namespace dbpp {
/** Brief Possible implementation of numerical algorithm using base class of the
 * framework. In the current version, we have a base class for numerical
 * discretization of the right-hand-side terms. But user may want to provide its
 * own implementation of the flux algorithm (for example E. McNeil Nujic
 * algorithm but decided for some) reason to implement a different algo for flux
 * face evaluation.
 *
 * Reminder: 'BaseNumTreatmemt' is a base class that provide a base
 * implementation of the right-hand-side terms(numeric approximation).
 *
 *  @Usage
 *    good practice no memory leak
 *    std::unique_ptr<BaseNumTreatmemt> w_Bnum( new TestCalculFF);
 *    to call IFluxAlgo implementation, i need to cast i guess
 *    but i have a pointer to the base class, i thought it will
 *    be possible to access the interface method directly.
 *
 * if( IFluxAlgoImpl* w_ifalgo = dynamic_cast<IFluxAlgoImpl*>(w_Bnum.get()))
 * 		{
 *   ...
 * 			// compute the flux at cell interface
 * 			w_ifalgo->calculFF( m_swerhs.m_FF1, m_swerhs.m_FF2,
 * w_U1LR, w_U2LR);
 *   ...
 *   }
 *
 */
class TestCalculFF : public BaseNumTreatmemt, // provide basic implementation of
                                              // the RHS algorithm
                     public IFluxAlgoImpl // provide another implementation of
                                          // the flux algorithm
{
public:
  /** In this version the reconstruction procedure have been removed inside of
   * the flux algorithm. This way we don't need to modify the flux algorithm to
   * change the reconstruction procedure just switch to another. By
   * encapsulating method in class, Usage: reconstr_vec( const scalarField&
   * aScalrField, , vecULUR); // return calculFF(..., vecULUR);
   */
  void calculFF(std::vector<double> &FF1,
                std::vector<double> &FF2, /*const std::vector<double>& U1,*/
                /*const std::vector<double>& U2,*/ const cellfaceVar &aU1LR,
                const cellfaceVar &aU2LR) final override;

  /**
   *
   * Reconstruction outside of the Riemann solver (assume that ...)
   */
  void calculFF(const std::list<cellFace> &aListOface, std::vector<double> &FF1,
                std::vector<double> &FF2) final override;

  /**
   *   deprecated
   */
  // void calculFF(std::vector<double> &FF1, std::vector<double> &FF2,
  //            const std::vector<double> &U1, const std::vector<double> &U2,
  //          const ListSectFlow *aListSectFlow);

  /**
   *   new signature of this method
   */
  void TraitementTermeSource2(std::vector<double> &S,
                              const std::vector<double> &Q,
                              const std::vector<double> &A, const double aDx,
                              ListSectFlow *aListSectFlow);

  /**
   *   another signature which is probably more natural
   */
  //   void TraitementTermeSource2( std::vector<double>& S, const
  //   dbpp::scalarField* Q, const dbpp::scalarField* A, const ListSectFlow*
  //   aListSectFlow);

  /**
   *  Set boundary condition for this ....
   */
  //   void setBoundaryCondition( const Nodal_Value& aUpstreanBC, const
  //   Nodal_Value& aDownstreaBC)
  //  {
  // set boundary condition for this ....
  //    }
private:
  // shall we add a method for the boundary condition
  Nodal_Value m_upstreamBCValues;   /**< */
  Nodal_Value m_downstreamBCValues; /**< */
};
} // namespace dbpp
