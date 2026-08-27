//
// Created by evgen on 27.08.2026.
//

#include "scanner.h"

#include <iostream>
#include <utility>


Scanner::Scanner(std::vector<fs::path> dirs, const std::vector<fs::path> &excludes, const int max_depth,
                 const size_t min_size,
                 const std::vector<std::string> &masks) : dirs_(std::move(dirs)),
                                                          max_depth_(max_depth), min_size_(min_size) {
    for (auto &ex: excludes) {
        excludes_.push_back(fs::canonical(ex));
    }

    for (const auto &mask: masks) {
        std::string pattern;
        for (char c: mask) {
            if (c == '*') pattern += ".*";
            else if (c == '?') pattern += ".";
            else if (c == '.') pattern += "\\.";
            else pattern += c;
        }
        compiled_masks_.emplace_back(pattern, boost::regex::icase);
    }
}

std::vector<std::vector<fs::path> > Scanner::Run() {
    for (const auto &dir: dirs_) {
        ScanDirectory(dir, 0);
    }
    std::vector<std::vector<fs::path>> result;
    for (auto& [size, files] : groups_) {
        if (files.size() >= 2) {
            result.push_back(std::move(files));
        }
    }
    return result;
}

void Scanner::ScanDirectory(const fs::path &dir, const int current_depth) {
    try {
        for (auto &entry : fs::directory_iterator(dir)) {
            if (entry.is_regular_file()) {
                if (const auto sz = entry.file_size(); sz > min_size_ && MatchesMask(entry.path().filename())) {
                    groups_[sz].push_back(entry.path());
                }
            } else if (entry.is_directory() && current_depth < max_depth_) {
                if (auto canonical = fs::canonical(entry.path()); std::find(excludes_.begin(), excludes_.end(),
                                                                            canonical) == excludes_.end()) {
                    ScanDirectory(entry.path(), current_depth + 1);
                                                                            }
            }
        }
    } catch (const fs::filesystem_error &e) {
        std::cerr << "Warning: " << e.what() << std::endl;
    }
}

bool Scanner::MatchesMask(const fs::path &filename) const {
    if (compiled_masks_.empty()) return true;

    const std::string name = filename.string();
    for (const auto &mask: compiled_masks_) {
        if (boost::regex_match(name, mask)) return true;
    }
    return false;
}
