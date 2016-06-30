#include "testutils/FuseLstatReturnTest.h"

using ::testing::WithParamInterface;
using ::testing::Values;

class FuseLstatReturnGidTest: public FuseLstatReturnTest<gid_t>, public WithParamInterface<gid_t> {
private:
  void set(struct stat *stat, gid_t value) override {
    stat->st_gid = value;
  }
};
INSTANTIATE_TEST_CASE_P(FuseLstatReturnGidTest, FuseLstatReturnGidTest, Values(
    0,
    10
));

TEST_P(FuseLstatReturnGidTest, ReturnedFileGidIsCorrect) {
  struct ::stat result = CallFileLstatWithValue(GetParam());
  EXPECT_EQ(GetParam(), result.st_gid);
}

TEST_P(FuseLstatReturnGidTest, ReturnedDirGidIsCorrect) {
  struct ::stat result = CallDirLstatWithValue(GetParam());
  EXPECT_EQ(GetParam(), result.st_gid);
}
