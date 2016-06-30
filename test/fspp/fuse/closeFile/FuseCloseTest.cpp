#include "../../testutils/FuseTest.h"
#include <condition_variable>

using ::testing::_;
using ::testing::StrEq;
using ::testing::Eq;
using ::testing::WithParamInterface;
using ::testing::Values;
using ::testing::Return;
using ::testing::Invoke;
using ::testing::InSequence;
using ::testing::AtLeast;

using std::string;
using std::mutex;
using std::unique_lock;
using std::condition_variable;
using std::chrono::duration;
using std::chrono::seconds;

// The fuse behaviour is: For each open(), there will be exactly one call to release().
// Directly before this call to release(), flush() will be called. After flush() returns,
// the ::close() syscall (in the process using the filesystem) returns. So the fuse release() call is
// called asynchronously afterwards. Errors have to be returned in the implementation of flush().

// Citing FUSE spec:
//  1) Flush is called on each close() of a file descriptor.
//  2) Filesystems shouldn't assume that flush will always be called after some writes, or that if will be called at all.
// I can't get these sentences together. For the test cases here, I go with the first one and assume that
// flush() will ALWAYS be called on a file close.

class Barrier {
public:
  Barrier(): m(), cv(), finished(false) {}

  template<class A, class B>
  void WaitAtMost(const duration<A, B> &atMost) {
    unique_lock<mutex> lock(m);
    if (!finished) {
      cv.wait_for(lock, atMost, [this] () {return finished;});
    }
  }

  void Release() {
    unique_lock<mutex> lock(m);
    finished = true;
    cv.notify_all();
  }
private:
  mutex m;
  condition_variable cv;
  bool finished;
};

class FuseCloseTest: public FuseTest, public WithParamInterface<int> {
public:
  const string FILENAME = "/myfile";

  void OpenAndCloseFile(const string &filename) {
    auto fs = TestFS();
    int fd = OpenFile(fs.get(), filename);
    CloseFile(fd);
  }

  int OpenFile(const TempTestFS *fs, const string &filename) {
    auto real_path = fs->mountDir() / filename;
    int fd = ::open(real_path.c_str(), O_RDONLY);
    EXPECT_GE(fd, 0) << "Opening file failed";
    return fd;
  }

  void CloseFile(int fd) {
    int retval = ::close(fd);
    EXPECT_EQ(0, retval);
  }
};
INSTANTIATE_TEST_CASE_P(FuseCloseTest, FuseCloseTest, Values(0, 1, 2, 100, 1024*1024*1024));

//TODO Figure out what's wrong and enable this test
//Disabled, because it is flaky. libfuse seems to not send the release() event sometimes.
/*TEST_P(FuseCloseTest, CloseFile) {
  Barrier barrier;

  ReturnIsFileOnLstat(FILENAME);
  EXPECT_CALL(fsimpl, openFile(StrEq(FILENAME), _)).WillOnce(Return(GetParam()));
  {
    //InSequence fileCloseSequence;
    EXPECT_CALL(fsimpl, flush(Eq(GetParam()))).Times(1);
    EXPECT_CALL(fsimpl, closeFile(Eq(GetParam()))).Times(1).WillOnce(Invoke([&barrier] (int) {
      // Release the waiting lock at the end of this test case, because the fuse release() came in now.
      barrier.Release();
    }));
  }

  OpenAndCloseFile(FILENAME);

  // Wait, until fuse release() was called, so we can check for the function call expectation.
  barrier.WaitAtMost(seconds(10));
}*/
