#include "testutils/FuseFTruncateTest.h"

#include "fspp/fuse/FuseErrnoException.h"

using ::testing::_;
using ::testing::StrEq;
using ::testing::Throw;
using ::testing::WithParamInterface;
using ::testing::Values;

using namespace fspp::fuse;

class FuseFTruncateErrorTest: public FuseFTruncateTest, public WithParamInterface<int> {
};
INSTANTIATE_TEST_CASE_P(FuseFTruncateErrorTest, FuseFTruncateErrorTest, Values(EACCES, EFAULT, EFBIG, EINTR, EINVAL, EIO, EISDIR, ELOOP, ENAMETOOLONG, ENOENT, ENOTDIR, EPERM, EROFS, ETXTBSY, EBADF));

TEST_P(FuseFTruncateErrorTest, ReturnedErrorIsCorrect) {
  ReturnIsFileOnLstat(FILENAME);
  OnOpenReturnFileDescriptor(FILENAME, 0);
  EXPECT_CALL(fsimpl, ftruncate(0, _))
    .Times(1).WillOnce(Throw(FuseErrnoException(GetParam())));
  //Needed to make ::ftruncate system call return successfully
  ReturnIsFileOnFstat(0);

  int error = FTruncateFileReturnError(FILENAME, 0);
  EXPECT_EQ(GetParam(), error);
}
