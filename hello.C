#include <stdio.h>
#include <chrono>
#include <thread>
// *MUST* include "charm++.h" to inherit from CkArrayIndex
#include "charm++.h"

// *MUST* declare index type *BEFORE* we include hello.decl.h!

// This is our application-specific index type. It's a completely ordinary C++
// class (or even C struct) -- the only requirement is that all the data be
// allocated locally (no pointers, no virtual methods).
class Fancy {
private:
  int value;
//  int dummy;
public:
  Fancy() : value(0) {} //, dummy(0) {}
  Fancy(int val) : value(val) {} //, dummy(0) {}
  int getValue() const { return value; }
};

// This adapts the application's index for use by the array manager. This class
// is only used by the translator -- you never need to refer to it again!
class CkArrayIndexFancy : public CkArrayIndex {
private:
  Fancy* idx;
public:
  CkArrayIndexFancy() {
    idx = new(index) Fancy();
    nInts = sizeof(Fancy) / sizeof(int);
  }

  CkArrayIndexFancy(const Fancy& f) {
    idx = new(index) Fancy(f);
    nInts = sizeof(Fancy) / sizeof(int);
  }
};

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
      const Fancy f(i);
      arr[f].insert();
    }
    arr.doneInserting();

    arr[Fancy(0)].SayHi();
  };

  void done(void) {
    CkPrintf("All done\n");
    CkExit();
  };
};

/*array [Fancy]*/
class Hello : public CBase_Hello {
private:
  int counter;
public:
  Hello() {
    // Note how thisIndex is of type fancyIndex:
    CkPrintf("Hello (%d) created on %d\n",
             thisIndex.getValue(), CkMyPe());
    counter = 0;
    usesAtSync = true;
  }

  Hello(CkMigrateMessage *m) {}

  void ResumeFromSync() {
    CkPrintf("[%d] %d resume from sync\n", CkMyPe(), thisIndex.getValue());             
    if (thisIndex.getValue() == 0)
      thisProxy[thisIndex].SayHi();
  }

  void pup(PUP::er &p) {
    p | counter;
  }

  void SayHi() {
    counter++;
    const int value = thisIndex.getValue();
    CkPrintf("[%d] Iter %d from element %d\n", CkMyPe(), counter, value);
    if (value + 1 < nElements)
      thisProxy[Fancy(thisIndex.getValue() + 1)].SayHi();

    //std::this_thread::sleep_for (std::chrono::milliseconds(1000));
    if (counter < iters) {
      AtSync();
    } else {
      mainProxy.done();
    }
  }
};

#include "hello.def.h"
