/**
 * @file Metrics.h
 * @brief Defines the data structures used to store code analysis results.
 *
 * This file contains the structs `FunctionMetric` for individual function data and
 * `FileMetrics` for aggregated, file-level data, including the final calculated score.
 *
 * @author HotspringDev
 * @date 2025-09-14
 */

#ifndef METRICS_H
#define METRICS_H

#include <string>
#include <vector>

/**
 * @struct FunctionMetric
 * @brief Stores analysis metrics for a single function.
 */
struct FunctionMetric {
    std::string name;          ///< The name of the function.
    int line_start = 0;        ///< The starting line number of the function.
    int line_end = 0;          ///< The ending line number of the function.
    int line_count = 0;        ///< Metric: Total lines of code in the function.
    int complexity = 1;        ///< Metric: Cyclomatic Complexity of the function.
};

/**
 * @struct FileMetrics
 * @brief Stores aggregated analysis metrics for a single source file.
 */
struct FileMetrics {
    std::string file_path;      ///< The full path to the analyzed file.
    std::vector<FunctionMetric> functions; ///< A list of all functions found in the file.

    // --- Aggregated raw metrics used for scoring ---
    double avg_function_length = 0.0;     ///< Dimension 1: Average function length.
    double avg_function_complexity = 0.0; ///< Dimension 2: Average cyclomatic complexity.
    int total_lines = 0;                  ///< The total number of lines in the file.
    int comment_lines = 0;                ///< The total number of lines containing comments.
    double comment_coverage_ratio = 0.0;  ///< Dimension 3: Comment coverage percentage.
    int naming_violations = 0;            ///< Dimension 4: Count of poorly named identifiers.

    /**
     * The final calculated score, also known as the Legacy Code Index (LCI) or
     * Shit Mountain Index (SMI). A higher value indicates worse code quality.
     */
    double shit_mountain_index = 0.0;
};

#endif // METRICS_H
