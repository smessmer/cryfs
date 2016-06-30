#include "testutils/FuseReadTest.h"

#include "fspp/fuse/FuseErrnoException.h"

using ::testing::_;
using ::testing::StrEq;
using ::testing::WithParamInterface;
using ::testing::Values;
using ::testing::Eq;
using ::testing::Ne;
using ::testing::Return;
using ::testing::Invoke;
using ::testing::Throw;

using namespace fspp::fuse;

class FuseReadErrorTest: public FuseReadTest, public WithParamInterface<int> {
public:
  size_t FILESIZE = 64*1024*1024;
  size_t READCOUNT = 32*1024*1024;

  void SetUp() override {
    //Make the file size big enough that fuse should issue at least two reads
    ReturnIsFileOnLstatWithSize(FILENAME, FILESIZE);
    OnOpenReturnFileDescriptor(FILENAME, 0);
  }
};
INSTANTIATE_TEST_CASE_P(FuseReadErrorTest, FuseReadErrorTest, Values(EAGAIN, EBADF, EFAULT, EINTR, EINVAL, EIO, EISDIR, EOVERFLOW, ESPIPE, ENXIO));


TEST_P(FuseReadErrorTest, ReturnErrorOnFirstReadCall) {
  EXPECT_CALL(fsimpl, read(0, _, _, _))
    .WillRepeatedly(Throw(FuseErrnoException(GetParam())));

  char *buf = new char[READCOUNT];
  auto retval = ReadFileReturnError(FILENAME, buf, READCOUNT, 0);
  EXPECT_EQ(GetParam(), retval.error);
  delete[] buf;
}

TEST_P(FuseReadErrorTest, ReturnErrorOnSecondReadCall) {
  // The first read request is from the beginning of the file and works, but the later ones fail.
  // We store the number of bytes the first call could successfully read and check later that our
  // read syscall returns exactly this number of bytes
  size_t successfullyReadBytes = -1;
  EXPECT_CALL(fsimpl, read(0, _, _, Eq(0)))
    .Times(1)
    .WillOnce(Invoke([&successfullyReadBytes](int, void*, size_t count, off_t) {
      // Store the number of successfully read bytes
      successfullyReadBytes = count;
      return count;
    }));
  EXPECT_CALL(fsimpl, read(0, _, _, Ne(0)))
    .WillRepeatedly(Throw(FuseErrnoException(GetParam())));

  char *buf = new char[READCOUNT];
  auto retval = ReadFileReturnError(FILENAME, buf, READCOUNT, 0);
  EXPECT_EQ(0, retval.error);
  EXPECT_EQ(successfullyReadBytes, retval.read_bytes); // Check that we're getting the number of successfully read bytes (the first read call) returned
  delete[] buf;
}
