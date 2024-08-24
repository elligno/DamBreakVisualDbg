#pragma once

// C++ includes
#include <cstdio>
#include <stdarg.h>
// project include
#include "../SfxTypes/dbpp_Singleton.hpp"

namespace dbpp {
/** Brief class to output an error log.
 *	  Use the CRTP idiom (inheriting framework type) Singleton
 *    passing in the argument template (Curious Recursive Template Pattern).
 *    Singleton pattern for managing the sole instance of  the logger.
 *
 *    Usage:
 *      dbpp::Logger::instance()->OutputSuccess("Initial time step is: %f",
 *w_dt);
 */
class Logger : public Singleton<Logger> {
public:
  /**
   *   deprecated
   */
  bool Init(/*char * filename*/);
  /**
   *   deprecated
   */
  bool Shutdown(void);

  /**
   *   deprecated
   */
  // Output to log
  void OutputNewline();
  /**
   *   deprecated
   */
  void OutputError(char *text, ...);
  /**
   *   deprecated
   */
  void OutputSuccess(char *text, ...);

  /**
   *  Access to Logger ctor
   */
  friend class Singleton<Logger>;

protected:
  /**
   *   deprecated
   */
  // Default ctor
  Logger() {}
  /**
   *   deprecated
   */
  // Destructor
  ~Logger() { Shutdown(); }

  FILE *logfile; /**< file to log info*/
  bool m_opened; /**< flag file status*/
                 // bool Shutdown(void);
};
} // namespace dbpp
