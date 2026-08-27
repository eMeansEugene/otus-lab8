//
// Created by evgen on 27.08.2026.
//
/**
 * @file scanner.h
 * @brief Сканирование файловой системы и группировка файлов по размеру.
 */

#ifndef BAYAN_SCANNER_H
#define BAYAN_SCANNER_H

#include <filesystem>
#include <unordered_map>
#include <vector>
#include <boost/regex.hpp>

namespace fs = std::filesystem;

/**
 * @class Scanner
 * @brief Обходит указанные директории, фильтрует файлы и группирует их по размеру.
 *
 * Scanner выполняет рекурсивный обход директорий с контролем глубины вложенности,
 * исключением указанных директорий и фильтрацией по маскам имён и минимальному размеру.
 * Результат — вектор групп файлов одинакового размера (кандидаты на дубликаты).
 */
class Scanner {
public:
    /**
     * @brief Конструктор.
     * @param dirs Директории для сканирования.
     * @param excludes Директории, которые следует исключить (полные пути).
     * @param max_depth Максимальная глубина вложенности (0 — только указанная директория).
     * @param min_size Минимальный размер файла в байтах.
     * @param masks Маски имён файлов (glob-формат, регистронезависимые). Пустой вектор — все файлы.
     */
    Scanner(std::vector<fs::path> dirs,
            const std::vector<fs::path>& excludes,
            int max_depth,
            size_t min_size,
            const std::vector<std::string>& masks);

    /**
     * @brief Запустить сканирование.
     * @return Вектор групп файлов. Каждая группа содержит файлы одинакового размера (≥ 2 файла).
     */
    std::vector<std::vector<fs::path>> Run();

private:
    /**
     * @brief Рекурсивный обход одной директории.
     * @param dir Путь к директории.
     * @param current_depth Текущая глубина вложенности.
     */
    void ScanDirectory(const fs::path& dir, int current_depth);

    /**
     * @brief Проверить, соответствует ли имя файла хотя бы одной маске.
     * @param filename Имя файла (без пути).
     * @return true если маски не заданы или имя соответствует хотя бы одной маске.
     */
    bool MatchesMask(const fs::path& filename) const;

    std::vector<fs::path> dirs_;             ///< Директории для сканирования.
    std::vector<fs::path> excludes_;         ///< Исключённые директории (канонические пути).
    int max_depth_;                          ///< Максимальная глубина вложенности.
    size_t min_size_;                        ///< Минимальный размер файла.
    std::vector<boost::regex> compiled_masks_; ///< Скомпилированные regex-маски.

    /// Накопитель: размер файла → список путей.
    std::unordered_map<size_t, std::vector<fs::path>> groups_;
};

#endif // BAYAN_SCANNER_H
