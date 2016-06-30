#pragma once
#ifndef MESSMER_FSPP_FSTEST_FSPPFILETEST_H_
#define MESSMER_FSPP_FSTEST_FSPPFILETEST_H_

#include <sys/fcntl.h>
#include <sys/stat.h>

#include "testutils/FileTest.h"

template<class ConcreteFileSystemTestFixture>
class FsppFileTest: public FileTest<ConcreteFileSystemTestFixture> {
public:
  void Test_Open_RDONLY(fspp::File *file) {
    file->open(O_RDONLY);
  }

  void Test_Open_WRONLY(fspp::File *file) {
    file->open(O_WRONLY);
  }

  void Test_Open_RDWR(fspp::File *file) {
    file->open(O_RDONLY);
  }

  void Test_Truncate_DontChange1(fspp::File *file) {
	file->truncate(0);
	this->EXPECT_SIZE(0, file);
  }

  void Test_Truncate_GrowTo1(fspp::File *file) {
	file->truncate(1);
	this->EXPECT_SIZE(1, file);
  }

  void Test_Truncate_Grow(fspp::File *file) {
	file->truncate(10*1024*1024);
	this->EXPECT_SIZE(10*1024*1024, file);
  }

  void Test_Truncate_DontChange2(fspp::File *file) {
	file->truncate(10*1024*1024);
	file->truncate(10*1024*1024);
	this->EXPECT_SIZE(10*1024*1024, file);
  }

  void Test_Truncate_Shrink(fspp::File *file) {
    file->truncate(10*1024*1024);
    file->truncate(5*1024*1024);
    this->EXPECT_SIZE(5*1024*1024, file);
  }

  void Test_Truncate_ShrinkTo0(fspp::File *file) {
	file->truncate(10*1024*1024);
	file->truncate(0);
	this->EXPECT_SIZE(0, file);
  }

  void Test_Chown_Uid(fspp::File *file) {
      file->chown(100, 200);
      this->IN_STAT(file, [] (struct stat st){
          EXPECT_EQ(100u, st.st_uid);
      });
  }

  void Test_Chown_Gid(fspp::File *file) {
    file->chown(100, 200);
    this->IN_STAT(file, [] (struct stat st){
        EXPECT_EQ(200u, st.st_gid);
    });
  }

  void Test_Chmod(fspp::File *file) {
    file->chmod(S_IFREG | S_IRUSR | S_IWOTH);
    this->IN_STAT(file, [] (struct stat st){
        EXPECT_EQ((mode_t)(S_IFREG | S_IRUSR | S_IWOTH), st.st_mode);
    });
  }

  void Test_Utimens(fspp::File *file) {
    struct timespec ATIME; ATIME.tv_sec = 1458086400; ATIME.tv_nsec = 34525;
    struct timespec MTIME; MTIME.tv_sec = 1458086300; MTIME.tv_nsec = 48293;
    file->utimens(ATIME, MTIME);
    this->IN_STAT(file, [this, ATIME, MTIME] (struct stat st) {
        this->EXPECT_ATIME_EQ(ATIME, st);
        this->EXPECT_MTIME_EQ(MTIME, st);
    });
  }
};

TYPED_TEST_CASE_P(FsppFileTest);

TYPED_TEST_P(FsppFileTest, Open_RDONLY) {
  this->Test_Open_RDONLY(this->file_root.get());
}

TYPED_TEST_P(FsppFileTest, Open_RDONLY_Nested) {
  this->Test_Open_RDONLY(this->file_nested.get());
}

TYPED_TEST_P(FsppFileTest, Open_WRONLY) {
  this->Test_Open_WRONLY(this->file_root.get());
}

TYPED_TEST_P(FsppFileTest, Open_WRONLY_Nested) {
  this->Test_Open_WRONLY(this->file_nested.get());
}

TYPED_TEST_P(FsppFileTest, Open_RDWR) {
  this->Test_Open_RDWR(this->file_root.get());
}

TYPED_TEST_P(FsppFileTest, Open_RDWR_Nested) {
  this->Test_Open_RDWR(this->file_nested.get());
}

TYPED_TEST_P(FsppFileTest, Truncate_DontChange1) {
  this->Test_Truncate_DontChange1(this->file_root.get());
}

