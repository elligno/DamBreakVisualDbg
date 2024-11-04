#pragma once

// stl include
#include <list>
// boost includes
#include <boost/iterator/transform_iterator.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/range/iterator_range.hpp>
// Discretization package includes
#include "../Discretization/dbpp_EMcnilGamma.h"
#include "../Discretization/dbpp_NodalVariables.h"
// Utility package includes
//#include "../Utility/dbpp_SomeUtilities.hpp"
#include "../Utility/dbpp_EMcNeilUtils.h"
// SfxTypes package
#include "../SfxTypes/dbpp_Singleton.hpp"
#include "../SfxTypes/dbpp_cellFace.h"

namespace dbpp {
// forward declaration
class EMcNeil1D;
class SemiDiscreteModel;

/** Brief Object-Oriented Design and Analysis of Finite Volume Method.
 *
 * PHYSICAL ENTITIES include nodes and elements
 * Node <---- Omega_eh; i.e., element depends on node
 * Omega_h = { all Node, all Omega_eh }; global discretized domain is defined as
 * a set of all nodes and all elements.
 * A finite volume is defined as a set of {Omega_eh, u_eh, N_e}
 *
 * Design Note
 * -----------
 *  Experimenting the concept of global discretization
 *  that carry information about nodal value, boundary
 *  condition and cell faces (for now). It is a Singleton
 *  design pattern (one instance per application). Actually
 *  it follow Observer Design pattern (strong relationship)
 *  between the numerical representation or method and the
 *  global discretization.
 *
 *  First select and preserve the relation: Omega_h <---- u_h,
 *  that is the declaration of u_h will have to refer to Omega_h as
 *  > Omega_h oh;
 *  > U_h uh(oh);
 *
 *  Global_Discretization = { Omega_h, U_h, gh_on_Gamma_h} is a
 *    strong component design class
 *            Global_Discretization = { Omega_h, u_h}
 */
class GlobalDiscretization : // global instance
                             public Singleton<GlobalDiscretization> {
public:
  //		typedef boost::iterator_range<std::list<double>::iterator>
  // rngA_iter;
  // 		typedef
  // boost::iterator_range<std::forward_list<double>::const_iterator> rngQ_iter;
  // 		typedef
  // boost::iterator_range<std::forward_list<double>::const_iterator> rngH_iter;

  /** Component of the nodal variable
   */
  enum class NodalValComp {
    A = 0, /**< double value*/
    Q = 1, /**< double value*/
    H = 2  /**< double value*/
  };

  /** initialize the global discretization*/
  void init(const ListSectFlow *aListSections);

  // testing some concept of global discretization (prototype)
  /** Read-only instance of the numerical scheme.
   *
   * @return instance of the numerical scheme
   */
  EMcNeil1D *num_rep() const { return m_numRep; }
  /** Read/write instance of the numerical scheme.
   *
   * @result instance of the numerical scheme
   */
  EMcNeil1D *&num_rep() { return m_numRep; }

  /** just testing (for now NbSections=101)
   *
   */
  const boost::ptr_vector<Nodal_Value> &Uh() const { return m_nval; }
  boost::ptr_vector<Nodal_Value> &Uh() { return m_nval; }

  // this is again some test about the interface
  // supported by this concept (it's all temporary stuff)
  /** Read instance of the numerical scheme.
   *
   * @result vector of
   */
  std::vector<double> Xcoord() {
    return std::vector<double>(&X[0], &X[0] + NbGlobalNode);
  }
  std::vector<double> Zvalues() {
    return std::vector<double>(&Z[0], &Z[0] + NbGlobalNode);
  }
  std::vector<double> U1values() {
    return std::vector<double>(&U1[0], &U1[0] + NbGlobalNode);
  }
  std::vector<double> U2values() {
    return std::vector<double>(&U2[0], &U2[0] + NbGlobalNode);
  }
  std::vector<double> Hvalues() {
    return std::vector<double>(&H[0], &H[0] + NbGlobalNode);
  }

  // Design Note
  // -----------
  //  i don't see the point why we should return a reference
  //  to a pointer of container, since these faces are created
  //  in the initialization process of the discretization, it will
  //  remains static for the whole simulation. Why don't we
  //  return a copy of the list?
  //  Again the question: how it will be used by client of this
  //  class? want to traverse the list from beginning to the end?
  //  want to access to faces randomly?
  //  For now the following signature is more appropriate fill the
  //  requirement above.
  //  std::list<cellFace> getFaces() const { return m_listFaces;}

  // NbSections-1 in the original code (deprecated)
  // 		const boost::ptr_vector<cellFace>& faces() const { return
  // m_listFaces;} 		      boost::ptr_vector<cellFace>& faces() {
  // return m_listFaces;}

  const std::list<cellFace> &faces() const { return m_listFaces; }
  std::list<cellFace> &faces() { return m_listFaces; }

  // Finite volume formulation (list of all cells)
  // Omega&  omega() {/*not implemented yet*/}

  /** boundary condition in use */
  Gamma &gamma() { return m_gamma; }
  const Gamma &gamma() const { return m_gamma; }

  /**
   *@param returned vector
   *@param component of nodal variable
   */
  void to_stdVector(std::vector<double> &aVec2Fill,
                    NodalValComp aNvalComp = NodalValComp::A) const {
    using namespace boost;

    // replacing the functor declaration in SomeUtilities.h
    std::function<double(const Nodal_Value &)> w_getA =
        [](const Nodal_Value &aVal) -> double { return aVal[0]; };
    std::function<double(const Nodal_Value &)> w_getQ =
        [](const Nodal_Value &aVal) -> double { return aVal[1]; };
    std::function<double(const Nodal_Value &)> w_getH =
        [](const Nodal_Value &aVal) -> double { return aVal[2]; };

    switch (aNvalComp) {
    case NodalValComp::A: {
      // using delegate function
      aVec2Fill.assign(make_transform_iterator(m_nval.begin(), w_getA),
                       make_transform_iterator(m_nval.end(), w_getA));
      break;
    }
    case NodalValComp::Q: {
      aVec2Fill.assign(make_transform_iterator(m_nval.begin(), w_getQ),
                       make_transform_iterator(m_nval.end(), w_getQ));
      break;
    }
    case NodalValComp::H: {
      aVec2Fill.assign(make_transform_iterator(m_nval.begin(), w_getH),
                       make_transform_iterator(m_nval.end(), w_getH));
      break;
    }
    default:
      // log something in the log file
      std::cerr << "Cannot return an invalid type in GlobalDiscretization\n";
      break;
    }
  }

  /** retrieve state form EMcNeil1D numerical rep and update
      Observer Design Pattern*/
  void update();

  /**
   *@return some attributes of the discretization
   */
  int getNbSections() const { return NbGlobalNode; }

  /** Base class need to be a friend to access the protected
   * member of the derived class.
   */
  friend class Singleton<GlobalDiscretization>;

protected:
  /** default ctor (create the global variables and ...)
      for now is just a vector of nodal variables*/
  GlobalDiscretization();
  ~GlobalDiscretization();

private:
  double *U1, *U2, *X, *H, *h; /**<section flow properties */
  double *Z /*, *n*/;          /**<bathymetry */
  int j, NbGlobalNode;         /**< number of global node*/
  double B, dx;                /**< section width*/
  double m_Phi1;               /**< water level at upstream*/
  double m_Phi0;               /**< water level at downstream*/
  double m_xmin;               /**< minimum x-coordinate*/
  double m_xmax;               /**< maximum x-coordinate */
  double m_shockLocation;      /**< dambreak shock location*/

  boost::ptr_vector<Nodal_Value> m_nval;
  /**< nodal variables (A,Q,H)*/

  std::list<cellFace> m_listFaces;
  /**<cell faces j+1/2*/

  std::list<std::pair<cellFace, cellFace>> m_cellFacesPair;
  /**< pair of faces with global identifier*/

  /**< Physical boundary condition*/
  Gamma m_gamma;

  /**< Numerical representation of math equations*/
  EMcNeil1D *m_numRep;

  // initialize the field with some initial values
  // for our test, we use the dam-break problem
  void DamBreakInit();

  /** create nodal variable (deprecated)*/
  void createNodalVal();

  /** create cell faces (global)*/
  void createFaces();

  /** create pair of faces (cell for Omega)*/
  void createPairFaces();

  /** cell for Omega*/
  void createOmega();
};
} // namespace dbpp
