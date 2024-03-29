//
// Created by sigsegv on 10/5/23.
//

#ifndef FSBITS_EXT2FS_FILE_H
#define FSBITS_EXT2FS_FILE_H

#include <memory>
#include <files/fileitem.h>
#include <files/fsreferrer.h>
#include <files/fsreference.h>
#include <files/fsresource.h>
#include <filesystems/filesystem.h>

class ext2fs;
class ext2fs_inode;

class ext2fs_file : public fileitem, public fsreferrer, public fsresource<ext2fs_file> {
    friend ext2fs;
protected:
    std::shared_ptr<ext2fs> fs;
    fsreference<ext2fs_inode> inode;
    ext2fs_file * GetResource() override;
public:
    constexpr static fileitem_status Convert(filesystem_status ino_stat) {
        fileitem_status status{fileitem_status::SUCCESS};
        switch (ino_stat) {
            case filesystem_status::SUCCESS:
                status = fileitem_status::SUCCESS;
                break;
            case filesystem_status::IO_ERROR:
                status = fileitem_status::IO_ERROR;
                break;
            case filesystem_status::INTEGRITY_ERROR:
                status = fileitem_status::INTEGRITY_ERROR;
                break;
            case filesystem_status::NOT_SUPPORTED_FS_FEATURE:
                status = fileitem_status::NOT_SUPPORTED_FS_FEATURE;
                break;
            case filesystem_status::INVALID_REQUEST:
                status = fileitem_status::INVALID_REQUEST;
                break;
            default:
                status = fileitem_status::IO_ERROR;
        }
        return status;
    }
    ext2fs_file(std::shared_ptr<ext2fs> fs, fsresourcelockfactory &lockfactory);
protected:
    virtual void Init(const std::shared_ptr<ext2fs_file> &self_ref, fsresource<ext2fs_inode> &inode);
public:
    std::string GetReferrerIdentifier() override;
    uint32_t Mode() override;
    std::size_t Size() override;
    uintptr_t InodeNum() override;
    uint32_t BlockSize() override;
    uintptr_t SysDevId() override;
    file_getpage_result GetPage(std::size_t pagenum) override;
    file_read_result Read(uint64_t offset, void *ptr, std::size_t length) override;
};

#endif //FSBITS_EXT2FS_FILE_H
