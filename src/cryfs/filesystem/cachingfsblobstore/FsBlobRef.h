#pragma once
#ifndef MESSMER_CRYFS_FILESYSTEM_CACHINGFSBLOBSTORE_FSBLOBREF_H
#define MESSMER_CRYFS_FILESYSTEM_CACHINGFSBLOBSTORE_FSBLOBREF_H

#include "../fsblobstore/FsBlob.h"

namespace cryfs {
namespace cachingfsblobstore {
class CachingFsBlobStore;

//TODO Rename to CachedFsBlob, CachedFileBlob, CachedDirBlob to avoid confusion with parallelaccessfsblobstore
class FsBlobRef {
public:
    virtual ~FsBlobRef();
    virtual const blockstore::Key &key() const = 0;
    virtual off_t lstat_size() const = 0;

    cpputils::unique_ref<fsblobstore::FsBlob> releaseBaseBlob() {
        return std::move(_baseBlob);
    }

protected:
    FsBlobRef(cpputils::unique_ref<fsblobstore::FsBlob> baseBlob, cachingfsblobstore::CachingFsBlobStore *fsBlobStore): _fsBlobStore(fsBlobStore), _baseBlob(std::move(baseBlob)) {}

    fsblobstore::FsBlob *baseBlob() {
        return _baseBlob.get();
    }

private:
    CachingFsBlobStore *_fsBlobStore;
    cpputils::unique_ref<fsblobstore::FsBlob> _baseBlob;

    DISALLOW_COPY_AND_ASSIGN(FsBlobRef);
};

}
}

#endif
