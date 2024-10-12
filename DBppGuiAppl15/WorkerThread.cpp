
#include "WorkerThread.h"

namespace Util {
// By the way, one extremely important thing to note here is that you should
// NEVER allocate heap objects (using new) in the constructor of the QObject
// class as this allocation is then performed on the main thread and not on the
// new QThread instance, meaning that the newly created object is then owned by
// the main thread and not the QThread instance. This will make your code fail
// to work. Instead, allocate such resources in the main function slot such as
// process() in this case as when that is called the object will be on the new
// thread instance and thus it will own the resource.

// --- CONSTRUCTOR ---
Worker::Worker() {
  // you could copy data from constructor arguments to internal variables here.
}

// --- DECONSTRUCTOR ---
Worker::~Worker() {
  // free resources
}

// --- PROCESS ---
// Start processing data.
void Worker::process() {
  // allocate resources using new here
  qDebug("Hello World!");
  emit finished();
}
} // namespace Util
