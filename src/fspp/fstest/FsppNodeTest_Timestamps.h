#pragma once
#ifndef MESSMER_FSPP_FSTEST_FSPPNODETEST_TIMESTAMPS_H_
#define MESSMER_FSPP_FSTEST_FSPPNODETEST_TIMESTAMPS_H_

#include "testutils/FsppNodeTest.h"
#include "../fuse/FuseErrnoException.h"
#include "testutils/TimestampTestUtils.h"
#include <cpp-utils/system/stat.h>

using namespace cpputils::time;
using std::function;

template<class ConcreteFileSystemTestFixture>
class FsppNodeTest_Timestamps: public FsppNodeTest<ConcreteFileSystemTestFixture>, public TimestampTestUtils {
public:

    void Test_Create() {
        timespec lowerBound = now();
        auto node = this->CreateNode("/mynode");
        timespec upperBound = now();
        EXPECT_ACCESS_TIMESTAMP_BETWEEN        (lowerBound, upperBound, *node);
        EXPECT_MODIFICATION_TIMESTAMP_BETWEEN  (lowerBound, upperBound, *node);
        EXPECT_METADATACHANGE_TIMESTAMP_BETWEEN(lowerBound, upperBound, *node);
    }

    void Test_Stat() {
        auto node = this->CreateNode("/mynode");
        auto operation = [&node] () {
            struct stat st;
            node->stat(&st);
        };
        this->EXPECT_OPERATION_UPDATES_TIMESTAMPS_AS(*node, operation, {ExpectDoesntUpdateAnyTimestamps});
    }

    void Test_Chmod() {
        auto node = this->CreateNode("/mynode");
        mode_t mode = stat(*node).st_mode;
        auto operation = [&node, mode] () {
            node->chmod(mode);
        };
        this->EXPECT_OPERATION_UPDATES_TIMESTAMPS_AS(*node, operation, {ExpectDoesntUpdateAccessTimestamp, ExpectDoesntUpdateModificationTimestamp, ExpectUpdatesMetadataTimestamp});
    }

    void Test_Chown() {
        auto node = this->CreateNode("/mynode");
        uid_t uid = stat(*node).st_uid;
        gid_t gid = stat(*node).st_gid;
        auto operation = [&node, uid, gid] () {
            node->chown(uid, gid);
        };
        this->EXPECT_OPERATION_UPDATES_TIMESTAMPS_AS(*node, operation, {ExpectDoesntUpdateAccessTimestamp, ExpectDoesntUpdateModificationTimestamp, ExpectUpdatesMetadataTimestamp});
    }

    void Test_Access() {
        auto node = this->CreateNode("/mynode");
        auto operation = [&node] () {
            node->access(0);
        };
        this->EXPECT_OPERATION_UPDATES_TIMESTAMPS_AS(*node, operation, {ExpectDoesntUpdateAnyTimestamps});
    }

    void Test_Rename_Error_TargetParentDirDoesntExist() {
        auto node = this->CreateNode("/oldname");
        auto operation = [&node] () {
            try {
                node->rename("/notexistingdir/newname");
                EXPECT_TRUE(false); // expect rename to fail
            } catch (const fspp::fuse::FuseErrnoException &e) {
                EXPECT_EQ(ENOENT, e.getErrno()); //Rename fails, everything is ok.
            }
        };
        this->EXPECT_OPERATION_UPDATES_TIMESTAMPS_AS(*node, operation, {ExpectDoesntUpdateAnyTimestamps});
    }

    void Test_Rename_Error_TargetParentDirIsFile() {
        auto node = this->CreateNode("/oldname");
        this->CreateFile("/somefile");
        auto operation = [&node] () {
            try {
                node->rename("/somefile/newname");
                EXPECT_TRUE(false); // expect rename to fail
            } catch (const fspp::fuse::FuseErrnoException &e) {
                EXPECT_EQ(ENOTDIR, e.getErrno()); //Rename fails, everything is ok.
            }
        };
        this->EXPECT_OPERATION_UPDATES_TIMESTAMPS_AS(*node, operation, {ExpectDoesntUpdateAnyTimestamps});
    }

    void Test_Rename_Error_RootDir() {
        // TODO Re-enable this test once the root dir stores timestamps correctly
        /*
        auto root = this->LoadDir("/");
        auto operation = [&root] () {
            try {
                root->rename("/newname");
                EXPECT_TRUE(false); // expect throws
            } catch (const fspp::fuse::FuseErrnoException &e) {
                EXPECT_EQ(EBUSY, e.getErrno()); //Rename fails, everything is ok.
            }
        };
        this->EXPECT_OPERATION_UPDATES_TIMESTAMPS_AS(*node, operation, {ExpectDoesntUpdateAnyTimestamps});
         */
    }

