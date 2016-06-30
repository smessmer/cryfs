#include "testutils/BlobStoreTest.h"
#include <cpp-utils/data/Data.h>
#include <cpp-utils/data/DataFixture.h>
#include "blobstore/implementations/onblocks/datanodestore/DataNodeView.h"

using cpputils::unique_ref;
using ::testing::WithParamInterface;
using ::testing::Values;

using namespace blobstore;
using blobstore::onblocks::datanodestore::DataNodeLayout;
using blockstore::Key;
using cpputils::Data;
using cpputils::DataFixture;

class BlobReadWriteTest: public BlobStoreTest {
public:
  static constexpr uint32_t LARGE_SIZE = 10 * 1024 * 1024;
  static constexpr DataNodeLayout LAYOUT = DataNodeLayout(BLOCKSIZE_BYTES);

  BlobReadWriteTest()
    :randomData(DataFixture::generate(LARGE_SIZE)),
     blob(blobStore->create()) {
  }

  Data readBlob(const Blob &blob) {
    Data data(blob.size());
    blob.read(data.data(), 0, data.size());
    return data;
  }

  template<class DataClass>
  void EXPECT_DATA_READS_AS(const DataClass &expected, const Blob &actual, uint64_t offset, uint64_t size) {
  	Data read(size);
  	actual.read(read.data(), offset, size);
    EXPECT_EQ(0, std::memcmp(expected.data(), read.data(), size));
  }

  Data randomData;
  unique_ref<Blob> blob;
};
constexpr uint32_t BlobReadWriteTest::LARGE_SIZE;
constexpr DataNodeLayout BlobReadWriteTest::LAYOUT;

TEST_F(BlobReadWriteTest, WritingImmediatelyFlushes_SmallSize) {
	blob->resize(5);
	blob->write(randomData.data(), 0, 5);
	auto loaded = loadBlob(blob->key());
	EXPECT_DATA_READS_AS(randomData, *loaded, 0, 5);
}

TEST_F(BlobReadWriteTest, WritingImmediatelyFlushes_LargeSize) {
	blob->resize(LARGE_SIZE);
	blob->write(randomData.data(), 0, LARGE_SIZE);
	auto loaded = loadBlob(blob->key());
	EXPECT_DATA_READS_AS(randomData, *loaded, 0, LARGE_SIZE);
}

// Regression test for a strange bug we had
TEST_F(BlobReadWriteTest, WritingCloseTo16ByteLimitDoesntDestroySize) {
  blob->resize(1);
  blob->write(randomData.data(), 32776, 4);
  EXPECT_EQ(32780u, blob->size());
}

struct DataRange {
  size_t blobsize;
  off_t offset;
  size_t count;
};
class BlobReadWriteDataTest: public BlobReadWriteTest, public WithParamInterface<DataRange> {
public:
  Data foregroundData;
  Data backgroundData;

  BlobReadWriteDataTest()
    : foregroundData(DataFixture::generate(GetParam().count, 0)),
      backgroundData(DataFixture::generate(GetParam().blobsize, 1)) {
  }

  template<class DataClass>
  void EXPECT_DATA_READS_AS_OUTSIDE_OF(const DataClass &expected, const Blob &blob, off_t start, size_t count) {
    Data begin(start);
    Data end(GetParam().blobsize - count - start);

    std::memcpy(begin.data(), expected.data(), start);
    std::memcpy(end.data(), (uint8_t*)expected.data()+start+count, end.size());

    EXPECT_DATA_READS_AS(begin, blob, 0, start);
    EXPECT_DATA_READS_AS(end, blob, start + count, end.size());
  }

