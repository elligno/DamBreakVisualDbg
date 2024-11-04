#pragma once

namespace dbpp {
// description : TimePrm is a class for handling logical time
//
//  Design Note: this class need a serious check and re-factoring
//  not clear to me what we do exactly. Need to be enhanced with
//  new methods ... to be completed
class TimePrm {
public:
  enum TimeStepMode { FIXED_TIME_STEP = 0, VARIABLE_TIME_STEP = 1 };

private:
  // more detail coming soon
  // stepping has 2 mode,
  TimeStepMode m_stepMode; // ...
  double time_;            // current time value
  double delta;            // time step size
  double stop;             // stop time
  double m_start;
  int timestep; // time step counter
                // bool   m_fixedTimeStep;

public:
  TimePrm(double start_, double delta_, double stop_ /*, bool aFixedT=true*/)
      : time_(start_), delta(delta_), stop(stop_), m_start(start_)
  // m_fixedTimeStep(aFixedT)
  {
    m_stepMode = FIXED_TIME_STEP;
    initTimeLoop();
  }

  // default copy and assignment will do (move copy and assignment)

  // alternative initialization:
  // { time_=start_; delta=delta_; stop=stop_; initTimeLoop(); }

  double time() { return time_; }
  double Delta() { return delta; }
  void initTimeLoop() {
    time_ = 0;
    timestep = 0;
  }
  bool finished() { return (time_ >= stop) ? true : false; }
  void setTimeStepMode(TimeStepMode aMode) { m_stepMode = aMode; }
  void increaseTime(double aDelta = 0.) // i am not sure about this one
  {
    if (m_stepMode == FIXED_TIME_STEP) {
      time_ += delta;
      ++timestep;
    } else // variable time step
    {
      delta = aDelta; // set new time step
      time_ += delta;
      ++timestep;
    }
  }
  // time parameters of the simulation
  int getTimeStepNo() const { return timestep; }
  double getFinalTime() const { return stop; }
  void setStopTime(const double aStop) { stop = aStop; }
  double getStartTime() const { return m_start; }
  void setStartTime(double aStartime) { m_start = aStartime; }
};
} // namespace dbpp
