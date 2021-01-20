#include <stdio.h>
#include <chrono>
#include <thread>

#include "hello.decl.h"

/*readonly*/ CProxy_Main mainProxy;
/*readonly*/ int nElements;
/*readonly*/ int iters;

/*mainchare*/
class Main : public CBase_Main {
public:
  Main(CkArgMsg* m) {
    //Process command-line arguments
    nElements = 5;
    if (m->argc > 1) nElements=atoi(m->argv[1]);
    iters = 10;
    if (m->argc > 2) iters=atoi(m->argv[2]);
    delete m;

    //Start the computation
    CkPrintf("Running Hello on %d processors for %d elements\n", CkNumPes(), nElements);

    mainProxy = thisProxy;
    CProxy_Hello arr = CProxy_Hello::ckNew();
    for (int i = 0; i < nElements; i++) {
      arr[i].insert();
    }
    arr.doneInserting();

    arr[0].SayHi();
  };

  void done(void) {
    CkPrintf("All done\n");
    CkExit();
  };
};

class Hello : public CBase_Hello {
private:
  int counter;
  int originalPE;
public:
  Hello() {
    CkPrintf("Hello (%d) created on %d\n",
             thisIndex, CkMyPe());
    counter = 0;
    originalPE = CkMyPe();
    usesAtSync = true;
  }

  Hello(CkMigrateMessage *m) {}

  void ResumeFromSync() {
    CkPrintf("[%d] %d resume from sync, iter %d\n", CkMyPe(), thisIndex, counter);
    if (thisIndex == 0)
      thisProxy[thisIndex].SayHi();
  }

  void pup(PUP::er &p) {
    p | counter;
    p | originalPE;
  }

  void SayHi() {
    counter++;
    const int value = thisIndex;
    const int expectedPE = (originalPE + counter - 1) % CkNumPes();
    CkPrintf("[%d](%d) *** Iter %d from element %d\n", CkMyPe(), expectedPE, counter, value);
    //CkAssert(expectedPE == CkMyPe());
    if (value + 1 < nElements)
    {
      CkPrintf("[%d](%d) Sending to %d from element %d\n", CkMyPe(), expectedPE, thisIndex + 1, thisIndex);
      thisProxy[thisIndex + 1].SayHi();
    }

    //std::this_thread::sleep_for (std::chrono::milliseconds(1000));
    if (counter < iters) {
      AtSync();
    } else {
      mainProxy.done();
    }
  }
};

#include "hello.def.h"