  void EXPECT_DATA_IS_ZEROES_OUTSIDE_OF(const Blob &blob, off_t start, size_t count) {
    Data ZEROES(GetParam().blobsize);
    ZEROES.FillWithZeroes();
    EXPECT_DATA_READS_AS_OUTSIDE_OF(ZEROES, blob, start, count);
  }
};
INSTANTIATE_TEST_CASE_P(BlobReadWriteDataTest, BlobReadWriteDataTest, Values(
  //Blob with only one leaf
  DataRange{BlobReadWriteDataTest::LAYOUT.maxBytesPerLeaf(),     0,   BlobReadWriteDataTest::LAYOUT.maxBytesPerLeaf()},     // full size leaf, access beginning to end
  DataRange{BlobReadWriteDataTest::LAYOUT.maxBytesPerLeaf(),     100, BlobReadWriteDataTest::LAYOUT.maxBytesPerLeaf()-200}, // full size leaf, access middle to middle
  DataRange{BlobReadWriteDataTest::LAYOUT.maxBytesPerLeaf(),     0,   BlobReadWriteDataTest::LAYOUT.maxBytesPerLeaf()-100}, // full size leaf, access beginning to middle
  DataRange{BlobReadWriteDataTest::LAYOUT.maxBytesPerLeaf(),     100, BlobReadWriteDataTest::LAYOUT.maxBytesPerLeaf()-100}, // full size leaf, access middle to end
  DataRange{BlobReadWriteDataTest::LAYOUT.maxBytesPerLeaf()-100, 0,   BlobReadWriteDataTest::LAYOUT.maxBytesPerLeaf()-100}, // non-full size leaf, access beginning to end
  DataRange{BlobReadWriteDataTest::LAYOUT.maxBytesPerLeaf()-100, 100, BlobReadWriteDataTest::LAYOUT.maxBytesPerLeaf()-300}, // non-full size leaf, access middle to middle
  DataRange{BlobReadWriteDataTest::LAYOUT.maxBytesPerLeaf()-100, 0,   BlobReadWriteDataTest::LAYOUT.maxBytesPerLeaf()-200}, // non-full size leaf, access beginning to middle
  DataRange{BlobReadWriteDataTest::LAYOUT.maxBytesPerLeaf()-100, 100, BlobReadWriteDataTest::LAYOUT.maxBytesPerLeaf()-200},  // non-full size leaf, access middle to end
  //Larger blob
  DataRange{BlobReadWriteDataTest::LARGE_SIZE,     0,   BlobReadWriteDataTest::LARGE_SIZE},     // access beginning to end
  DataRange{BlobReadWriteDataTest::LARGE_SIZE,     100, BlobReadWriteDataTest::LARGE_SIZE-200}, // access middle first leaf to middle last leaf
  DataRange{BlobReadWriteDataTest::LARGE_SIZE,     0,   BlobReadWriteDataTest::LARGE_SIZE-100}, // access beginning to middle last leaf
  DataRange{BlobReadWriteDataTest::LARGE_SIZE,     100, BlobReadWriteDataTest::LARGE_SIZE-100}, // access middle first leaf to end
  DataRange{BlobReadWriteDataTest::LARGE_SIZE,     BlobReadWriteDataTest::LARGE_SIZE*1/3, BlobReadWriteDataTest::LARGE_SIZE*1/3}, // access middle to middle
  DataRange{BlobReadWriteDataTest::LARGE_SIZE,     0,   BlobReadWriteDataTest::LARGE_SIZE*2/3}, // access beginning to middle
  DataRange{BlobReadWriteDataTest::LARGE_SIZE,     BlobReadWriteDataTest::LARGE_SIZE*1/3, BlobReadWriteDataTest::LARGE_SIZE*2/3} // access middle to end
));

TEST_P(BlobReadWriteDataTest, WritingDoesntChangeSize) {
  blob->resize(GetParam().blobsize);
  blob->write(this->foregroundData.data(), GetParam().offset, GetParam().count);
  EXPECT_EQ(GetParam().blobsize, blob->size());
}

TEST_P(BlobReadWriteDataTest, WriteAndReadImmediately) {
  blob->resize(GetParam().blobsize);
  blob->write(this->foregroundData.data(), GetParam().offset, GetParam().count);

  EXPECT_DATA_READS_AS(this->foregroundData, *blob, GetParam().offset, GetParam().count);
  EXPECT_DATA_IS_ZEROES_OUTSIDE_OF(*blob, GetParam().offset, GetParam().count);
}

TEST_P(BlobReadWriteDataTest, WriteAndReadAfterLoading) {
  blob->resize(GetParam().blobsize);
  blob->write(this->foregroundData.data(), GetParam().offset, GetParam().count);
  auto loaded = loadBlob(blob->key());

  EXPECT_DATA_READS_AS(this->foregroundData, *loaded, GetParam().offset, GetParam().count);
  EXPECT_DATA_IS_ZEROES_OUTSIDE_OF(*loaded, GetParam().offset, GetParam().count);
}

TEST_P(BlobReadWriteDataTest, OverwriteAndRead) {
  blob->resize(GetParam().blobsize);
  blob->write(this->backgroundData.data(), 0, GetParam().blobsize);
  blob->write(this->foregroundData.data(), GetParam().offset, GetParam().count);
  EXPECT_DATA_READS_AS(this->foregroundData, *blob, GetParam().offset, GetParam().count);
  EXPECT_DATA_READS_AS_OUTSIDE_OF(this->backgroundData, *blob, GetParam().offset, GetParam().count);
}

TEST_P(BlobReadWriteDataTest, WriteWholeAndReadPart) {
  blob->resize(GetParam().blobsize);
  blob->write(this->backgroundData.data(), 0, GetParam().blobsize);
  Data read(GetParam().count);
  blob->read(read.data(), GetParam().offset, GetParam().count);
  EXPECT_EQ(0, std::memcmp(read.data(), (uint8_t*)this->backgroundData.data()+GetParam().offset, GetParam().count));
}

TEST_P(BlobReadWriteDataTest, WritePartAndReadWhole) {
  blob->resize(GetParam().blobsize);
  blob->write(this->backgroundData.data(), 0, GetParam().blobsize);
  blob->write(this->foregroundData.data(), GetParam().offset, GetParam().count);
  Data read = readBlob(*blob);
  EXPECT_EQ(0, std::memcmp(read.data(), this->backgroundData.data(), GetParam().offset));
  EXPECT_EQ(0, std::memcmp((uint8_t*)read.data()+GetParam().offset, this->foregroundData.data(), GetParam().count));
  EXPECT_EQ(0, std::memcmp((uint8_t*)read.data()+GetParam().offset+GetParam().count, (uint8_t*)this->backgroundData.data()+GetParam().offset+GetParam().count, GetParam().blobsize-GetParam().count-GetParam().offset));
}
