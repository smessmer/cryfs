#include "testutils/FuseCreateAndOpenTest.h"

using ::testing::_;
using ::testing::StrEq;
using ::testing::WithParamInterface;
using ::testing::Values;
using ::testing::Return;

class FuseCreateAndOpenFlagsTest: public FuseCreateAndOpenTest, public WithParamInterface<mode_t> {
};
INSTANTIATE_TEST_CASE_P(FuseCreateAndOpenFlagsTest, FuseCreateAndOpenFlagsTest, Values(O_RDWR, O_RDONLY, O_WRONLY));

//TODO Disabled because it doesn't seem to work. Fuse doesn't seem to pass flags to create(). Why?
/*TEST_P(FuseCreateAndOpenFlagsTest, testFlags) {
  ReturnDoesntExistOnLstat(FILENAME);
  EXPECT_CALL(fsimpl, createAndOpenFile(StrEq(FILENAME), OpenFlagsEq(GetParam()), _, _))
    .Times(1).WillOnce(Return(0));
  //For the syscall to succeed, we also need to give an fstat implementation.
  ReturnIsFileOnFstat(0);

  CreateAndOpenFile(FILENAME, GetParam());
}*/
