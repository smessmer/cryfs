#include "../testutils/FuseTest.h"

using namespace fspp::fuse;
using namespace fspp::fuse;

using ::testing::_;
using ::testing::StrEq;

typedef FuseTest BasicFuseTest;

//This test case simply checks whether a filesystem can be setup and teardown without crashing.
TEST_F(BasicFuseTest, setupAndTearDown) {
  auto fs = TestFS();
}
