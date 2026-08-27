//
// Created by evgen on 27.08.2026.
//

/**
 * @file i_hasher.h
 * @brief Интерфейс алгоритма хэширования блоков.
 */

#ifndef BAYAN_I_HASHER_H
#define BAYAN_I_HASHER_H

#include <string>

/**
 * @class IHasher
 * @brief Абстрактный интерфейс для вычисления хэша блока данных.
 *
 * Реализации: Crc32Hasher, Md5Hasher.
 * Возвращаемое значение — std::string из сырых байт (4 байта для CRC32, 16 для MD5).
 */
class IHasher {
public:
    virtual ~IHasher() = default;

    /**
     * @brief Вычислить хэш блока данных.
     * @param data Указатель на данные.
     * @param sz Размер данных в байтах.
     * @return Хэш в виде строки сырых байт.
     */
    virtual std::string Calculate(const char* data, size_t sz) = 0;
};

#endif //BAYAN_I_HASHER_H