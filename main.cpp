#include <iostream>
#include <boost/program_options.hpp>

#include "md5_hasher.h"
#include "crc32_hasher.h"
#include "comparator.h"
#include "scanner.h"

namespace po = boost::program_options;

int main(const int argc, char* argv[]) {
    try {
        po::options_description desc("bayan options");
        desc.add_options()
            ("help,h", "show help")
            ("dirs,d", po::value<std::vector<std::string>>()->multitoken()->required(), "scan directories")
            ("exclude,e", po::value<std::vector<std::string>>()->multitoken()->default_value({}, ""), "exclude directories")
            ("depth,n", po::value<int>()->default_value(0), "max depth")
            ("min-size,s", po::value<size_t>()->default_value(1), "min file size")
            ("mask,m", po::value<std::vector<std::string>>()->multitoken()->default_value({}, ""), "file masks")
            ("block-size,b", po::value<size_t>()->required(), "block size")
            ("algo,a", po::value<std::string>()->default_value("crc32"), "hash algorithm: crc32 or md5");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);

        if (vm.count("help")) {
            std::cout << desc << std::endl;
            return 0;
        }

        po::notify(vm); // валидация required после проверки help

        auto dir_strings = vm["dirs"].as<std::vector<std::string>>();
        auto ex_strings = vm["exclude"].as<std::vector<std::string>>();
        auto depth = vm["depth"].as<int>();
        auto min_size = vm["min-size"].as<size_t>();
        auto masks = vm["mask"].as<std::vector<std::string>>();
        auto block_size = vm["block-size"].as<size_t>();
        auto algo = vm["algo"].as<std::string>();

        std::vector<fs::path> dirs(dir_strings.begin(), dir_strings.end());
        std::vector<fs::path> excludes(ex_strings.begin(), ex_strings.end());

        std::shared_ptr<IHasher> hasher;
        if (algo == "crc32") {
            hasher = std::make_shared<Crc32Hasher>();
        } else if (algo == "md5") {
            hasher = std::make_shared<Md5Hasher>();
        } else {
            std::cerr << "Unknown algorithm: " << algo << ". Use crc32 or md5." << std::endl;
            return 1;
        }
        Scanner scanner(dirs, excludes, depth, min_size, masks);
        Comparator comparator(block_size, hasher);
        auto filtered_paths = scanner.Run();
        auto duplicates = comparator.Process(filtered_paths);
        for (auto& paths : duplicates) {
            for (auto& path : paths ) {
                std::cout << path << std::endl;
            }
            std::cout << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }



}