//
// Created by evgen on 27.08.2026.
//

#include "comparator.h"

#include <fstream>
#include <unordered_map>
#include <utility>

Comparator::Comparator(const size_t block_size, std::shared_ptr<IHasher> hasher) : block_size_(block_size),
    hasher_(std::move(hasher)) {
}

std::vector<std::vector<fs::path> > Comparator::Process(const std::vector<std::vector<fs::path> > &groups) {
    std::vector<std::vector<fs::path> > result;
    for (auto &group: groups) {
        std::vector<const fs::path *> ptrs;
        for (auto &f: group) ptrs.push_back(&f);

        auto duplicates = ProcessGroup(std::move(ptrs));
        for (auto &dup: duplicates) {
            result.push_back(std::move(dup));
        }
    }
    return result;
}

std::string Comparator::ReadBlock(const fs::path &path, const size_t block_index) {
    auto key = path.string();
    auto it = open_files_.find(key);
    if (it == open_files_.end()) {
        auto stream = std::make_unique<std::ifstream>(path, std::ios::binary);
        if (!stream->is_open()) {
            throw std::runtime_error("Cannot open file: " + key);
        }
        it = open_files_.emplace(key, std::move(stream)).first;
    }

    auto& file = *it->second;

    file.seekg(static_cast<std::streamoff>(block_index * block_size_));

    std::string block(block_size_, '\0');
    file.read(block.data(), static_cast<std::streamsize>(block_size_));

    if (file.bad()) {
        throw std::runtime_error("Read error: " + key);
    }

    return block;
}

// Дожимает одну группу до конца, возвращает группы дубликатов
std::vector<std::vector<fs::path> > Comparator::ProcessGroup(std::vector<const fs::path *> group) {
    std::vector<std::vector<const fs::path *> > sub_groups = {std::move(group)};
    size_t block_index = 0;
    size_t file_blocks = (fs::file_size(*sub_groups[0][0]) + block_size_ - 1) / block_size_;

    while (!sub_groups.empty() && block_index < file_blocks) {
        std::vector<std::vector<const fs::path *> > next_sub_groups;

        for (auto &sg: sub_groups) {
            std::unordered_map<std::string, std::vector<const fs::path *> > split;
            for (auto *file: sg) {
                auto hash = hasher_->Calculate(ReadBlock(*file, block_index).data(), block_size_);
                split[hash].push_back(file);
            }
            for (auto &[hash, files]: split) {
                if (files.size() >= 2) {
                    next_sub_groups.push_back(std::move(files));
                }
            }
        }

        sub_groups = std::move(next_sub_groups);
        block_index++;
        open_files_.clear();
    }

    // конвертируем path* → path
    std::vector<std::vector<fs::path> > result;
    for (auto &sg: sub_groups) {
        std::vector<fs::path> paths;
        for (auto *p: sg) paths.push_back(*p);
        result.push_back(std::move(paths));
    }
    return result;
}
