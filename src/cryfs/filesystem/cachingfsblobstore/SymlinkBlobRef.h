#pragma once
#ifndef MESSMER_CRYFS_FILESYSTEM_CACHINGFSBLOBSTORE_SYMLINKBLOBREF_H
#define MESSMER_CRYFS_FILESYSTEM_CACHINGFSBLOBSTORE_SYMLINKBLOBREF_H

#include "FsBlobRef.h"
#include "../fsblobstore/SymlinkBlob.h"

namespace cryfs {
namespace cachingfsblobstore {

class SymlinkBlobRef final: public FsBlobRef {
public:
    SymlinkBlobRef(cpputils::unique_ref<fsblobstore::SymlinkBlob> base, CachingFsBlobStore *fsBlobStore)
        :FsBlobRef(std::move(base), fsBlobStore),
        _base(dynamic_cast<fsblobstore::SymlinkBlob*>(baseBlob())) {
        ASSERT(_base != nullptr, "We just initialized this with a pointer to SymlinkBlob. Can't be something else now.");
    }

    const boost::filesystem::path &target() const {
        return _base->target();
    }

    const blockstore::Key &key() const {
        return _base->key();
    }

    off_t lstat_size() const {
        return _base->lstat_size();
    }

private:

    fsblobstore::SymlinkBlob *_base;

    DISALLOW_COPY_AND_ASSIGN(SymlinkBlobRef);
};

}
}

#endif
