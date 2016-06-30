#include "testutils/FuseLstatReturnTest.h"

using ::testing::WithParamInterface;
using ::testing::Values;

class FuseLstatReturnModeTest: public FuseLstatTest, public WithParamInterface<mode_t> {
public:
  struct stat CallLstatWithValue(mode_t mode) {
    return CallLstatWithImpl([mode] (struct stat *stat) {
      stat->st_mode = mode;
    });
  }
};
INSTANTIATE_TEST_CASE_P(FuseLstatReturnModeTest, FuseLstatReturnModeTest, Values(
    S_IFREG,
    S_IFDIR,
    S_IFREG | S_IRUSR | S_IWGRP | S_IXOTH, // a file with some access bits set
    S_IFDIR | S_IWUSR | S_IXGRP | S_IROTH  // a dir with some access bits set
));

TEST_P(FuseLstatReturnModeTest, ReturnedModeIsCorrect) {
  struct ::stat result = CallLstatWithValue(GetParam());
  EXPECT_EQ(GetParam(), result.st_mode);
}
