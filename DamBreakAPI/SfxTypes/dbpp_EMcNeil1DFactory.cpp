#include "dbpp_EMcNeil1DFactory.h"
//#include "EMcNeil/HLL_RK2_Algo_Vec.h"
#include "../Utility/dbpp_TestLogger.h"
// SUPPORTED ALGORITHM
#include "../NumericalSchemes/dbpp_EMcNeil1d_f.h"
#include "../NumericalSchemes/dbpp_EMcNeil1d_mod.h"
//#include "../NumericalSchemes/dbpp_TestBcSectF.h"
//#include "Test/TestNewAlgo.h"
//#include "TestEMcNeilVec.h"
//#include "TestNewAlgo.h"
//#include "Test/TestCellFaceImpl.h"
// concept of global discretization
//#include "Test/GlobalFVDiscr.h"

namespace dbpp {
using shrPtr2EMcNeil1D = std::shared_ptr<dbpp::EMcNeil1D>;

EMcNeil1DFactory::EMcNeil1DFactory() {
  auto w_msg = "EMcNeil1DFactory::EMcNeil1DFactory() ctor";
  dbpp::Logger::instance()->OutputSuccess(const_cast<char *>(w_msg));
}

//	EMcNeil1DFactory::~EMcNeil1DFactory()
//	{
//		dbpp::Logger::instance()->OutputSuccess("EMcNeil1DFactory::~EMcNeil1DFactory()
// dtor");
//	}

///////////////////////////////////////////////////////////////////////////////////////////////
// Function:    CreateSolver
// Class:       CSolverFactory
// Purpose:     Create an engine
// Inputs:      eType               type of solver
//              rSettings           settings for solver
// Outputs:     return              pointer to the new solver or NULL if not a
// known type
///////////////////////////////////////////////////////////////////////////////////////////////
shrPtr2EMcNeil1D
EMcNeil1DFactory::CreateSolver(EMcNeil1DFactory::EMcNeil1DType eType,
                               const DamBreakData &rSettings) {
  //		CSolver* pSolver = NULL;
  switch (eType) {
    // 		case EMcNeil1DFactory::EMcNeil1DType::EMCNEIL1D:
    // 			{
    // 			//	pSolver = new CMilsteinSolver(rSettings);
    // 				shrPtr2EMcNeil1D w_numRep =
    // std::make_shared<emcil::EMcNeil1D>();
    // 				// sanity check
    // 				if( w_numRep)
    // 				{
    // 					emcil::Logger::instance()->OutputSuccess("EMcNeil1D
    // class created");
    // 				}
    // 				// not sure about this
    // 				return w_numRep;
    // 				break;
    // 			}
  case EMcNeil1DFactory::EMcNeil1DType::EMCNEIL1D_MOD: {
    //		pSolver = new CRungeKuttaSolver(rSettings);
    shrPtr2EMcNeil1D w_numRep = std::make_shared<dbpp::EMcNeil1d_mod>();
    // sanity check
    if (w_numRep) {
      // auto w_msg = std::string{"EMcNeil1D_mod class created"};
      dbpp::Logger::instance()->OutputSuccess(
          std::string{"EMcNeil1D_mod class created"}.data());
    }
    // not sure about this
    return w_numRep;
    break;
  }
  case EMcNeil1DFactory::EMcNeil1DType::EMCNEIL1D_F: {
    //		pSolver = new CEMSolver(rSettings);
    shrPtr2EMcNeil1D w_numRep = std::make_shared<dbpp::EMcNeil1d_f>();
    if (w_numRep) {
      dbpp::Logger::instance()->OutputSuccess(
          std::string{"EMcNeil1D_f class created"}.data());
    }
    return w_numRep;
    break;
  }
    // 		case EMcNeil1DFactory::EMcNeil1DType::TESTEMCNEILVEC:
    // 			{
    // 		//		pSolver = new CEMSolver(rSettings);
    // 				shrPtr2EMcNeil1D w_numRep =
    // std::make_shared<dbapp::TestEMcNeilVec>();
    //
    // 				if( w_numRep)
    // 				{
    // 					dbapp::Logger::instance()->OutputSuccess("TestEMcNeilVec
    // class created");
    // 				}
    //
    // 				return w_numRep;
    // 				break;
    // 			}
    //     case EMcNeil1DType::TESTBCSECTF:
    //       {
    //         // create an instance
    //         shrPtr2EMcNeil1D w_numRep =
    //         std::make_shared<dbpp::TestBcSectF>(); if( w_numRep)
    //         {
    //           dbpp::Logger::instance()->OutputSuccess("TestBcSectF class
    //           created");
    //         }
    //
    //         return w_numRep;
    //         break;
    //       }
    //     case EMcNeil1DType::TESTNEWALGO:
    //       {
    //         // create an instance
    //         shrPtr2EMcNeil1D w_numRep =
    //         std::make_shared<dbapp::TestNewAlgo>(); if( w_numRep)
    //         {
    //           dbapp::Logger::instance()->OutputSuccess("TestNewAlgo class
    //           created");
    //         }
    //
    //         return w_numRep;
    //         break;
    //       }
  default:
    return std::shared_ptr<dbpp::EMcNeil1D>(nullptr);

    // Add implementations here
  }

  //		return pSolver; i am not sure about that!!
  return std::shared_ptr<dbpp::EMcNeil1D>(nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Function:    CreateSolver
// Class:       CSolverFactory
// Purpose:     Create an engine
// Inputs:      rstrType            name of solver type
//              rSettings           settings for solver
// Outputs:     return              pointer to the new solver or NULL if not a
// known type
///////////////////////////////////////////////////////////////////////////////////////////////
shrPtr2EMcNeil1D EMcNeil1DFactory::CreateSolver(const std::string &rstrType,
                                                const DamBreakData &rSettings) {
  return CreateSolver(GetType(rstrType), rSettings);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Function:    GetName
// Class:       CSolverFactory
// Purpose:     Get the name of the solver
// Inputs:      eType               type of solver to create
// Outputs:     return              name of solver
///////////////////////////////////////////////////////////////////////////////////////////////
std::string EMcNeil1DFactory::GetName(EMcNeil1DFactory::EMcNeil1DType eType) {
  std::string strName{};

  //   		if (EMcNeil1DFactory::EMcNeil1DType::EMCNEIL1D == eType)
  //   		{
  //   			strName = "EMCNEIL1D";
  //   		}
  if (EMcNeil1DFactory::EMcNeil1DType::EMCNEIL1D_MOD == eType) {
    strName = "EMCNEIL1D_MOD";
  } else if (EMcNeil1DFactory::EMcNeil1DType::EMCNEIL1D_F == eType) {
    strName = "EMCNEIL1D_F";
  } else if (EMcNeil1DFactory::EMcNeil1DType::TESTEMCNEILVEC == eType) {
    strName = "TESTEMCNEILVEC";
  } else if (EMcNeil1DFactory::EMcNeil1DType::TESTBCSECTF == eType) {
    strName = "TESTBCSECTF";
  } else if (EMcNeil1DFactory::EMcNeil1DType::TESTNEWALGO == eType) {
    strName = "TESTNEWALGO";
  }

  // Add implementations here

  return strName;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Function:    GetType
// Class:       CSolverFactory
// Purpose:     Get the solver type from name
// Inputs:      rstrName            name of solver
// Outputs:     return              the type
///////////////////////////////////////////////////////////////////////////////////////////////
EMcNeil1DFactory::EMcNeil1DType
EMcNeil1DFactory::GetType(const std::string &rstrName) {
  // working variable that hold uppercase
  std::string w_strUpperCase{};

  // make string upper case for comparison
  std::transform(rstrName.begin(), rstrName.end(),
                 std::back_inserter(w_strUpperCase), ::toupper);

  // 		if( w_strUpperCase == "EMCNEIL1D")
  // 		{
  // 			return EMcNeil1DFactory::EMcNeil1DType::EMCNEIL1D;
  // 		}
  if (w_strUpperCase == "EMCNEIL1D_MOD") {
    return EMcNeil1DFactory::EMcNeil1DType::EMCNEIL1D_MOD;
  } else if (w_strUpperCase == "EMCNEIL1D_F") {
    return EMcNeil1DFactory::EMcNeil1DType::EMCNEIL1D_F;
  } else if (w_strUpperCase == "TESTEMCNEILVEC") {
    return EMcNeil1DFactory::EMcNeil1DType::TESTEMCNEILVEC;
  } else if (w_strUpperCase == "TESTBCSECTF") {
    return EMcNeil1DFactory::EMcNeil1DType::TESTBCSECTF;
  } else if (w_strUpperCase == "TESTNEWALGO") {
    return EMcNeil1DFactory::EMcNeil1DType::TESTNEWALGO;
  } else {
    // printf("Unrecognised solver name. Using EM\n");
    auto w_msg = "Unrecognised solver name. Using EMcNeil1D\n";
    dbpp::Logger::instance()->OutputError(const_cast<char *>(w_msg));
    return EMcNeil1DFactory::EMcNeil1DType::EMCNEIL1D_MOD; // default
  }

  // Add implementations here
}

// 	std::shared_ptr<emcil::EMcNeil1D>
// EMcNeil1DFactory::CreateSolver(SolverType eType, const DamBreakData &
// rSettings)
// 	{
// 		typedef std::shared_ptr<emcil::EMcNeil1D> shrPtr2EMcNeil1D;
//
// 		//
// 		// compare algorithm name and create corresponding impl.
// 		//
//
// 		// Description
// 		//   Use BaseNumTreatment as rhs algorithm (which is E. McNeil
// impl.) 		if( aAlgoName == std::string("EMcNeil1D"))
// 		{
// 			shrPtr2EMcNeil1D
// w_numRep=std::make_shared<emcil::EMcNeil1D>();
// 			// sanity check
// 			if( w_numRep)
// 			{
// 				emcil::Logger::instance()->OutputSuccess("EMcNeil1D
// class created");
// 			}
//
// 			// not sure about this
// 			return w_numRep;
// 		}
// 		// Description
// 		//   Use TestCalculFF impl. which use reconstruction procedure
// 		//   outside of the HLL calculFF, implement the interface
// 		//   IFluxAlgoImpl which provides methods for calculFF.
// 		else if( aAlgoName == std::string("TestNewAlgo"))
// 		{
// 			// create the global discretization
// 			Omega w_Domain;
// 			U_h w_U12(3,w_Domain);
// 			GlobalFVDiscr w_testFVdiscr(&w_Domain,&w_U12);
//
// 			// return some advance impl
// 			shrPtr2EMcNeil1D w_numRep =
// 				std::make_shared<emcil::TestNewAlgo>(&w_testFVdiscr);
//
// 			if( w_numRep)
// 			{
// 				emcil::Logger::instance()->OutputSuccess("TestNewAlgo
// class created");
// 			}
//
// 			return w_numRep.get();
// 		}
// 		// Description
// 		//   Use the SweRhsAlgorithm type to compute spatial terms.
// 		//   class TestRhsImpl is an implementation of SweRhsAlgorithm.
// 		//   This impl. is based on the reconstruction procedure of
// 		//   SweRhsAlgorithm and TestCalculFF class (calculFF()).
// 		else if( aAlgoName == std::string("TestEMcNeilVec"))
// 		{
// 			shrPtr2EMcNeil1D
// w_numRep=std::make_shared<TestEMcNeilVec>();
//
// 			if( w_numRep)
// 			{
// 				emcil::Logger::instance()->OutputSuccess("TestEMcNeilVec
// class created");
// 			}
//
// 			return w_numRep.get();
// 		}
// 		else // return null pointer not supported
// 		{
// 			return std::shared_ptr<emcil::EMcNeil1D>(nullptr).get();
// 		}
// 	}

} // namespace dbpp
