
#pragma once

// Packages includes
#include "../Algorithm/dbpp_IFluxAlgoImpl.h"
#include "../Discretization/dbpp_EmcilNumTreatment.h"
#include "../Discretization/dbpp_GlobalDiscretization.h"
#include "../MathModel/dbpp_MathEquations.h"
#include "dbpp_HLL_RK2_Algo_Vec.h"
// temp for dev debug
#include "../SfxTypes/dbpp_ListSectFlow.h"
#include "../SfxTypes/dbpp_Simulation.h"
// boost include
#include <boost/operators.hpp>

// forward declarations
namespace dbpp {
class MUSCLReconsrtr;
class EMcNeil1D;
} // namespace dbpp
namespace dbpp {
class IRhsDiscretization;
}

namespace dbpp {
// Question: since i am making this class noncopyable
// if i want to create a container of pointer of base
// class, can i still insert this class (container make copy)?
// this class cannot be overriden by subclass
class TestBcSectF final : public EMcNeil1D // base class
{
public:
  // default ctor
  TestBcSectF();
  // another ctor i am not sure aout this one???
  TestBcSectF(const GlobalDiscretization &aGdiscr);
  // not allowed to copy or assign construction
  TestBcSectF(const TestBcSectF &) = delete;
  TestBcSectF &operator=(const TestBcSectF &) = delete;
  // dtor
  ~TestBcSectF();

protected:
  // set of services that can be customized for user specific
  // time stepping
  void timeStep() override final;

  // set all parameters for the from base class
  // Usage
  void setInitSln(const StateVector &aU,
                  ListSectFlow *aListofSect) override final;

protected:
  // boundary condition
  // 		virtual void setAmont();
  // 		virtual void setAval();

  // ... to be completed ???
  friend class TwoStepsScehmeIntegrator;

private:
  //
  // Explicit Integrator
  //
  class TwoStepsScehmeIntegrator {
    using solnU12 = std::pair<std::vector<double>, std::vector<double>>;

  public:
    TwoStepsScehmeIntegrator() = default;
    TwoStepsScehmeIntegrator(IRhsDiscretization *aRhsdiscr)
        : m_Rhsdiscr(aRhsdiscr) { /*log message???*/
    }
    TwoStepsScehmeIntegrator(const TwoStepsScehmeIntegrator &) = delete;
    TwoStepsScehmeIntegrator &
    operator=(const TwoStepsScehmeIntegrator &) = delete;

    void setInitSln(const std::vector<double> &aU1,
                    const std::vector<double> &aU2) {
      // design note: add a check on the size of both vector, must be equal
      U1.reserve(aU1.size());
      U2.reserve(aU2.size());

      std::copy(aU1.cbegin(), aU1.cend(), U1.begin());
      std::copy(aU2.cbegin(), aU2.cend(), U2.begin());
    }
    void setInitSln(const double *aU1, const double *aU2,
                    const unsigned aSize) {
      // design note: add a check on the size of both vector, must be equal
      U1.reserve(aSize);
      U2.reserve(aSize);

      // design note: check pointer null value (try-catch)
      std::copy(&aU1[0], &aU1[0] + aSize, U1.begin());
      std::copy(&aU2[0], &aU2[0] + aSize, U2.begin());
    }

    // design note: what about move semantic?? in some situation ... to be
    // completed
    void setInitSln(std::vector<double> &&aU1, std::vector<double> &&aU2) {
      // not sure how to do it??? can i do it??
      U1 = aU1; // use move version of operator=??
      U2 = aU2; // use move version of operator=??
    }

    // ...
    void predictor(const std::vector<double> &avH);
    void corrector(const std::vector<double> &avH) {
      predictor(avH); // debugging purpose, should be removed
      // not implemented yet
      //			w_pom->CalculFF(m_FF1, m_FF2, m_U1p, m_U2p,
      // w_dU1, w_dU2, NbSections, w_Section0->B());
      //			w_pom->TraitementTermeSource2(m_S, m_U2p, m_U1p,
      // w_vH, w_n, dx, NbSections, w_Section0->B());
    }
    const solnU12 &getSolutionU12() const { return m_soln; }

