//
// Created by sigsegv on 4/25/22.
//

#ifndef FSBITS_FILESYSTEM_H
#define FSBITS_FILESYSTEM_H

#include <memory>
#include <vector>
#include <files/directory.h>
#include <files/fsreference.h>

class blockdev;

enum class filesystem_status {
    SUCCESS,
    IO_ERROR,
    INTEGRITY_ERROR,
    NOT_SUPPORTED_FS_FEATURE,
    INVALID_REQUEST,
    NO_AVAIL_INODES,
    NO_AVAIL_BLOCKS
};

std::string text(filesystem_status status);

template <typename T> struct filesystem_get_node_result {
    T node;
    filesystem_status status;
};

class fsreferrer;

class filesystem {
public:
    virtual filesystem_get_node_result<fsreference<directory>> GetRootDirectory(std::shared_ptr<filesystem> shared_this, const std::shared_ptr<fsreferrer> &referrer) = 0;
};

struct FlushOrCloseResult {
    std::vector<dirty_block> blocks;
    bool completed;
};

class blockdev_filesystem : public filesystem {
protected:
    std::shared_ptr<blockdev> bdev;
public:
    blockdev_filesystem(std::shared_ptr<blockdev> bdev) : bdev(bdev) {
    }
    [[nodiscard]] std::shared_ptr<blockdev> GetBlockdev() const {
        return bdev;
    }
    filesystem_get_node_result<fsreference<directory>> GetRootDirectory(std::shared_ptr<filesystem> shared_this, const std::shared_ptr<fsreferrer> &referrer) override = 0;
    virtual std::vector<std::vector<dirty_block>> GetWrites() = 0;
    virtual std::vector<dirty_block> OpenForWrite() = 0;
    virtual FlushOrCloseResult FlushOrClose() = 0;
};

class filesystem_provider {
public:
    virtual std::string name() const = 0;
};

class special_filesystem_provider : public filesystem_provider {
public:
    virtual std::shared_ptr<filesystem> open() const = 0;
};

class fsresourcelockfactory;

class blockdev_filesystem_provider : public filesystem_provider {
public:
    virtual std::shared_ptr<blockdev_filesystem> open(std::shared_ptr<blockdev> bdev, const std::shared_ptr<fsresourcelockfactory> &fsreslockfactory) const = 0;
};

void init_filesystem_providers();
void add_filesystem_provider(std::shared_ptr<filesystem_provider> provider);
std::vector<std::string> get_filesystem_providers();
std::shared_ptr<filesystem> open_filesystem(std::string provider);
std::shared_ptr<blockdev_filesystem> open_filesystem(const std::shared_ptr<fsresourcelockfactory> &lockfactory, std::string provider, std::shared_ptr<blockdev> bdev);

void register_filesystem_providers();

#endif //FSBITS_FILESYSTEM_H
