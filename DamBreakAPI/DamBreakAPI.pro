QT -= gui

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Algorithm/dbpp_SweRhsAlgorithm.cpp \
    Algorithm/dbpp_TestRhsImpl.cpp \
    Discretization/dbpp_BCharacteristicSolver.cpp \
    Discretization/dbpp_BaseRhsDiscretization.cpp \
    Discretization/dbpp_EMcNeilBCimpl.cpp \
    Discretization/dbpp_EMcnilGamma.cpp \
    Discretization/dbpp_EmcilNumTreatment.cpp \
    Discretization/dbpp_GlobalDiscretization.cpp \
    Discretization/dbpp_HydroTerms.cpp \
    Discretization/dbpp_NodalVariables.cxx \
    Discretization/dbpp_ReconstrUtility.cpp \
    MathModel/dbpp_MathEquations.cpp \
    NumericalSchemes/dbpp_EMcNeil1d_f.cpp \
    NumericalSchemes/dbpp_EMcNeil1d_mod.cpp \
    NumericalSchemes/dbpp_HLL_RK2_Algo_Vec.cpp \
    NumericalSchemes/dbpp_TestBcSectF.cpp \
    NumericalSchemes/dbpp_TestEMcNeilVec.cpp \
    Numerics/dbpp_TestCalculFF.cpp \
    Numerics/dbpp_TimeStepCriteria.cpp \
    SfxTypes/dbpp_DamBreakData.cpp \
    SfxTypes/dbpp_EMcNeil1DFactory.cpp \
    SfxTypes/dbpp_ListSectFlow.cpp \
    SfxTypes/dbpp_PhyConstant.cxx \
    SfxTypes/dbpp_Simulation.cpp \
    SfxTypes/dbpp_WaveFunc.cpp \
    SfxTypes/dbpp_cellFace.cpp \
    SfxTypes/dbpp_gridLattice.cpp \
    SfxTypes/dbpp_scalarField.cpp \
    Utility/dbpp_CommandLineArgs.cpp \
    Utility/dbpp_EMcNeilUtils.cpp \
    Utility/dbpp_GuiEnvVarMgr.cpp \
    Utility/dbpp_Hydro1DLogger.cpp \
    Utility/dbpp_SimulationUtility.cpp \
    Utility/dbpp_TestLogger.cxx \
    dbpp_Wave1DSimulator.cpp

HEADERS += \
    Algorithm/dbpp_SweRhsAlgorithm.h \
    Algorithm/dbpp_TestRhsImpl.h \
    Discretization/dbpp_BCharacteristicSolver.h \
    Discretization/dbpp_BaseRhsDiscretization.h \
    Discretization/dbpp_EMcNeilBCimpl.h \
    Discretization/dbpp_EMcnilGamma.h \
    Discretization/dbpp_EmcilNumTreatment.h \
    Discretization/dbpp_GlobalDiscretization.h \
    Discretization/dbpp_HydroTerms.h \
    Discretization/dbpp_NodalVariables.h \
    Discretization/dbpp_ReconstrUtility.h \
    MathModel/dbpp_MathEquations.h \
    MathModel/dbpp_StVenant1D.h \
    NumericalSchemes/dbpp_EMcNeil1d_f.h \
    NumericalSchemes/dbpp_EMcNeil1d_mod.h \
    NumericalSchemes/dbpp_HLL_RK2_Algo_Vec.h \
    NumericalSchemes/dbpp_TestBcSectF.h \
    NumericalSchemes/dbpp_TestEMcNeilVec.h \
    Numerics/dbpp_TestCalculFF.h \
    Numerics/dbpp_TimeStepCriteria.hpp \
    Numerics/dbpp_UpwindScheme1st.hpp \
    SfxTypes/dbpp_BaseMacros.h \
    SfxTypes/dbpp_DamBreakData.h \
    SfxTypes/dbpp_DamBreakStepFunc.hpp \
    SfxTypes/dbpp_DefineTypes.h \
    SfxTypes/dbpp_EMcNeil1DFactory.h \
    SfxTypes/dbpp_ExceptionUtils.h \
    SfxTypes/dbpp_IRhsDiscretization.h \
    SfxTypes/dbpp_ListSectFlow.h \
    SfxTypes/dbpp_Observer.h \
    SfxTypes/dbpp_PhyConstant.h \
    SfxTypes/dbpp_RealNumArray.h \
    SfxTypes/dbpp_Simulation.h \
    SfxTypes/dbpp_Singleton.hpp \
    SfxTypes/dbpp_TimePrm.h \
    SfxTypes/dbpp_WaveFunc.h \
    SfxTypes/dbpp_cellFace.h \
    SfxTypes/dbpp_gridLattice.h \
    SfxTypes/dbpp_scalarField.h \
    Utility/dbpp_AppConstant.hpp \
    Utility/dbpp_CommandLineArgs.h \
    Utility/dbpp_EMcNeilUtils.h \
    Utility/dbpp_EnvVarMgr.h \
    Utility/dbpp_GuiEnvVarMgr.h \
    Utility/dbpp_Hydro1DLogger.h \
    Utility/dbpp_HydroCodeDef.h \
    Utility/dbpp_SimulationUtility.h \
    Utility/dbpp_SomeUtilities.hpp \
    Utility/dbpp_TestLogger.h \
    dbpp_Wave1DSimulator.h

INCLUDEPATH += $(BOOST_ROOT)
INCLUDEPATH += $$PWD/Utility
INCLUDEPATH += $$PWD/SfxTypes
INCLUDEPATH += $$PWD/Numerics
INCLUDEPATH += $$PWD/NumericalSchemes
INCLUDEPATH += $$PWD/MathModel
INCLUDEPATH += $$PWD/Discretization
INCLUDEPATH += $$PWD/Algorithm

TRANSLATIONS += \
    DamBreakAPI_en_US.ts

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target