  private:
    solnU12 m_soln;                 //**< */
    std::vector<double> U1;         //**< initial cnd*/
    std::vector<double> U2;         //**< */
    std::vector<double> m_vH;       //**< water level*/
    std::vector<double> m_U1p;      //**< imtermediate state */
    std::vector<double> m_U2p;      //**< */
    IRhsDiscretization *m_Rhsdiscr; //**< supported POM discretization*/
  };

  //		unsigned m_NbSections;
  // copy and assignment ctor are declared private
  // since we inherit from boost::noncopyable
  // 		std::vector<double> m_U1p; // intermediate state in our
  // two-steps
  // time stepping 		std::vector<double> m_U2p;
  // std::vector<double> m_FF1; // numerical flux at cell face
  // std::vector<double> m_FF2; std::vector<double> m_S;   // source terms:
  // friction and bottom

  // again testing some new implementation
  std::shared_ptr<dbpp::scalarField> m_U12;
  std::shared_ptr<dbpp::scalarField> m_U12p;

  // some helpers functions
  void predictor();
  void corrector();

  // it sure is!
  std::unique_ptr<BaseNumTreatmemt> createBaseNumTreatment();

  // List of sections flow in use
  ListSectFlow *m_listSectFlow;

private:
  //
  // Some experimental stuff (not to be used outside of this class)
  //

  //  class CellFaceFluxSolver {
  // public:
  // kind of Observer
  //   CellFaceFluxSolver(const EMcNeil1D &aNumRep)
  //       : m_numRep(nullptr /*aNumRep*/) {}

  // Usage
  //   should be at the algorithm level
  //   at some point we need to set boundary condition (global faces)
  //   const std::list<cellFace>& w_listCellFaces = Gdiscr->faces();
  //   CellFaceFluxSolver mySolver(EMcNeil1D);
  //
  // sanity check
  // 			if( !w_cellFacs.empty())
  // 			{
  // 				// logging information for check later
  // 				dbapp::Logger::instance()->OutputSuccess("Couldn't
  // create cell faces");
  //
  // 				// check for number of faces
  // loop on each cell faces
  // for (const cellFace& var : w_listCellFaces)
  // std::map<unsigned, FluxTensor>  mySolver.calculFF(var)
  // 			}

  // compute flux at cell face according to HLL algorithm
  // Design Note
  //   separation concern
  //   MUSCLReconstr(GDisrc) dependency about the global discr.
  //   when reconstruction is done its about the scalar field at cell face
  //   cell is around some node and values
  //   m_musclReconstr.reconstr(vectorField) similar to Meneguzzi code
  //   return pair of state variable at cell face (UL,UR)
  //   could be calculFF( const cellFace& aCellFace, MUSCLReconstr& aReconstr)
  // this way no need to pass EMcNeil1D in ctor which make no sense
  // but make reconstruction depends on global discr.
  // when computing flux at cell face, we need face and how to reconstr
  // Reminder: calculFF is responsible to calculate flux at cell face
  // Another approach could be to pass reconstr in ctor
  // flux solver may use a reconstruction procedure to solve flux at cell face
  // CellFaceFluxSolver(MUSCLReconsrt& aReconstr)
  // which seems more natural
  // getDU(cellFaceIdx) -> pair of DU: (DUL,DUR)
  // then ready to compute UL12=U ... UR12=...
  // Next compute FL,FR with these values
  // createSCLEquations()
  // flux(U12) ?? not sure but something like this
  // RiemannSolver
  //  solve(RiemannProb aRprob) at cell face i+1/2(FL,FR)
  // return pair of FL,FR
  // This algorithm
  //   std::pair<double, double> calculFF(const cellFace &aCellFace) {
  // retrieve current state
  //    StateVector w_currState = m_numRep->getState();

