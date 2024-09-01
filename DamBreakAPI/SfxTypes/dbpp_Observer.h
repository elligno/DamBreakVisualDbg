#pragma once

namespace dbpp {
/** Brief Observer design pattern
 *
 *  Provide a single method for client called to update
 *  after a notification (setState).
 */
class Observer {
public:
  virtual ~Observer() = default;
  virtual void update() = 0;
};
} // namespace dbpp