    void Test_Rename_InRoot() {
        auto node = this->CreateNode("/oldname");
        auto operation = [&node] () {
            node->rename("/newname");
        };
        this->EXPECT_OPERATION_UPDATES_TIMESTAMPS_AS(*node, operation, {ExpectDoesntUpdateAccessTimestamp, ExpectDoesntUpdateModificationTimestamp, ExpectUpdatesMetadataTimestamp});
    }

    void Test_Rename_InNested() {
        this->CreateDir("/mydir");
        auto node = this->CreateNode("/mydir/oldname");
        auto operation = [&node] () {
            node->rename("/mydir/newname");
        };
        this->EXPECT_OPERATION_UPDATES_TIMESTAMPS_AS(*node, operation, {ExpectDoesntUpdateAccessTimestamp, ExpectDoesntUpdateModificationTimestamp, ExpectUpdatesMetadataTimestamp});
    }

    void Test_Rename_RootToNested_SameName() {
        this->CreateDir("/mydir");
        auto node = this->CreateNode("/oldname");
        auto operation = [&node] () {
            node->rename("/mydir/oldname");
        };
        this->EXPECT_OPERATION_UPDATES_TIMESTAMPS_AS(*node, operation, {ExpectDoesntUpdateAccessTimestamp, ExpectDoesntUpdateModificationTimestamp, ExpectUpdatesMetadataTimestamp});
    }

    void Test_Rename_RootToNested_NewName() {
        this->CreateDir("/mydir");
        auto node = this->CreateNode("/oldname");
        auto operation = [&node] () {
            node->rename("/mydir/newname");
        };
        this->EXPECT_OPERATION_UPDATES_TIMESTAMPS_AS(*node, operation, {ExpectDoesntUpdateAccessTimestamp, ExpectDoesntUpdateModificationTimestamp, ExpectUpdatesMetadataTimestamp});
    }

    void Test_Rename_NestedToRoot_SameName() {
        this->CreateDir("/mydir");
        auto node = this->CreateNode("/mydir/oldname");
        auto operation = [&node] () {
            node->rename("/oldname");
        };
        this->EXPECT_OPERATION_UPDATES_TIMESTAMPS_AS(*node, operation, {ExpectDoesntUpdateAccessTimestamp, ExpectDoesntUpdateModificationTimestamp, ExpectUpdatesMetadataTimestamp});
    }

    void Test_Rename_NestedToRoot_NewName() {
        this->CreateDir("/mydir");
        auto node = this->CreateNode("/mydir/oldname");
        auto operation = [&node] () {
            node->rename("/newname");
        };
        this->EXPECT_OPERATION_UPDATES_TIMESTAMPS_AS(*node, operation, {ExpectDoesntUpdateAccessTimestamp, ExpectDoesntUpdateModificationTimestamp, ExpectUpdatesMetadataTimestamp});
    }

    void Test_Rename_NestedToNested_SameName() {
        this->CreateDir("/mydir1");
        this->CreateDir("/mydir2");
        auto node = this->CreateNode("/mydir1/oldname");
        auto operation = [&node] () {
            node->rename("/mydir2/oldname");
        };
        this->EXPECT_OPERATION_UPDATES_TIMESTAMPS_AS(*node, operation, {ExpectDoesntUpdateAccessTimestamp, ExpectDoesntUpdateModificationTimestamp, ExpectUpdatesMetadataTimestamp});
    }

    void Test_Rename_NestedToNested_NewName() {
        this->CreateDir("/mydir1");
        this->CreateDir("/mydir2");
        auto node = this->CreateNode("/mydir1/oldname");
        auto operation = [&node] () {
            node->rename("/mydir2/newname");
        };
        this->EXPECT_OPERATION_UPDATES_TIMESTAMPS_AS(*node, operation, {ExpectDoesntUpdateAccessTimestamp, ExpectDoesntUpdateModificationTimestamp, ExpectUpdatesMetadataTimestamp});
    }

    void Test_Rename_ToItself() {
        auto node = this->CreateNode("/oldname");
        auto operation = [&node] () {
            node->rename("/oldname");
        };
        this->EXPECT_OPERATION_UPDATES_TIMESTAMPS_AS(*node, operation, {ExpectDoesntUpdateAccessTimestamp, ExpectDoesntUpdateModificationTimestamp, ExpectUpdatesMetadataTimestamp});
    }

    void Test_Rename_Overwrite_InSameDir() {
        auto node = this->CreateNode("/oldname");
        this->CreateNode("/newname");
        auto operation = [&node] () {
            node->rename("/newname");
        };
        this->EXPECT_OPERATION_UPDATES_TIMESTAMPS_AS(*node, operation, {ExpectDoesntUpdateAccessTimestamp, ExpectDoesntUpdateModificationTimestamp, ExpectUpdatesMetadataTimestamp});
    }

