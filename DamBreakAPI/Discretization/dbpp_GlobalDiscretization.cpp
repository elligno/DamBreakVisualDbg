
// stl include
#include <functional>
#include <numeric> // adjacent_difference
// boost include
#include <boost/assert.hpp>
// SfxTypes package include
#include "../SfxTypes/dbpp_ListSectFlow.h"
#include "../dbpp_Wave1DSimulator.h"
//#include "../SfxTypes/dbpp_DamBreakStepFunc.hpp"
// Discretization package
#include "dbpp_GlobalDiscretization.h"
#include "dbpp_NodalVariables.h"
// SfxTypes package
#include "../SfxTypes/dbpp_PhyConstant.h"
#include "../SfxTypes/dbpp_Simulation.h"
// utility package  includes
#include "../Utility/dbpp_TestLogger.h"
//#include "../SfxTypes/dbpp_ListSectFlow.h"

namespace dbpp {
// Callable
template <typename T, typename U = T>
struct pair_creator //: std::binary_function<T, U, std::pair<T, U>>
{
  std::pair<T, U> operator()(const T &arg1, const U &arg2) const {
    return std::make_pair(arg1, arg2);
  }
};

// Changing local variables (safest way to do it)
// Use it in init when passing ListOfSections (nbSections=100)
// but we want 101 sections for global attributes such as X, Z
// take account in the resonstruction process.
std::function<int(int)> make_offsetter(int aOffset) {
  return [=](int aInt2Add) { return aOffset + aInt2Add; };
}

GlobalDiscretization::GlobalDiscretization()
    : U1{nullptr}, U2{nullptr}, X{nullptr}, H{nullptr}, Z{nullptr},
      NbGlobalNode{101}, // Eric cMcNeil default value don't make sense!!
      dx{10.}            //  Eric cMcNeil default value??? don't get it!!
{
  // create nodal variables and initialize it
  // m_subj->attach(this);  ??? deprecated
}
GlobalDiscretization::~GlobalDiscretization() {
  // free memory when releasing
  if (U1) {
    free(U1);
  }
  if (U2) {
    free(U2);
  }
  if (X) {
    free(X);
  }
  if (H) {
    free(H);
  }
  if (Z) {
    free(Z);
  }

  if (!m_listFaces.empty()) {
    m_listFaces.clear();
  }
  if (!m_cellFacesPair.empty()) {
    m_cellFacesPair.clear();
  }
  if (!m_nval.empty()) {
    m_nval.clear();
  }
  // section flow
  // need to call the DeleteIt method
  // 		emcil::ListIterator
  // w_listIter=emcil::g_ListSectFlow->getListIterator(); 		while (
  // w_listIter.begin()!=w_listIter.end())
  // 		{
  // 			std::cout << "We are deleting Section flow\n";
  // 			emcil::SectFlow* w_sectF=w_listIter.front();
  // 			delete w_sectF;
  // 			w_sectF=nullptr;
  // 			// return iterator range (i think it's a subrange of the
  // initial range)
  // 			// i am not sure if this make sense???
  // 			w_listIter.advance_begin(1); // next in the list??
  // 		}

  // check if list of section flow was deleted
  // 		if (!m_ListSectFlow->isEmpty())
  // 		{
  // 			std::cout << "Wave1DSimulator dtor is deleting the list
  // of section flow\n"; 			m_ListSectFlow->deleteIt();
  // if( m_ListSectFlow->isEmpty())
  // 			{
  // 				std::cout << "List of section flow not empty\n";
  // 			}
  // 		}

  std::cout << "Stepping out of the Wave1DSimu7lator dtor\n";
}

#if 0  
	// initialize the field with some initial values
	// for our test, we use the dam-break problem
	void GlobalDiscretization::DamBreakInit()
	{
		using namespace std;
		using namespace std::placeholders;

		// spatial discretization
        // IMPORTANT 
		//    DiscrInfo is deprecated we will use the DamBreakData
		//    DiscrInfo use Phi0 and Phi1 values in wrong order
		//    Phi0 is downstream and phi1 is upstream. But in the 
		//    actual version it's the inverse which lead to wrong
		//    initial condition.
		DiscrInfo w_Disrc; // check default ctor, not sure of it

		// initializer (unary functor) which set initial values 
		DamBreakStepFunc w_testForeachptr(w_Disrc);

		// initialization of nodal values (H,Q)
		for_each( m_nval.begin(), m_nval.end(), w_testForeachptr);

		// section flow unit width and flat bed (then we can set A=H)
		for_each( m_nval.begin(), m_nval.end(), // range
			[](Nodal_Value& aNodeVal){aNodeVal[0]=aNodeVal[2];});
	}
#endif

void GlobalDiscretization::createNodalVal() {
  m_nval.reserve(NbGlobalNode);

  // loop on the number of sections
  for (int i = 0; i < NbGlobalNode; ++i) {
    // add nodal variable to pointer container
    if (i == 0) // tied node
    {
      // DESIGN NOTE (initialize with IC)
      // m_nval.push_back( new Nodal_Value(
      // std::make_pair(i,X[i]),std::make_tuple(U1[i],U2[i],H[i]),3)); maybe we
      // shall start at index=1
      m_nval.push_back(new Nodal_Value(std::make_pair(i, X[i]), 3));
    } else if (i == (NbGlobalNode - 1)) {
      // Set last node as ghost node (in the current version of the solver
      // we only support a single algorithm based on ghost node)
      //	m_nval[NbGlobalNode-1].setAsGhostNode();

      // DESIGN NOTE (initialize with IC)
      // m_nval.push_back( new Nodal_Value(
      // std::make_pair(i,X[i]),std::make_tuple(U1[i],U2[i],H[i]),3)); set last
      // node has tied (ghost node)
      m_nval.push_back(new Nodal_Value(std::make_pair(i, X[i]), 3));

      // just testing
      // auto w_lastNode = m_nval.back(); back return a reference, but reference
      // is stripped away
      auto &w_lastNode = m_nval.back(); // force auto to be a reference
      // check value category
      if (std::is_lvalue_reference<decltype(w_lastNode)>::value) {
        w_lastNode.setAsGhostNode();
      }
      // else {
      // auto &w_lastOne = m_nval.back();
      //}
      // std::is_lvalue_reference<decltype(w_lastNode)>
      // auto isTied = w_lastNode.isTiedNode();
    } else {
      // DESIGN NOTE (initialize with IC)
      // m_nval.push_back( new Nodal_Value(
      // std::make_pair(i,X[i]),std::make_tuple(U1[i],U2[i],H[i]),3)); not tied
      // node (interior nodes)
      m_nval.push_back(new Nodal_Value(std::make_pair(i, X[i]), 3, 0));
    }
  } // for-loop

  // sanity check (one before last)
  assert(101 == m_nval.size());

  // initialize nodal variables with dam-break initial solution
  // IMPORTANT
  //   DamBreakStepFunc use DiscrInfo which is deprecated. We will
  //   use the DamBreakData type ()
  //	DamBreakInit();

  // Design Note
  // ------------
  //   We will use this implementation for the prototyping
  //   because we want to use E. McNeil initialization code.
  //   Also, DiscrInfo is deprecated (used byDamBreakInit),

  unsigned i = 0;
  // using lambda function
  for_each(m_nval.begin(), m_nval.end(), [=, &i](Nodal_Value &aNval) {
    // set nodal value
    aNval[0] = U1[i];  // A
    aNval[1] = U2[i];  // Q
    aNval[2] = H[i++]; // H
  });
}

// TODO: add signature
void GlobalDiscretization::init(const ListSectFlow *aListSections) {

  // NO need of this!!! Only need ListOfSections and we have all we need
  // Note: actually a class attribute with same name is initialized in ctor
  // (NbSections) i don't see why we should declare locally, but this is the
  // original code
  // 		const DamBreakData w_dbData(
  // dbpp::Simulation::instance()->getActiveDiscretization()); 		if(
  // w_dbData.getDType()==DamBreakData::DiscrTypes::emcneil)
  // 		{
  //       //TODO: use a logger
  // 			std::cout << "Active discretization used by
  // GlobalDiscretization is: " << std::string("EMcNeil") << "\n";
  // 		}

  // DIM+1
  const auto NbSections = aListSections->size() + 1; // w_dbData.nbSections();

  //	Allocation de mémoire pour les variables d'état
  //	et les termes de pression/convection
  // NbSections = 101;
  U1 = (double *)malloc(NbSections * sizeof(double));
  U2 = (double *)malloc(NbSections * sizeof(double));
  //	Allocation de mémoire des paramètres du modèle hydrodynamique
  H = (double *)malloc(NbSections * sizeof(double));
  h = (double *)malloc(NbSections * sizeof(double));
  X = (double *)malloc(NbSections * sizeof(double));
  Z = (double *)malloc(NbSections * sizeof(double));
  // n = (double *) malloc( NbSections * sizeof (double));

  //	Caractérisation des paramètres pour les sections d'écoulement
  // dx = 10.;  //spatial discretization
  //	B = 1.;    section width

  // for now will do
  //     auto checkDx = aListSections->getList()[99]->X() -
  //     aListSections->getList()[98]->X(); const auto dx = checkDx;
  //     //w_dbData.dx(); assert(10. == dx); //debugging purpose

  // IMPORTANT
  // need to const cast to remove the 'const' of ListSectFlow
  // because we have 'const_iterator' which is incompatible with &X
  // not const. Also, a const_iterator can't be dereferenced.
  // Since C++14, cannot modify the object of the container
  // const_iterator return const T&.
  ListSectFlow *w_lis = const_cast<ListSectFlow *>(aListSections);
  // Initialize x-coord with section
  std::transform(w_lis->getList().begin(), w_lis->getList().end(), &X[0],
                 std::bind(&SectFlow::X, std::placeholders::_1));

  //     auto i = 0;
  //     auto begIter = aListSections->getList().begin();
  //     auto endIter = aListSections->getList().end();
  //     while( i!= aListSections->getList().size())
  //     {
  //    //   auto val = aListSections->getList()[i]->X();
  //       X[i++] = aListSections->getList()[i]->X();
  //     //  auto val1 = X[i - 1];
  //     }
  //     auto ddx = X[99] - X[98];

  // Ghost node (DIM+1)
  X[aListSections->getList().size()] =
      X[aListSections->getList().size() - 1] + dx;
  //   assert(1000.== X[100]); // sanity check

  // 		for( j = 0; j < NbSections; j++)
  // 			X[j] = dx*boost::numeric_cast<double>(j);

  // 		for (j = 0; j < NbSections; j++)
  // 			n[j] = 0.0;

  std::fill(&Z[0], &Z[0] + NbSections, 0.);
  // 		for( j = 0; j < NbSections; j++)
  // 			Z[j] = 0.0;//(1000.-X[j]) * 0.0001;

  //     std::transform( aListSections.getList().begin(),
  //     aListSections.getList().end(),
  //       &Z[0], std::bind(&SectFlow::Z, std::placeholders::_1));
  //	S0am = S0av = 0.;

  // IMPORTANT (remove the hard coded value)
  //	Spécification des conditions initiales
  //	Dans la version finale, ces conditions initiales "devraient être"
  //	spécifiées sous la forme de l'élévation de la surface libre et
  //	du débit pour chaque section
#if 0
		for (j = 0; j < NbSections; j++)
		{ 
			if (X[j] < 500.)  // shock location
				H[j] = 10.;     // upstream water level
			else
				H[j] = 1.;    // downstream water level 

			U2[j] = 0.;       // discharge (at rest)
		}
#endif
  std::fill(&U2[0], &U2[0] + NbSections, 0.); // at rest Q=0 (discharge)
  std::transform(w_lis->getList().begin(), w_lis->getList().end(), &H[0],
                 std::bind(&SectFlow::H, std::placeholders::_1));

  // DIM+1 (global spatial)
  H[aListSections->getList().size()] = H[aListSections->getList().size() - 1];

  for (j = 0; j < NbSections; j++) // wetted area
    U1[j] = HydroUtils::Evaluation_A_fonction_H(H[j], Z[j], 1. /*B*/);

#if 0
    // debugging purpose (WaveSimulator)
//     assert( m_grid->getPoint(1,48) == 470.);
//     assert( m_grid->getPoint(1,49) == 480.);
//     assert( m_grid->getPoint(1,50) == 490.);
//     assert( m_grid->getPoint(1,51) == 500.);
//     assert( w_lambda(50) == 10.); //H,x=490.
//     assert( w_lambda(51) == 1.);  //H,x=500.
    // in this case, node index offset by -1
    assert(X[47] == 470.);
    assert(X[48] == 480.);
    assert(X[49] == 490.);
    assert(X[50] == 500.);
    assert(X[99] == 990.);
    assert(H[49] == 10.); // shock location
    assert(H[50] == 1.);
#endif

  // create global variables
  createNodalVal();

  // create global faces
  createFaces();

  // create the cell (element) needed for this simulation
  createPairFaces();

  // Omega in the global discretization
  // createOmega();
}

// this is not completed, more check is required
// Design Note:
//  Create the global face (located between two points) at x_j+1/2
//  These have global identifier that correspond to the global
//  discretization. We use these face to create the domain (Omega).
//  During the simulation we may need those cell faces.
//
void GlobalDiscretization::createFaces() {
  // some check if already created
  if (!m_listFaces.empty()) {
    // shall throw an exception and not return
    // hard to debug (need to be fix)
    // shall log something in the log file or something else
    return;
  }
  // ongoing, just try to make something work and then test it!!
  for (unsigned i = 0; i < boost::numeric_cast<unsigned>(NbGlobalNode) - 1;
       ++i) {
    // add cell face with their corresponding stencil
    m_listFaces.push_back(
        cellFace(i /*cell id*/, i /*left node id*/, i + 1 /*right node id*/));
  }
  // m_cellF.push_back (new cellFace());
  auto w_msg = "Global Cell faces created";
  Logger::instance()->OutputSuccess(const_cast<char *>(w_msg));
}

// under construction
void GlobalDiscretization::createPairFaces() {
  using namespace std::placeholders;
  m_cellFacesPair.resize(NbGlobalNode -
                         2); // hard coded value, never, set to a variable

  //     cellFace w_testOp = *m_listFaces.begin();
  //     cellFace w_testOp1 = *m_listFaces.begin();
  //     w_testOp = w_testOp1;
  //     std::pair<cellFace,cellFace> w_pairF =
  //     std::make_pair(w_testOp,w_testOp1);
  //     m_cellFacesPair.push_back(std::make_pair(w_testOp,w_testOp1));
  //     *m_cellFacesPair.begin() = std::make_pair(w_testOp,w_testOp1);
  //     std::list<cellFace>::iterator beg = m_listFaces.begin();
  //     std::list<cellFace>::iterator end = m_listFaces.end();
  //     std::list<cellFace>::iterator beg1 = m_listFaces.begin();
  //     std::advance(beg1,1);
  //     std::advance(end,-1);

  using namespace std;
  using namespace boost;
  using namespace std::placeholders;
  typedef std::list<cellFace>::iterator list_iter;        // list iterator
  typedef iterator_range<list_iter> iter_cellface;        // iterator list range
  typedef range_iterator<iter_cellface>::type range_iter; // range iterator type
  typedef range_difference<iter_cellface>::type diff_type; // range difference

  // Creating 2 range with offset towards right and left (at both end)
  // sub range of the whole range (std::list<cellFace>)
  // [begin,end-1[
  iter_cellface w_rngFace = make_iterator_range(m_listFaces).advance_end(-1);
  // [begin+1,end[
  iter_cellface w_rngFacep1 = make_iterator_range(m_listFaces).advance_begin(1);

  // check equality of both range size (use boost range algorithm)
  diff_type w_diff1 = distance(w_rngFace);
  diff_type w_diff2 = distance(w_rngFacep1);
  assert(w_diff1 == w_diff2);

  // NOTE
  //  we create only 99 pair because ... first node i=0 is tied node
  // but we should push it at the end
  // creating pair of cell face by using transform algorithm with lambda
  // expression (C++11)
  transform(
      w_rngFace.begin(), w_rngFace.end(), w_rngFacep1.begin(), // range of faces
      m_cellFacesPair.begin(),                           // store pair of faces
      [](const cellFace &aFace1, const cellFace &aFace2) // lambda expression
      { return std::make_pair(aFace1, aFace2); });

  // Temporary fix for now since we assume that the first right end node (index
  // = 0) is tied by b.c.
  m_cellFacesPair.push_front(
      std::make_pair(cellFace{0, 0, 0, false, true}, cellFace{0, 0, 1}));

#if 0
    // would it work? first element will be the first face, then all the rest 
    // will pair faces () i do not think it will work, first element?? not a pair 
    std::adjacent_difference( w_rngFace.begin(), w_rngFace.end(), // range of faces
      m_cellFacesPair.begin(), // store pair of faces);
      [](const cellFace& aFace1, const cellFace& aFace2) // lambda expression
    {
      return std::make_pair(aFace1, aFace2);
    });

    // another way of making pair by using bind adaptor 
    std::transform( w_rngFace.begin(), w_rngFace.end(), w_rngFacep1.begin(), // range of faces 
       m_cellFacesPair.begin(), // store pair of faces
      std::bind(pair_creator<cellFace, cellFace>{}, _1, _2)); // binder
#endif

  auto w_msg = "Pair of cell faces created";
  Logger::instance()->OutputSuccess(const_cast<char *>(w_msg));
}

void GlobalDiscretization::update() {
  using namespace std;
  using namespace std::placeholders;
  using namespace dbpp;

#if 0
		// retrieve the final state (i ma not sure of the size)
		std::vector<double> w_U1vec; w_U1vec.reserve( m_nval.size());
		std::vector<double> w_U2vec; w_U2vec.reserve( m_nval.size());

		// assume that m_numrep has been initialized in the EMcNeil1D ctor
		// with the meyhod num_rep() ...
		// computational nodes which is 1,...,100 in field lattice
		// correspond to 0,...,99 in std::vector
		m_numRep->getFinalState( w_U1vec, w_U2vec);

		// sanity check
		if( w_U1vec.empty())
		{
			std::cerr << "GDiscr::update() empty U1 state\n";
			dbpp::Logger::instance()->OutputError("GDiscr::update() empty U1 state\n");
		}
		if( w_U2vec.empty())
		{
			std::cerr << "GDiscr::update() empty U2 state\n";
			dbpp::Logger::instance()->OutputError("GDiscr::update() empty U2 state\n");
		}
#endif

  // Don't really need to start from 0, ...
  // because 0 and N (boundary node has been set by boundary condition)
  // when you call Gamma::applyBC() these 2 values are set
  // we need also to update the U1/U2 variables
#if 1 // next version

  const auto w_stateVec = m_numRep->getState();
  auto w_vec1 = w_stateVec.first->values().to_stdVector();
  auto w_boostIter = boost::make_iterator_range(w_vec1.begin(), w_vec1.end());
  auto w_vec2 = w_stateVec.second->values().to_stdVector();
  std::copy(w_boostIter.begin(), w_boostIter.end(), &U1[0]);
  auto w_boostIter2 = boost::make_iterator_range(w_vec2.begin(), w_vec2.end());
  std::copy(w_boostIter2.begin(), w_boostIter2.end(), &U2[0]);
#endif

#if 0
    // Don't need that stuff
		unsigned ii=0;
		unsigned jj=0;
		for( double w_val1 : w_U1vec) //A-variable
		{
			U1[ii++]=w_val1;
		}
		for( double w_val2 : w_U2vec) //Q-variable
		{
			U2[jj++]=w_val2;
		}
#endif

  // set H water level (using utility hydro stuff)
  constexpr auto B = 1.; // section width (temporary fix)
  // H=A-Z (water depth) flat bed
  std::transform(&U1[0], &U1[0] + NbGlobalNode, &Z[0], &H[0],
                 std::bind(&HydroUtils::Evaluation_H_fonction_A, _1, B, _2));

  // updating nodal values at grid node
  unsigned i = 0;
  // retrieve A/Q values from nodal_value
  auto w_nvalBeg = m_nval.begin();
  auto w_nvalEnd = m_nval.end();
  while (w_nvalBeg != w_nvalEnd) {
    Nodal_Value &w_nval = m_nval[i];
    w_nval[0] = U1[i];  // A
    w_nval[1] = U2[i];  // Q
    w_nval[2] = H[i++]; // H
    w_nvalBeg++;
  }
}

void GlobalDiscretization::createOmega() { throw "Not implemented yet\n"; }
} // namespace dbpp