  // could be replaced by line below, reconstr has values since
  // it computes dU gradient (or something like this)
  //	m_reconstr->getFieldValues();

  // but it has also a dependency on global discretization
  //	m_reconstr->golobalDiscr();

  // retrieve neighbors nodes i,i-1 (left/right)
  // we need to reconstruct our state variables at cell face
  // ULi12 = U... DU left state
  // URi12 = U... DU right state
  // now we need to compute algorithm parameters
  // finally according to shock wave speed evaluate flux at cell face
  // then return as a pair of component
  // return std::make_pair();
  //  }

  // private:
  // just testing some ref initializer
  // const EMcNeil1D& m_numRep;
  //  EMcNeil1D *m_numRep;
  //  GlobalDiscretization *m_Gdiscr; //??
  //  MUSCLReconsrtr *m_reconstr;
#if 0
  public:
    //
    // cellFaceVariables
    //

    // (U1L,U2L) + (U1R, U2R)
    class cellFaceVariables
        : public boost::equality_comparable<
              cellFaceVariables> // strickly comparable
                                 //                     boost::less_than_comparable<cellFace>
                                 //                     // less than comparable
    {
    public:
      using cellFaceVar = std::pair<double /*U1*/, double /*U2*/>;

    public:
      explicit cellFaceVariables(unsigned aCellIdx) : m_cellIdx(aCellIdx) {}
      // copy and assignment default ctor fine

      // cellFaceVariables(const cellFaceVariables& aOther);
      // assignment ctor
      // 				cellFaceVariables& operator= (const
      // cellFaceVariables& aOther)
      // 				{
      // 					return *this; // debugging
      // purpose
      // 				}

      // i am not sure about move semantic what about default move ctor???
      // default are probably fine, no need to re-define, not sure???
      //    cellFaceVariables(cellFaceVariables &&aOther);
      //    cellFaceVariables &operator=(cellFaceVariables &&aOther) {
      // check for self-assignment
      //    return *this; // debugging purpose
      //  }

      // equality comparable operations
      // comparison operator (boost operator equality_comparable)
      // operator != is defined
      //    friend bool operator==(const cellFaceVariables &aFace1,
      //                           const cellFaceVariables &aFace2) {
      //     return false; // debugging purpose
      //   }

      // U1L,U1R
      cellFaceVar first() const {
        return std::make_pair(m_leftState.first, m_rightState.first);
      }
      // U2L,U2R
      cellFaceVar second() const {
        return std::make_pair(m_leftState.second, m_rightState.second);
      }

      // return as default for our purpose
      unsigned getOrderReconstr() const { return 2; }

      // getter/setter
      cellFaceVar leftState() const { return m_leftState; }
      cellFaceVar &leftState() { return m_leftState; }
      cellFaceVar rightState() const { return m_rightState; }
      cellFaceVar &rightState() { return m_rightState; }
      // cell face global index
      unsigned cellFaceGblIdx() const { return m_cellIdx; }
      void setCellGblIdx(unsigned aCellIdx) { m_cellIdx = aCellIdx; }

    private:
      cellFaceVar m_leftState;  // pair=U1L,U2L
      cellFaceVar m_rightState; // pair=U1R,U2R
      unsigned m_cellIdx;
    };
#endif // 0
private:
  //
  // HLLParams
  //

  typedef struct PairLeftRight {
    PairLeftRight() : m_leftVal(0.), m_rightVal(0.) {}
    PairLeftRight(double aLeftVal, double aRightVal)
        : m_leftVal(aLeftVal), m_rightVal(aRightVal) {}

    std::pair<double, double> asStdPair() const {
      return std::make_pair(m_leftVal, m_rightVal);
    }

    double leftVal() const { return m_leftVal; }
    double rightVal() const { return m_rightVal; }

    double m_leftVal;
    double m_rightVal;
  } PairLeftRight;

  //    typedef struct HLLParams {

  //      // typedef std::pair<double/*UL*/,double/*UR*/> cellFacesVar;

