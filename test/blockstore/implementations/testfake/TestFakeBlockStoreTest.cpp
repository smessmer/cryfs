#include "blockstore/implementations/testfake/FakeBlock.h"
#include "blockstore/implementations/testfake/FakeBlockStore.h"
#include "../../testutils/BlockStoreTest.h"
#include "../../testutils/BlockStoreWithRandomKeysTest.h"
#include <gtest/gtest.h>
#include <cpp-utils/pointer/unique_ref_boost_optional_gtest_workaround.h>

using blockstore::BlockStore;
using blockstore::BlockStoreWithRandomKeys;
using blockstore::testfake::FakeBlockStore;
using cpputils::unique_ref;
using cpputils::make_unique_ref;

class FakeBlockStoreTestFixture: public BlockStoreTestFixture {
public:
  unique_ref<BlockStore> createBlockStore() override {
    return make_unique_ref<FakeBlockStore>();
  }
};

INSTANTIATE_TYPED_TEST_CASE_P(TestFake, BlockStoreTest, FakeBlockStoreTestFixture);

class FakeBlockStoreWithRandomKeysTestFixture: public BlockStoreWithRandomKeysTestFixture {
public:
  unique_ref<BlockStoreWithRandomKeys> createBlockStore() override {
    return make_unique_ref<FakeBlockStore>();
  }
};

INSTANTIATE_TYPED_TEST_CASE_P(TestFake, BlockStoreWithRandomKeysTest, FakeBlockStoreWithRandomKeysTestFixture);
