#pragma once
#ifndef MESSMER_FSPP_TEST_FUSE_LSTAT_TESTUTILS_FUSELSTATTEST_H_
#define MESSMER_FSPP_TEST_FUSE_LSTAT_TESTUTILS_FUSELSTATTEST_H_

#include <string>
#include <functional>
#include <sys/stat.h>

#include "../../../testutils/FuseTest.h"

// This class offers some utility functions for testing lstat().
class FuseLstatTest: public FuseTest {
protected:
  const char *FILENAME = "/myfile";

  // Set up a temporary filesystem (using the fsimpl mock in FuseTest as filesystem implementation)
  // and call the lstat syscall on the given (filesystem-relative) path.
  void LstatPath(const std::string &path);
  // Same as LstatPath above, but also return the result of the lstat syscall.
  void LstatPath(const std::string &path, struct stat *result);

  // These two functions are the same as LstatPath above, but they don't fail the test when the lstat syscall
  // crashes. Instead, they return the value of errno after calling ::lstat.
  int LstatPathReturnError(const std::string &path);
  int LstatPathReturnError(const std::string &path, struct stat *result);

  // You can specify an implementation, which can modify the (struct stat *) result,
  // our fuse mock filesystem implementation will then return this to fuse on an lstat call.
  // This functions then set up a temporary filesystem with this mock, call lstat on a filesystem node
  // and return the (struct stat) returned from an lstat syscall to this filesystem.
  struct stat CallLstatWithImpl(std::function<void(struct stat*)> implementation);

  // These two functions are like CallLstatWithImpl, but they also modify the (struct stat).st_mode
  // field, so the node accessed is specified to be a file/directory.
  struct stat CallFileLstatWithImpl(std::function<void(struct stat*)> implementation);
  struct stat CallDirLstatWithImpl(std::function<void(struct stat*)> implementation);

private:

  struct stat CallLstatWithModeAndImpl(mode_t mode, std::function<void(struct stat*)> implementation);
};

#endif