    void Test_Rename_Overwrite_InDifferentDir() {
        this->CreateDir("/mydir1");
        this->CreateDir("/mydir2");
        this->CreateNode("/mydir2/newname");
        auto node = this->CreateNode("/mydir1/oldname");
        auto operation = [&node] () {
            node->rename("/mydir2/newname");
        };
        this->EXPECT_OPERATION_UPDATES_TIMESTAMPS_AS(*node, operation, {ExpectDoesntUpdateAccessTimestamp, ExpectDoesntUpdateModificationTimestamp, ExpectUpdatesMetadataTimestamp});
    }

    void Test_Rename_Overwrite_Error_DirWithFile_InSameDir() {
        auto node = this->CreateFile("/oldname");
        this->CreateDir("/newname");
        auto operation = [&node] () {
            try {
                node->rename("/newname");
                EXPECT_TRUE(false); // expect rename to fail
            } catch (const fspp::fuse::FuseErrnoException &e) {
                EXPECT_EQ(EISDIR, e.getErrno()); //Rename fails, everything is ok.
            }
        };
        this->EXPECT_OPERATION_UPDATES_TIMESTAMPS_AS(*node, operation, {ExpectDoesntUpdateAnyTimestamps});
    }

    void Test_Rename_Overwrite_Error_DirWithFile_InDifferentDir() {
        this->CreateDir("/mydir1");
        this->CreateDir("/mydir2");
        auto node = this->CreateFile("/mydir1/oldname");
        this->CreateDir("/mydir2/newname");
        auto operation = [&node] () {
            try {
                node->rename("/mydir2/newname");
                EXPECT_TRUE(false); // expect rename to fail
            } catch (const fspp::fuse::FuseErrnoException &e) {
                EXPECT_EQ(EISDIR, e.getErrno());//Rename fails, everything is ok.
            }
        };
        this->EXPECT_OPERATION_UPDATES_TIMESTAMPS_AS(*node, operation, {ExpectDoesntUpdateAnyTimestamps});
    }

    void Test_Rename_Overwrite_Error_FileWithDir_InSameDir() {
        auto node = this->CreateDir("/oldname");
        this->CreateFile("/newname");
        auto operation = [&node] () {
            try {
                node->rename("/newname");
                EXPECT_TRUE(false); // expect rename to fail
            } catch (const fspp::fuse::FuseErrnoException &e) {
                EXPECT_EQ(ENOTDIR, e.getErrno()); //Rename fails, everything is ok.
            }
        };
        this->EXPECT_OPERATION_UPDATES_TIMESTAMPS_AS(*node, operation, {ExpectDoesntUpdateAnyTimestamps});
    }

    void Test_Rename_Overwrite_Error_FileWithDir_InDifferentDir() {
        this->CreateDir("/mydir1");
        this->CreateDir("/mydir2");
        auto node = this->CreateDir("/mydir1/oldname");
        this->CreateFile("/mydir2/newname");
        auto operation = [&node] () {
            try {
                node->rename("/mydir2/newname");
                EXPECT_TRUE(false); // expect rename to fail
            } catch (const fspp::fuse::FuseErrnoException &e) {
                EXPECT_EQ(ENOTDIR, e.getErrno()); //Rename fails, everything is ok.
            }
        };
        this->EXPECT_OPERATION_UPDATES_TIMESTAMPS_AS(*node, operation, {ExpectDoesntUpdateAnyTimestamps});
    }

    void Test_Utimens() {
        auto node = this->CreateNode("/mynode");
        timespec atime = xSecondsAgo(100);
        timespec mtime = xSecondsAgo(200);
        auto operation = [&node, atime, mtime] () {
            node->utimens(atime, mtime);
        };
        this->EXPECT_OPERATION_UPDATES_TIMESTAMPS_AS(*node, operation, {ExpectUpdatesMetadataTimestamp});
        EXPECT_EQ(atime, stat(*node).st_atim);
        EXPECT_EQ(mtime, stat(*node).st_mtim);
    }
};

REGISTER_NODE_TEST_CASE(FsppNodeTest_Timestamps,
    Create,
    Stat,
    Chmod,
    Chown,
    Access,
    Rename_Error_TargetParentDirDoesntExist,
    Rename_Error_TargetParentDirIsFile,
    Rename_Error_RootDir,
    Rename_InRoot,
    Rename_InNested,
    Rename_RootToNested_SameName,
    Rename_RootToNested_NewName,
    Rename_NestedToRoot_SameName,
    Rename_NestedToRoot_NewName,
    Rename_NestedToNested_SameName,
    Rename_NestedToNested_NewName,
    Rename_ToItself,
    Rename_Overwrite_InSameDir,
    Rename_Overwrite_InDifferentDir,
    Rename_Overwrite_Error_DirWithFile_InSameDir,
    Rename_Overwrite_Error_DirWithFile_InDifferentDir,
    Rename_Overwrite_Error_FileWithDir_InSameDir,
    Rename_Overwrite_Error_FileWithDir_InDifferentDir,
    Utimens
);

#endif
