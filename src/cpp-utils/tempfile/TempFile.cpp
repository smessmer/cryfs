#include "TempFile.h"
#include "../logging/logging.h"
#include <fstream>

namespace bf = boost::filesystem;
using std::ofstream;
using namespace cpputils::logging;

namespace cpputils {

TempFile::TempFile(const bf::path &path, bool create)
  : _path(path) {
  if (create) {
    ofstream file(_path.c_str());
    if (!file.good()) {
      throw std::runtime_error("Could not create tempfile");
    }
  }
}

TempFile::TempFile(bool create)
  : TempFile(bf::unique_path(bf::temp_directory_path() / "%%%%-%%%%-%%%%-%%%%"), create) {
}

TempFile::~TempFile() {
  try {
    if (exists()) {
      bf::remove(_path);
    }
  } catch (const boost::filesystem::filesystem_error &e) {
    LOG(ERROR) << "Could not delete tempfile.";
  }
}

bool TempFile::exists() const {
  return bf::exists(_path);
}

const bf::path &TempFile::path() const {
  return _path;
}

}
