
#pragma once

#include <QObject>

namespace Util {
// https://mayaposch.wordpress.com/2011/11/01/how-to-really-truly-use-qthreads-the-full-explanation/
// While this Worker class doesn’t do anything special, it nevertheless contains
// all the required elements. It starts processing when its main function, in
// this case process(), is called and when it is done it emits the signal
// finished() which will then be used to trigger the shutdown of the QThread
// instance it is contained in. Some QThread test on how to use it.
class Worker : public QObject {
  Q_OBJECT

public:
  Worker();
  ~Worker();

public slots:
  void process();
  // as a first test with Qthread, just do some wrting
  // in the text editor
  void doWork(const QString &parameter) {
    QString result{"Completed the iteration no: "};
    /* ... here is the expensive or blocking operation ... */
    result += parameter;
    emit resultReady(result); // print to GUI text editor
  }
signals:
  void finished();
  void error(QString err);
  void resultReady(const QString &result);

private:
  // add your variables here
};
} // namespace Util
