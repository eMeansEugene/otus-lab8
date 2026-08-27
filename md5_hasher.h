//
// Created by evgen on 27.08.2026.
//

/**
 * @file md5_hasher.h
 * @brief Реализация IHasher на основе MD5 (Boost.UUID).
 */

#ifndef BAYAN_MD5_HASHER_H
#define BAYAN_MD5_HASHER_H

#include "i_hasher.h"
#include <boost/uuid/detail/md5.hpp>

/**
 * @class Md5Hasher
 * @brief Вычисляет MD5-хэш блока данных.
 *
 * Возвращает 16 байт (128 бит) в виде std::string.
 * Использует boost::uuids::detail::md5 — формально internal API,
 * но широко применяется в учебных проектах.
 */
class Md5Hasher : public IHasher {
public:
    /// @copydoc IHasher::Calculate
    std::string Calculate(const char* data, size_t sz) override {
        boost::uuids::detail::md5 md5;
        md5.process_bytes(data, sz);
        boost::uuids::detail::md5::digest_type digest;
        md5.get_digest(digest);
        return {reinterpret_cast<const char*>(digest), sizeof(digest)};
    }
};

#endif // BAYAN_MD5_HASHER_H
