
// C++ includes
#include <cassert>
#include <deque>
#include <functional>
#include <numeric>
#include <valarray>
// boost includes
#include <boost/assert.hpp>
#include <boost/range/iterator_range.hpp>

// package includes
#include "../Utility/dbpp_EMcNeilUtils.h"
#include "dbpp_GlobalDiscretization.h"
#include "dbpp_ReconstrUtility.h"

namespace dbpp {
//
// int ReconstrUtil::NbSections=101;

// NOTE
//   could be done with std::valarray which has an operator '+', suppose to be
//   more more efficient on floating point operation, it would be nice if we
//   could measure performance between both implementation.
//
void ReconstrUtil::reconstr_val(
    const dbpp::scalarField &aField, /* field to reconstruct at cell face */
    std::vector<std::pair<double, double>>
        &aVecULR) // vector shall have reserve enough place
{
  using namespace std;
  using namespace std::placeholders;

  // ...
  const int NbSections = aField.values().size() + 1;

  // ...
  vector<real> w_U(aField.values().getPtr(),
                   aField.values().getPtr() + aField.values().size());
  //		w_U.reserve( aField.values().size()+1); // add ghost node

  // filed contains computational domain values
  // 		const real* w_arrPtr = aField.values().getPtr();
  // 		std::copy( w_arrPtr, w_arrPtr+aField.values().size(),
  // 			std::back_inserter(w_tmpUi));

  // setting boundary nodes value and ghost node
  // setBCvalues(w_tmpUi);
  //	Gamma& w_bc = GlobalDiscretization::instance()->gamma();

  // sanity check just to  make sure  that we added the extra value (ghost)
  assert(aField.values().size() + 1 ==
         static_cast<int>(w_U.size())); // asset will work on debug

  vector<real> w_dU;
  //		vector<real> w_dU2;
  //	Calculs préalables: évaluation des éléments du vecteur dU
  w_dU[0] = HydroUtils::minmod(w_U[1] - w_U[0], 0.);

  //		w_dU2[0] = HydroUtils::minmod( aU2[1]-aU2[0], 0.);
  // change loop index because out-of-range error
  // i=1, i<NbSections-1 compare to Eric McNeil code ()
  // and added the boundary cnd. for the last node (ghost node i=100)
  for (int i = 1; i < NbSections - 1; i++) // last element index=100
  {                                        // i+1 -> 101 (doesn't exist)
    w_dU[i] = HydroUtils::minmod(w_U[i + 1] - w_U[i], w_U[i] - w_U[i - 1]);
    //			w_dU2[i] = HydroUtils::minmod( aU2[i+1]-aU2[i],
    // aU2[i]-aU2[i-1]);
  }

  // special treatment i=NbSections-1=100??
  // IMPORTANT: shall change the index looping above. There is 2 errors
  // 1: i=0 which is wrong because this case already considered
  // 2: i=100 go out of range (U[101] doesn't exist
  // i=1;i<NbSections-1 [1,...,99]
  // put these lines below in comment and run the algorithm and see what happens
  w_dU[NbSections - 1] =
      HydroUtils::minmod(0., w_U[NbSections - 1] - w_U[NbSections - 2]);
  //		w_dU2[NbSections-1] = HydroUtils::minmod( 0.,
  // aU2[NbSections-1]-aU2[NbSections-2]);

  // actually we are looping over cell face (j+1/2) and not grid node
  // we first perform a reconstruction of state variable
  // at the interface by using a MUSCL interpolation type
  for (int i = 0; i < NbSections - 1;
       i++) // actually we are looping on the cell face
  {
    //	Calcul des éléments des vecteurs UR, UL [0,...,100]

    // first variable
    double U1L = w_U[i] + 0.5 * w_dU[i];         // [0,...,99]
    double U1R = w_U[i + 1] - 0.5 * w_dU[i + 1]; // [1,...,100]

    // second variable
    // 			double U2L = aU2[i] + 0.5*w_dU2[i];
    // 			double U2R = aU2[i+1] - 0.5*w_dU2[i+1];

    // debugging purpose (comparison with the other algorithm)
    aVecULR.push_back(make_pair(U1L, U1R));
    //			aVecU2LR.push_back(make_pair(U2L,U2R));
  }

  // compute the gradient at first-order using the stl numeric algorithm
  // for now this algorithm need to be validated
  // TODO:
  //		vector<real> w_tmpdUi; //= computeDU(w_tmpUi);
  // compute the TVD of the gradient by taking st::min as slope limiter
  //		auto w_stdminDU = applyLimiter(w_tmpUi); // could pass as
  // argument the
  // slope limiter function, ut for now just testing
  //	auto w_stdminDU = applyLimiter(computeDU(w_tmpUi));

  // just in case (sanity check)
  //		assert( w_tmpdUi.size() == w_tmpUi.size());

  //
  // left state
  //

  // 		std::valarray<real> w_vUi(w_Ui.data(),w_Ui.size());
  // 		std::valarray<real> w_vdUi(w_dUi.data(),w_dUi.size());

  // just a test if we could do that kind of operation
  // 		valarray<real> w_Ui( vector<real>(w_tmpUi.begin(),
  // w_tmpUi.end()-1).data(), 			aField.values().size());
  // valarray<real> w_dUi(vector<real>(w_tmpdUi.begin(),
  // w_tmpdUi.end()-1).data(), 			aField.values().size());
  // 		// compute left state according to MUSCL formula
  // 		valarray<real> w_UL=w_Ui+0.5*w_dUi; // one line of code, that's
  // it!
  // 		// create a vector a left state (UL)
  // 		vector<real> w_vecUL(begin(w_UL),end(w_UL));

  // can use the iterator concept with valarray
  // 		for( auto it = begin(w_UL); it!=end(w_UL); ++it)
  // 		{
  // 			std::cout << *it++ << "\n";
  // 		}

  //
  // right state
  //

  // just a test if we could do that kind of operation
  // 		valarray<real> w_Uip1( vector<real>(w_tmpUi.begin()+1,
  // w_tmpUi.end()).data(), 			aField.values().size());
  // valarray<real> w_dUip1( vector<real>(w_tmpdUi.begin()+1,
  // w_tmpdUi.end()).data(), 			aField.values().size());
  // 		// compute right state according to MUSCL formula
  // 		valarray<real> w_UR=w_Uip1-0.5*w_dUip1;
  // 		// create a vector of right state (UR)
  // 		vector<real> w_vecUR(begin(w_UR),end(w_UR));

  // check to make sure
  // 		BOOST_ASSERT( aVecULR.capacity()==aField.values().size()==0 &&
  // "Capacity size ok"); 		BOOST_ASSERT( aVecULR.size()==0 && "Size
  // empty");

  // create pair of reconstructed field at cell face variable(UL,UR)
  // 		transform( w_vecUL.begin(),w_vecUL.end(),w_vecUR.begin(),
  // 			std::back_inserter(aVecULR), bind(
  // &make_pair<double,double>,_1,_2));
}

// not completed, removed setBCvalues() will be handled by GlobalDiscrtetization
void ReconstrUtil::reconstr_rng(
    const dbpp::scalarField &aField, /* field to reconstruct at cell face */
    std::vector<std::pair<double, double>> &aVecULR) {
  using namespace std;
  using namespace std::placeholders;
  using namespace boost;

  // ...
  vector<real> w_vecU;
  w_vecU.reserve(aField.values().size() + 1);
  // filed contains computational domain values
  const auto w_arrPtr = aField.values().getPtr();
  std::copy(w_arrPtr, w_arrPtr + aField.values().size(),
            std::back_inserter(w_vecU));

  // add ghost node
  //		setBCvalues(w_vecU); probably use the GlobalDiscretization
  // sanity check just to  make sure  that we added the extra value (ghost)
  assert(aField.values().size() + 1 ==
         static_cast<int>(w_vecU.size())); // asset will work on debug
  // just to make sure everything is ok let's double check
  //	assert(w_vecU.size()==NbSections);

  // compute gradient at first order and apply the minmod slope limiter
  // based on the stl algorithm call adjacent_difference
  vector<double> w_vDU;
  //	w_vDU.reserve(NbSections);  i am  not sure about this line, need to be
  // validated!!!
  computeMinModDU(w_vecU, w_vDU);

  //
  // left-state vector range [0,...,99] -> UL=U_i + 0.5*dU_i
  //
  vector<real> w_vecUL;                    // UL=U_i+0.5*dU_i
  w_vecUL.reserve(aField.values().size()); // computational domain

  // C++20 ranges (counted views) return subrange
  // auto w_rngUi = std::views::counted(w_vecU.begin(), w_vecU.size()-1);

  // boost version range
  auto w_rngUi = make_iterator_range(w_vecU.begin(), w_vecU.end() - 1);
  auto w_rngdUi = make_iterator_range(w_vDU.begin(), w_vDU.end() - 1);

  // boost range version
  transform(w_rngUi.begin(), w_rngUi.end(),
            w_rngdUi.begin(), // both ranges to add: U+0.5*dU
            back_inserter(w_vecUL), plus<real>()); // store result UL

  //
  // right state vector range [1,...,100] -> UR=U_i+1 - 0.5*dU_i+1
  //

  // C++20 ranges (counted views) return subrange
  // auto w_rngUi = std::views::counted(w_vecU.begin()+1, w_vecU.size());

  vector<real> w_vecUR;                    // UL=U_i+0.5*dU_i
  w_vecUL.reserve(aField.values().size()); // computational domain

  // boost version range
  auto w_rngUip1 = make_iterator_range(w_vecU.begin() + 1, w_vecU.end());
  auto w_rngdUip1 = make_iterator_range(w_vDU.begin() + 1, w_vDU.end());

  // boost range version
  std::transform(w_rngUi.begin(), w_rngUi.end(),
                 w_rngdUip1.begin(), // both ranges to add: U+0.5*dU
                 std::back_inserter(w_vecUL),
                 std::plus<real>()); // store result UL

  // create pair of reconstructed field at cell face variable(UL,UR)
  // 		transform( w_vecUL.begin(),w_vecUL.end(),w_vecUR.begin(),
  // 			back_inserter(aVecULR),
  // bind(&make_pair<real,real>,_1,_2));

  aVecULR[0].first = 0.; // debugging purpose
  aVecULR[0].second = 0.;
}

// a am not sure if we really need it!!
// 	void ReconstrUtil::setBCvalues( std::vector<double>& aVecU)
// 	{
// for the purpose of this algorithm, we need to add a ghost node
// and probably to set the boundary condition (i am not sure yet)
// maybe we could use the ....
// IMPORTANT:
//   the boundary value has been already applied in the advance loop
//   we are just re-using them here, no need to call B.C. again
//		aVecU[0] = U1[0];
//	w_vecU[0] = U2[0];
// for the purpose of ...
//		assert( aField.values().size()==NbSections-1);
// ... push_back (add a ghost node and set value with B.C.)
//	w_vecU[NbSections-1] = U1[NbSections-1];
//		aVecU.push_back( U1[NbSections-1]); // be careful with global
// variable!!! 		assert( aVecU.size() == NbSections);
// w_vecU[NbSections-1] = U2[NbSections-1];
//	}

// temporary implementation (this is crapt!!)
void ReconstrUtil::computeMinModDU(const std::vector<real> &aVecU,
                                   std::vector<double> &aDU) {
  using namespace std;
  using namespace std::placeholders;

  //
  // using the stl numeric algorithm (adjacent difference) to compute
  // first-order gradient and then apply the slope limiter function
  //

  // use deque because we want to pop up the first element
  std::deque<double> w_vDU(aVecU.size()); // i am not sure about this one
  // w_vDU.resize(w_vecU.size());
  // zero added at the beginning to compare first element with something
  std::adjacent_difference(aVecU.begin(), aVecU.end(),
                           std::back_inserter(w_vDU));
  assert(w_vDU.size() == aVecU.size());
  //  because first element is set to 0 by the algo to compute the first diff.
  w_vDU.pop_front(); // might as well to remove it
  // just in case (sanity check)
  assert(w_vDU.size() == aVecU.size() - 1);
  // add 0. at the end for the b.c. node
  w_vDU.push_back(0.);
  assert(w_vDU.size() == aVecU.size());
  // add a zero again and it is exactly what we want
  std::adjacent_difference(w_vDU.begin(), w_vDU.end(), w_vDU.begin(),
                           bind(&HydroUtils::minmod, _1, _2));
  // we are stick again with a zero at the beginning, must get rid of it
  w_vDU.pop_front();

  // range (be careful, we are returning a copy ...)
  aDU.assign(w_vDU.begin(), w_vDU.end());
}

// NOTE: i think this one has been tested.
void ReconstrUtil::reconstr_vec(const std::vector<double> &aU1,
                                const std::vector<double> &aU2,
                                vecULR &aVecU1LR, vecULR &aVecU2LR) {
  using namespace std;

  // compute gradient at first order and apply the minmod slope limiter
  // based on the stl algorithm call adjacent_difference
  std::vector<double> w_dU1(aU1.size());
  // dU1.reserve( aU1.size());
  std::vector<double> w_dU2(aU2.size());
  // dU2.reserve( aU2.size());

  std::cout << "Compute the gradient over each cell\n";

  // debugging purpose we set this variable
  // this is one of the reason why using standard array instead
  // of mathematical based type is a problem. Array doesn't
  // contains any information. By passing a scalarField we could
  // retrieve the grid and the number of points of the computational
  // domain.
  const auto NbSections = aU1.size(); // deprecated
  assert(NbSections == 101);          // this is very bad

  //	Calculs préalables: évaluation des éléments du vecteur dU
  w_dU1[0] = HydroUtils::minmod(aU1[1] - aU1[0], 0.);
  w_dU2[0] = HydroUtils::minmod(aU2[1] - aU2[0], 0.);
  // change loop index because out-of-range error
  // i=1, i<NbSections-1 compare to Eric McNeil code ()
  // and added the boundary cnd. for the last node (ghost node i=100)
  for (unsigned i = 1; i < NbSections - 1; i++) // last element index=100
  {                                             // i+1 -> 101 (doesn't exist)
    w_dU1[i] = HydroUtils::minmod(aU1[i + 1] - aU1[i], aU1[i] - aU1[i - 1]);
    w_dU2[i] = HydroUtils::minmod(aU2[i + 1] - aU2[i], aU2[i] - aU2[i - 1]);
  }

  // special treatment i=NbSections-1=100??
  // IMPORTANT: shall change the index looping above. There is 2 errors
  // 1: i=0 which is wrong because this case already considered
  // 2: i=100 go out of range (U[101] doesn't exist
  // i=1;i<NbSections-1 [1,...,99]
  // put these lines below in comment and run the algorithm and see what happens
  w_dU1[NbSections - 1] =
      HydroUtils::minmod(0., aU1[NbSections - 1] - aU1[NbSections - 2]);
  w_dU2[NbSections - 1] =
      HydroUtils::minmod(0., aU2[NbSections - 1] - aU2[NbSections - 2]);

  // actually we are looping over cell face (j+1/2) and not grid node
  // we first perform a reconstruction of state variable
  // at the interface by using a MUSCL interpolation type
  for (unsigned i = 0; i < NbSections - 1;
       i++) // actually we are looping on the cell face
  {
    //	Calcul des éléments des vecteurs UR, UL [0,...,100]

    // first variable
    double U1L = aU1[i] + 0.5 * w_dU1[i];         // [0,...,99]
    double U1R = aU1[i + 1] - 0.5 * w_dU1[i + 1]; // [1,...,100]
    // second variable
    double U2L = aU2[i] + 0.5 * w_dU2[i];
    double U2R = aU2[i + 1] - 0.5 * w_dU2[i + 1];

    // debugging purpose (comparison with the other algorithm)
    aVecU1LR.push_back(make_pair(U1L, U1R));
    aVecU2LR.push_back(make_pair(U2L, U2R));
  }
}

void ReconstrUtil::reconstrv_sv(const StateVector &aU, vecULR &aVecU1LR,
                                vecULR &aVecU2LR) {
  using namespace std;

#if 0 // deprecated (older version0)
		vector<double> w_vU1; // A
		w_vU1.reserve( aU.first->values().size()+1);
		vector<double> w_vU2; // Q
		w_vU2.reserve( aU.second->values().size()+1);

		// only computational node
		aU.first->values().to_stdVector(w_vU1);
		aU.second->values().to_stdVector(w_vU2);
#endif

  vector<double> w_vU1 = aU.first->values().to_stdVector();  // A
  vector<double> w_vU2 = aU.second->values().to_stdVector(); // Q

  // boundary condition to applied at both end
  Gamma &w_bc = GlobalDiscretization::instance()->gamma();
  w_bc.applyBC();
  const Nodal_Value &w_bcUpstream = w_bc.getBCNodeAmont();
  const Nodal_Value &w_bcDownstream = w_bc.getBCNodeAval();

  // apply B.C. upstream
  w_vU1[0] = get<0>(w_bcUpstream.Values()); // A
  w_vU2[0] = get<1>(w_bcUpstream.Values()); // Q

  // need to do a push_back because the scalar field hold computational
  // node only. In the reconstruction process, in the case under study,
  // a ghost node is used (added at the end of the grid but not part of
  // the computational domain, extrapolate)

  // apply B.C. downstream
  w_vU1.push_back(get<0>(w_bcDownstream.Values())); // A
  w_vU2.push_back(get<1>(w_bcDownstream.Values())); // Q

  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  //
  // call the version reconstr_vec(); above
  // no point to re-write
  // the algorithm, error prone. Once reconstr_vec() has been tested
  // re-use it!!!
  // all lines below can be replaced by the following call
  // reconstr_vec(w_vU1,w_vU2, aVecU1LR,aVecU2LR); end of the story

  // compute gradient at first order and apply the minmod slope limiter
  // based on the stl algorithm call adjacent_difference
  std::vector<double> w_dU1(w_vU1.size());
  std::vector<double> w_dU2(w_vU2.size());

  // Calculs préalables: évaluation des éléments du vecteur dU
  w_dU1[0] = HydroUtils::minmod(w_vU1[1] - w_vU1[0], 0.);
  w_dU2[0] = HydroUtils::minmod(w_vU2[1] - w_vU2[0], 0.);

  const unsigned NbSections = GlobalDiscretization::instance()->getNbSections();
  // change loop index because out-of-range error
  // i=1, i<NbSections-1 compare to Eric McNeil code ()
  // and added the boundary cnd. for the last node (ghost node i=100)
  for (unsigned i = 1; i < NbSections - 1; i++) // last element index=100
  {                                             // i+1 -> 101 (doesn't exist)
    w_dU1[i] =
        HydroUtils::minmod(w_vU1[i + 1] - w_vU1[i], w_vU1[i] - w_vU1[i - 1]);
    w_dU2[i] =
        HydroUtils::minmod(w_vU2[i + 1] - w_vU2[i], w_vU2[i] - w_vU2[i - 1]);
  }

  // special treatment i=NbSections-1=100??
  // IMPORTANT: shall change the index looping above. There is 2 errors
  // 1: i=0 which is wrong because this case already considered
  // 2: i=100 go out of range (U[101] doesn't exist
  // i=1;i<NbSections-1 [1,...,99]
  w_dU1[NbSections - 1] =
      HydroUtils::minmod(0., w_vU1[NbSections - 1] - w_vU1[NbSections - 2]);
  w_dU2[NbSections - 1] =
      HydroUtils::minmod(0., w_vU2[NbSections - 1] - w_vU2[NbSections - 2]);

  // actually we are looping over cell face (j+1/2) and not grid node
  // we first perform a reconstruction of state variable
  // at the interface by using a MUSCL interpolation type
  for (unsigned i = 0; i < NbSections - 1;
       i++) // actually we are looping on the cell face
  {
    //	Calcul des éléments des vecteurs UR, UL [0,...,100]

    // first variable
    double U1L = w_vU1[i] + 0.5 * w_dU1[i];         // [0,...,99]
    double U1R = w_vU1[i + 1] - 0.5 * w_dU1[i + 1]; // [1,...,100]
    // second variable
    double U2L = w_vU2[i] + 0.5 * w_dU2[i];
    double U2R = w_vU2[i + 1] - 0.5 * w_dU2[i + 1];

    // debugging purpose (comparison with the other algorithm)
    aVecU1LR.push_back(make_pair(U1L, U1R));
    aVecU2LR.push_back(make_pair(U2L, U2R));
  } // end of recontr procedure
}

// just testing the prototype <cellFaceId,cellFaceVar>
std::map<unsigned, vecULR>
ReconstrUtil::reconstr_j12(const GlobalDiscretization *aGdiscr,
                           const dbpp::scalarField &aU) {
  using namespace std;

  aGdiscr = nullptr; // debugging  purpose, should be removed

#if 0
    vector<double> w_vU;
    w_vU.reserve( aU.values().size()+1);

    // only computational node
    aU.values().to_stdVector(w_vU);

    // boundary condition to applied at both end
    const Gamma& w_bc = aGdiscr->gamma();
    w_bc.applyBC(); // compute b.c.

    // retrieve values for both end 
    const Nodal_Value& w_bcUpstream = w_bc.getBCNodeAmont();
    const Nodal_Value& w_bcDownstream = w_bc.getBCNodeAval();

    // apply B.C. upstream
    w_vU[0] = get<0>( w_bcUpstream.Values()); // A
    w_vU[0] = get<1>( w_bcUpstream.Values()); // Q

    // need to do a push_back because the scalar field hold computational
    // node only. In the reconstruction process, in the case under study,
    // a ghost node is used (added at the end of the grid but not part of 
    // the computational domain, extrapolate)

    // apply B.C. downstream 
    w_vU.push_back( get<0>( w_bcDownstream.Values())); // A
    w_vU.push_back( get<1>( w_bcDownstream.Values())); // Q
#endif
  // only computational node
  auto w_vU = aU.values().to_stdVector();

  // compute the derivative (gradient) dU = U_i+1-U_i
  auto w_DU = computeDU(w_vU);

  // apply minmod limiter function to ...
  // first add at both ends 0. to be compare with
  w_DU.insert(w_DU.begin(), 0.);
  w_DU.push_back(0.);

  // ....
  std::vector<double> w_minDU;

  using namespace std::placeholders;
  // (just applying the minmod function to some range)
  //     std::adjacent_difference( w_DU.begin(), w_DU.end(),w_minDU.begin(),
  //       std::bind(&std::min<int>,_1,_2)); // for testing purpose

  auto w_vecIncrement = w_DU.begin();
  std::advance(w_vecIncrement, 1);

  // remove first element since not part of the computation
  std::vector<double> w_DU1(w_minDU.begin() + 1, w_minDU.end());

  // compute the variable at cell face at second-order (default U=U_dU)
  // retrieve cell face form discretization
  // const std::list<cellFace> &w_cellFaces = aGdiscr->faces();
  std::map<unsigned, vecULR> w_mapOfVarULR;
  // for (const cellFace &wCFace : w_cellFaces) {
  // compute the variable at cell face with the following algorithm
  // insert in the map
  //}

  return w_mapOfVarULR;
}
} // namespace dbpp
