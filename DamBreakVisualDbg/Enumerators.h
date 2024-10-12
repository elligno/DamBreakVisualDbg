
#pragma once

namespace qplot {

enum class eVariableProfile {
  XH, /**< */
  XQ, /**< */
  XV, /**< */
  XF  /**< */
};
enum class eDataTypeFmt {
  None,
  DebugFmt,       /**< debug log*/
  FullResFmt,     /**< simulation result*/
  VarProfileFmt,  /**< final profile*/
  ExactComputeFmt /**< */
};
enum class eDataTypes {
  EMcNeilDataType, /**< */
  HudsonDataType   /**< */
};
enum class eGraphType {
  profile, /**< 1 graph*/
  compare, /**< 2 graph*/
  multiple /**< 2,... graph*/
};
// Support of 4 format (default one is debug)
// enum class eFileFmt {debug, Variable Profile, Full result};
enum class eFileResultFormat {
  dbgFormat,
  var_profile,
  full_result,
  exact_computed
};

} // namespace qplot
