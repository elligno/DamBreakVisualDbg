
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
#include "../SfxTypes/dbpp_PhyConstant.h"
#include "../Utility/dbpp_EMcNeilUtils.h"
#include "../Utility/dbpp_TestLogger.h"
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

  // apply b.c. (add ghost node)
  //  const auto& w_bc = GlobalDiscretization::instance()->gamma();
  //  using nval = GlobalDiscretization::NodalValComp;
  //  auto w_Uaval = std::get<nval::A>(w_bc.values());

  // ...
  vector<real> w_U(aField.values().cbegin(), aField.values().cend());

  // sanity check just to  make sure  that we added the extra value (ghost)
  assert(aField.values().size() + 1 ==
         static_cast<int>(w_U.size())); // asset will work on debug

  vector<real> w_dU;
  //		vector<real> w_dU2;
  //	Calculs préalables: évaluation des éléments du vecteur dU
  w_dU[0] = HydroUtils::minmod(w_U[1] - w_U[0], 0.);

  // w_dU2[0] = HydroUtils::minmod( aU2[1]-aU2[0], 0.);
  // change loop index because out-of-range error
  // i=1, i<NbSections-1 compare to Eric McNeil code ()
  // and added the boundary cnd. for the last node (ghost node i=100)
  for (auto i = 1; i < NbSections - 1; i++) // last element index=100
  {                                         // i+1 -> 101 (doesn't exist)
    w_dU[i] = HydroUtils::minmod(w_U[i + 1] - w_U[i], w_U[i] - w_U[i - 1]);
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

  // ******************algorithm steps*****************
  // compute dU std::adjacent_difference(U)
  // force first element to 0 (yhe algorithm leave first element unchange)
  // push 0. (last element) compare last dU[100]
  // compute minmod: std::adjacent_difference(..., minmod) -> dU
  // popup first element (begin()+1)
  // cell face variables at j+1/2 (@second-order) left/right state (UL/UR)
  // for_each(dU.begin(),dU.end(),multiply{0.5})
  // transform(begin(),prev(end,1),dU,UL,plus});
  // transform(next(begin(),1),end,std::next(dU,1),UR,minus});
  // create pair cell face variables
  //  for(i,...,global cell faces)
  //   pair(UL[i],UR[i])

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

  // just a test
  valarray<real> w_Ui(
      vector<real>(aField.values().to_stdVector().cbegin(),
                   std::prev(aField.values().to_stdVector().cend(), 1))
          .data(),
      aField.values().size());

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

// under construction (need to be tested)
ReconstrUtil::mapofacesvar
ReconstrUtil::reconstr_j12(const GlobalDiscretization *aGdiscr,
                           const StateVector &aStateVector) {

  using namespace std;

  if (aStateVector.first) {
    assert(aStateVector.first == aStateVector.second);
    assert(EMCNEILNbSections::value ==
           aStateVector.first->values().size()); // hard coded
  }

  // avoid calling cbegin()/cend() on temporary prvalue (vector range ctor)
  //  auto Aiter = aStateVector.first->values().to_stdVector();
  //  auto Qiter = aStateVector.second->values().to_stdVector();
  // range ctor
  //  std::vector<double> w_Avec(Aiter.cbegin(), Aiter.cend());
  //  std::vector<double> w_Qvec(Qiter.cbegin(), Qiter.cend());

  // prvalue (pure reading value: temporary) move ctor
  vector w_Avec(aStateVector.first->values().to_stdVector());
  vector w_Qvec(aStateVector.second->values().to_stdVector());

  if (aGdiscr) {
    const auto &w_bc = aGdiscr->gamma();
    auto [Aamont, Qamont, Hamont] = w_bc.getBCNodeAmont().Values();
    w_Avec[0] = Aamont;
    w_Qvec[0] = Qamont;

    auto [Aaval, Qaval, Haval] = w_bc.getBCNodeAval().Values();
    w_Avec.push_back(Aaval);
    w_Qvec.push_back(Qaval);
  } else {
    exit(EXIT_FAILURE); // maybe throw an exception!!!
  }

  vector<double> w_dAvec;
  w_dAvec.reserve(EMCNEILNbSections::value);
  vector<double> w_dQvec;
  w_dQvec.reserve(EMCNEILNbSections::value);
  // compute gradient first-order
  adjacent_difference(w_Avec.cbegin(), w_Avec.cend(), w_dAvec.begin());
  adjacent_difference(w_Qvec.cbegin(), w_Qvec.cend(), w_dQvec.begin());
  // apply minmod slope limiter
  w_dAvec[0] = 0.;
  w_dAvec.push_back(0.); // add it for comparison of last element
  w_dQvec[0] = 0.;
  w_dQvec.push_back(0.); // ditto
  adjacent_difference(w_dAvec.cbegin(), w_dAvec.cend(), w_dAvec.begin(),
                      HydroUtils::minmod);
  adjacent_difference(w_dQvec.cbegin(), w_dQvec.cend(), w_dQvec.begin(),
                      HydroUtils::minmod);
  assert(EMCNEILNbSections::value + 1 == w_dAvec.size());

  // ***************left state*******************

  // popup first element // [0,...,99] [0,..,100[
  valarray<double> w_dU1(
      vector(next(w_dAvec.cbegin(), 1), std::prev(w_dAvec.cend(), 1)).data(),
      w_dAvec.size() - 1);
  assert(EMCNEILNbSections::value - 1 == w_dU1.size());
  valarray<double> w_dU2(
      vector(next(w_dQvec.cbegin(), 1), std::prev(w_dQvec.cend(), 1)).data(),
      w_dQvec.size() - 1);
  assert(EMCNEILNbSections::value - 1 == w_dU2.size());

  valarray<double> w_U1(
      vector(w_Avec.cbegin(), std::prev(w_Avec.cend(), 1)).data(),
      w_Avec.size());
  valarray<double> w_U2(
      vector(w_Qvec.cbegin(), std::prev(w_Qvec.cend(), 1)).data(),
      w_Avec.size());
  assert(w_U1.size() == w_dU1.size()); // debugging purpose
  assert(w_U2.size() == w_dU2.size()); // ditto

  auto w_U1L = w_U1 + 0.5 * w_dU1; // A at j+1/2
  auto w_U2L = w_U2 + 0.5 * w_dU2; // Q at j+1/2

  // ***************right state*******************
  // popup first element and go next // [1,...,100[ ]0,...,100]
  valarray<double> w_dU1r(
      vector(next(w_dAvec.cbegin(), 2), w_dAvec.cend()).data(),
      w_dAvec.size() - 1);
  assert(EMCNEILNbSections::value - 1 == w_dU1.size());
  valarray<double> w_dU2r(
      vector(next(w_dQvec.cbegin(), 2), w_dQvec.cend()).data(),
      w_dQvec.size() - 1);
  assert(EMCNEILNbSections::value - 1 == w_dU2.size());
  valarray<double> w_U1r(
      vector(std::next(w_Avec.cbegin(), 1), w_Avec.cend()).data(),
      w_Avec.size());
  valarray<double> w_U2r(
      vector(std::next(w_Qvec.cbegin(), 1), w_Qvec.cend()).data(),
      w_Avec.size());
  assert(w_U1r.size() == w_dU1r.size()); // debugging purpose
  assert(w_U2r.size() == w_dU2r.size()); // ditto
  auto w_U1R = w_U1r - 0.5 * w_dU1r;     // A at j+1/2
  auto w_U2R = w_U2r - 0.5 * w_dU2r;     // Q at j+1/2

  // loop to fill map of cell face variables
  mapofacesvar w_cellFaceVar;
  for (auto i = 1; i < EMCNEILNbSections::value; ++i) {
    cellFaceVariables w_U1faceVar{w_U1L[i - 1], w_U1R[i - 1],
                                  static_cast<unsigned>(i)};
    cellFaceVariables w_U2faceVar{w_U2L[i - 1], w_U2R[i - 1],
                                  static_cast<unsigned>(i)};

    if (auto [pos, succeed] = w_cellFaceVar.insert(
            {static_cast<unsigned>(i), {w_U1faceVar, w_U2faceVar}});
        !succeed) {
      std::cerr << "Couldn't insert cell face variablles into map\n";
      Logger::instance()->OutputError(
          std::string{"We set DamBreak step function"}.data());
      // auto checkPos=*pos; iterator that hold position
      // in this case it should be equal at end
    } // if
  }   // for-loop

  // ******************algorithm steps*****************
  // compute dU std::adjacent_difference(U)
  // force first element to 0 (the algorithm leave first element unchange)
  // push 0. (last element) compare last dU[100]
  // compute minmod: std::adjacent_difference(..., minmod) -> dU
  // popup first element (begin()+1)
  // cell face variables at j+1/2 (@second-order) left/right state (UL/UR)
  // for_each(dU.begin(),dU.end(),multiply{0.5})
  // transform(begin(),prev(end,1),dU,UL,plus});
  // transform(next(begin(),1),end,std::next(dU,1),UR,minus});
  // create pair cell face variables
  //  for(i,...,global cell faces)
  //   pair(UL[i],UR[i])

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

  return w_cellFaceVar;
}
} // namespace dbpp
