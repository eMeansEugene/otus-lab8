//
// Created by evgen on 27.08.2026.
//

/**
* @file crc32_hasher.h
* @brief Реализация IHasher на основе CRC32 (Boost.CRC).
*/

#ifndef BAYAN_CRC32_HASHER_H
#define BAYAN_CRC32_HASHER_H

#include "i_hasher.h"
#include <boost/crc.hpp>

/**
* @class Crc32Hasher
* @brief Вычисляет CRC32-хэш блока данных.
*
* Возвращает 4 байта (uint32_t) в виде std::string.
*/
class Crc32Hasher : public IHasher {
public:
    /// @copydoc IHasher::Calculate
    std::string Calculate(const char* data, size_t sz) override {
        boost::crc_32_type crc;
        crc.process_bytes(data, sz);
        const auto result = crc.checksum();
        return {reinterpret_cast<const char*>(&result), sizeof(result)};
    }
};


#endif //BAYAN_CRC32_HASHER_H
