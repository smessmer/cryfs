#include "testutils/FuseReadDirTest.h"

using ::testing::_;
using ::testing::StrEq;
using ::testing::Return;

using std::vector;
using std::string;

class FuseReadDirDirnameTest: public FuseReadDirTest {
public:
};

TEST_F(FuseReadDirDirnameTest, ReadRootDir) {
  EXPECT_CALL(fsimpl, readDir(StrEq("/")))
    .Times(1).WillOnce(ReturnDirEntries({}));

  ReadDir("/");
}

TEST_F(FuseReadDirDirnameTest, ReadDir) {
  ReturnIsDirOnLstat("/mydir");
  EXPECT_CALL(fsimpl, readDir(StrEq("/mydir")))
    .Times(1).WillOnce(ReturnDirEntries({}));

  ReadDir("/mydir");
}

TEST_F(FuseReadDirDirnameTest, ReadDirNested) {
  ReturnIsDirOnLstat("/mydir");
  ReturnIsDirOnLstat("/mydir/mydir2");
  EXPECT_CALL(fsimpl, readDir(StrEq("/mydir/mydir2")))
    .Times(1).WillOnce(ReturnDirEntries({}));

  ReadDir("/mydir/mydir2");
}

TEST_F(FuseReadDirDirnameTest, ReadDirNested2) {
  ReturnIsDirOnLstat("/mydir");
  ReturnIsDirOnLstat("/mydir/mydir2");
  ReturnIsDirOnLstat("/mydir/mydir2/mydir3");
  EXPECT_CALL(fsimpl, readDir(StrEq("/mydir/mydir2/mydir3")))
    .Times(1).WillOnce(ReturnDirEntries({}));

  ReadDir("/mydir/mydir2/mydir3");
}