TYPED_TEST_P(FsppFileTest, Truncate_DontChange1_Nested) {
  this->Test_Truncate_DontChange1(this->file_nested.get());
}

TYPED_TEST_P(FsppFileTest, Truncate_GrowTo1) {
  this->Test_Truncate_GrowTo1(this->file_root.get());
}

TYPED_TEST_P(FsppFileTest, Truncate_GrowTo1_Nested) {
  this->Test_Truncate_GrowTo1(this->file_nested.get());
}

TYPED_TEST_P(FsppFileTest, Truncate_Grow) {
  this->Test_Truncate_Grow(this->file_root.get());
}

TYPED_TEST_P(FsppFileTest, Truncate_Grow_Nested) {
  this->Test_Truncate_Grow(this->file_nested.get());
}

TYPED_TEST_P(FsppFileTest, Truncate_DontChange2) {
  this->Test_Truncate_DontChange2(this->file_root.get());
}

TYPED_TEST_P(FsppFileTest, Truncate_DontChange2_Nested) {
  this->Test_Truncate_DontChange2(this->file_nested.get());
}

TYPED_TEST_P(FsppFileTest, Truncate_Shrink) {
  this->Test_Truncate_Shrink(this->file_root.get());
}

TYPED_TEST_P(FsppFileTest, Truncate_Shrink_Nested) {
  this->Test_Truncate_Shrink(this->file_nested.get());
}

TYPED_TEST_P(FsppFileTest, Truncate_ShrinkTo0) {
  this->Test_Truncate_ShrinkTo0(this->file_root.get());
}

TYPED_TEST_P(FsppFileTest, Truncate_ShrinkTo0_Nested) {
  this->Test_Truncate_ShrinkTo0(this->file_nested.get());
}

TYPED_TEST_P(FsppFileTest, Chown_Uid) {
    this->Test_Chown_Uid(this->file_root.get());
}

TYPED_TEST_P(FsppFileTest, Chown_Uid_Nested) {
    this->Test_Chown_Uid(this->file_nested.get());
}

TYPED_TEST_P(FsppFileTest, Chown_Gid) {
    this->Test_Chown_Gid(this->file_root.get());
}

TYPED_TEST_P(FsppFileTest, Chown_Gid_Nested) {
    this->Test_Chown_Gid(this->file_nested.get());
}

TYPED_TEST_P(FsppFileTest, Chmod) {
    this->Test_Chmod(this->file_root.get());
}

TYPED_TEST_P(FsppFileTest, Chmod_Nested) {
    this->Test_Chmod(this->file_nested.get());
}

TYPED_TEST_P(FsppFileTest, Utimens) {
    this->Test_Utimens(this->file_root.get());
}

TYPED_TEST_P(FsppFileTest, Utimens_Nested) {
    this->Test_Utimens(this->file_nested.get());
}

REGISTER_TYPED_TEST_CASE_P(FsppFileTest,
  Open_RDONLY,
  Open_RDONLY_Nested,
  Open_WRONLY,
  Open_WRONLY_Nested,
  Open_RDWR,
  Open_RDWR_Nested,
  Truncate_DontChange1,
  Truncate_DontChange1_Nested,
  Truncate_GrowTo1,
  Truncate_GrowTo1_Nested,
  Truncate_Grow,
  Truncate_Grow_Nested,
  Truncate_DontChange2,
  Truncate_DontChange2_Nested,
  Truncate_Shrink,
  Truncate_Shrink_Nested,
  Truncate_ShrinkTo0,
  Truncate_ShrinkTo0_Nested,
  Chown_Uid,
  Chown_Uid_Nested,
  Chown_Gid,
  Chown_Gid_Nested,
  Chmod,
  Chmod_Nested,
  Utimens,
  Utimens_Nested
);

//TODO access
//TODO unlink
//TODO Test all operations do (or don't) affect file timestamps correctly (including rename, which shouldn't modify access/modify time, but inode change time)

//TODO Move applicable test cases to new instances of FsppNodeTest (like FsppNodeTest_Rename) (e.g. utimens, chmod, ...)
//TODO access
//TODO utimens
//TODO chmod
//TODO chown

#endif
