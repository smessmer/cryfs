#include "testutils/FuseFdatasyncTest.h"

#include "fspp/fuse/FuseErrnoException.h"

using ::testing::_;
using ::testing::StrEq;
using ::testing::WithParamInterface;
using ::testing::Values;
using ::testing::Eq;
using ::testing::Return;

using namespace fspp::fuse;

class FuseFdatasyncFileDescriptorTest: public FuseFdatasyncTest, public WithParamInterface<int> {
};
INSTANTIATE_TEST_CASE_P(FuseFdatasyncFileDescriptorTest, FuseFdatasyncFileDescriptorTest, Values(0,1,10,1000,1024*1024*1024));


TEST_P(FuseFdatasyncFileDescriptorTest, FileDescriptorIsCorrect) {
  ReturnIsFileOnLstat(FILENAME);
  OnOpenReturnFileDescriptor(FILENAME, GetParam());
  EXPECT_CALL(fsimpl, fdatasync(Eq(GetParam())))
    .Times(1).WillOnce(Return());

  FdatasyncFile(FILENAME);
}
