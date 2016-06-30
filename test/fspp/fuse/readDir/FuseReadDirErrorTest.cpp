#include "testutils/FuseReadDirTest.h"

#include "fspp/fuse/FuseErrnoException.h"

using ::testing::_;
using ::testing::StrEq;
using ::testing::Throw;
using ::testing::WithParamInterface;
using ::testing::Values;

using std::vector;
using std::string;

using namespace fspp::fuse;

class FuseReadDirErrorTest: public FuseReadDirTest, public WithParamInterface<int> {
};
INSTANTIATE_TEST_CASE_P(FuseReadDirErrorTest, FuseReadDirErrorTest, Values(EACCES, EBADF, EMFILE, ENFILE, ENOMEM, ENOTDIR, EFAULT, EINVAL));

//TODO On ENOENT, libfuse doesn't return the ENOENT error, but returns a success response with an empty directory. Why?

TEST_F(FuseReadDirErrorTest, NoError) {
  ReturnIsDirOnLstat(DIRNAME);
  EXPECT_CALL(fsimpl, readDir(StrEq(DIRNAME)))
    .Times(1).WillOnce(ReturnDirEntries({}));

  int error = ReadDirReturnError(DIRNAME);
  EXPECT_EQ(0, error);
}

TEST_P(FuseReadDirErrorTest, ReturnedErrorCodeIsCorrect) {
  ReturnIsDirOnLstat(DIRNAME);
  EXPECT_CALL(fsimpl, readDir(StrEq(DIRNAME)))
    .Times(1).WillOnce(Throw(FuseErrnoException(GetParam())));

  int error = ReadDirReturnError(DIRNAME);
  EXPECT_EQ(GetParam(), error);
}