  //      HLLParams(const EMcNeil1D *aNumrep)
  //          : uR(0.), uL(0.), SR(0.), SL(0.), CR(0.), CL(0.), uS(0.), CS(0.),
  //            FL(0.), FR(0.) {
  //        // logger
  //      }

  // ...
  // 				uL = UL2/UL1;
  // 				uR = UR2/UR1;
  //  PairLeftRight getmuLR(const cellFaceVariables &aULRVar) {
  //     typedef cellFaceVariables::cellFaceVar faceVar;

  //      const faceVar w_leftState = aULRVar.leftState();
  //    const faceVar w_rightState = aULRVar.rightState();

  //      uL = w_leftState.second / w_leftState.first;   // UL2/UL1;
  //      uR = w_rightState.second / w_rightState.first; // UR2/UR1;

  //    return PairLeftRight(uL, uR);
  //   }

  //      // Shock wave SL,SR () needed to compute flux according to HLL
  //      algorithm PairLeftRight getSLR(const cellFaceVariables &aULR) {
  //        // mu and CLR computation
  //        auto w_muLR = getmuLR(aULR);
  //   //     PairLeftRight w_cLR = getCLR(aULR);
  //   //     auto w_muCS = 0.; // getmuCS(aULR);

  //        // testing the standard function
  //        // SL = std::min( uL - CL, uS - CS);
  //        SR = std::max(uR + CR, uS + CS);
  //        SL = 0.; // std::min(w_muLR.leftVal()-w_cLR.leftVal(),
  //                 //       w_muCS.first - w_muCS.second);

  //        return PairLeftRight(SL, SR);
  //      }

  //      // celerity speed at shock face
  //      // 	CR = ::sqrt( grav*UR1/HydroUtils::T(B));
  //      // 	CL = ::sqrt( grav*UL1/HydroUtils::T(B));
  //   //   PairLeftRight getCLR(const cellFaceVariables &aULR) {
  //        //	TestBcSectF* w_class = static_cast<TestBcSectF*>(m_numRep);
  //        // 					if(
  //        // w_class->isSectionUnitWidth())
  //        // 					{
  //        // B=1. unit width
  //        // 						CR = ::sqrt(
  //        // PhysicalConstant::sGravity*m_U1LR.second); debugging purpose
  //        // CL =
  //        // ::sqrt( PhysicalConstant::sGravity*m_U1LR.first);

  //        //				  return PairLeftRight(CL,CR);
  //        //				}
  //        //				return PairLeftRight();
  //        //			}

  //        //	Une attention particulière devra être ici portée sur
  //        //	les signes (+/-) relativement au produit scalaire avec
  //        // 				//	Si+1/2
  //        // 				std::pair<double/*L*/,double/*R*/>
  //        // getmuCS(const cellFaceVariables& aULR)
  //        // 				{
  //        // 					//	Calcul de SR et SL
  //        (shock speed
  //        // at
  //        // cell face) 					auto w_muLT =
  //        getmuLR(aULR);
  //        // auto w_cLR = getCLR(aULR);
  //        //
  //        // 					//	Étape intermédiaire:
  //        calcul de US
  //        // et
  //        // CS 					CS = 0.5*(CL + CR) -
  //        0.25*(uL
  //        - uR);
  //        // uS = 0.5*(uL - uR) + CL - CR;
  //        //
  //      }

  //    private:
  //      // 				cellFacesVar m_U1LR;
  //      // 				cellFacesVar m_U2LR;
  //      // HLL solver variables
  //      double uR, uL;
  //      double SR, SL;
  //      double CR, CL;
  //      double uS, CS;
  //      EMcNeil1D *m_numRep;
  //      double FL;
  //      double FR;
  //      PairLeftRight m_FF;
  //    } HLLParams; // struct HLLParams
  //                 //}
  //  };             // class Flux solver
  std::vector<double> m_U1;
  std::vector<double> m_U2;

}; // class TestBCSect
} // namespace dbpp
