//
// Created by evgen on 27.08.2026.
//

/**
 * @file comparator.h
 * @brief Ленивое блочное сравнение файлов для поиска дубликатов.
 */

#ifndef BAYAN_COMPARATOR_H
#define BAYAN_COMPARATOR_H

#include <filesystem>
#include <memory>
#include <fstream>
#include <unordered_map>
#include <vector>

#include "i_hasher.h"

namespace fs = std::filesystem;

/**
 * @class Comparator
 * @brief Определяет дубликаты среди групп файлов-кандидатов путём поблочного сравнения хэшей.
 *
 * Для каждой группы файлов одинакового размера последовательно читаются блоки,
 * вычисляются хэши и группа дробится на подгруппы. Файлы, совпавшие по всем блокам
 * до конца — дубликаты. Каждый блок каждого файла читается с диска не более одного раза.
 */
class Comparator {
public:
    /**
     * @brief Конструктор.
     * @param block_size Размер блока чтения в байтах. Последний блок дополняется нулями.
     * @param hasher Указатель на реализацию алгоритма хэширования.
     */
    Comparator(size_t block_size, std::shared_ptr<IHasher> hasher);

    /**
     * @brief Найти дубликаты среди групп файлов-кандидатов.
     * @param groups Группы файлов одинакового размера (от Scanner::Run).
     * @return Группы файлов-дубликатов (≥ 2 файла в каждой группе).
     */
    std::vector<std::vector<fs::path>> Process(const std::vector<std::vector<fs::path>>& groups);

private:
    size_t block_size_;                  ///< Размер блока чтения.
    std::shared_ptr<IHasher> hasher_;    ///< Алгоритм хэширования.
    std::unordered_map<std::string, std::unique_ptr<std::ifstream>> open_files_; ///< Открытые файлы

    /**
     * @brief Прочитать один блок файла.
     * @param path Путь к файлу.
     * @param block_index Номер блока (0-based).
     * @return Строка из block_size_ байт (последний блок дополнен нулями).
     */
    std::string ReadBlock(const fs::path& path, size_t block_index);

    /**
     * @brief Дожать одну группу файлов до конца — определить дубликаты внутри неё.
     * @param group Указатели на пути файлов-кандидатов.
     * @return Группы дубликатов (может быть несколько, если группа распалась).
     */
    std::vector<std::vector<fs::path>> ProcessGroup(std::vector<const fs::path*> group);
};

#endif // BAYAN_COMPARATOR_H