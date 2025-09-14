/**
 * @file main.cpp
 * @brief The main entry point for the CodeWisdom Analyzer.
 *
 * This file handles command-line argument parsing, file system traversal,
 * orchestrating the analysis of each file, and printing the final ranked report.
 *
 * @author HotspringDev
 * @date 2025-09-14
 */

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <map>

#include "Parser.h"
#include "Analyzer.h"
#include "Metrics.h"
#include "TerminalColor.h"

namespace fs = std::filesystem;
using namespace TerminalColor;

/**
 * @brief Helper function to check if a string ends with a specific suffix.
 */
bool ends_with(const std::string& value, const std::string& ending) {
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

/**
 * @brief Prints the formatted, color-coded analysis report for a single file.
 * This version provides a simplified report for files without analyzable functions.
 */
void printReport(const FileMetrics& metrics) {
    const Color RESET = Color::RESET, RED = Color::RED, GREEN = Color::GREEN,
                YELLOW = Color::YELLOW, CYAN = Color::CYAN, WHITE = Color::WHITE;
    
    Color smi_color = GREEN;
    if (metrics.shit_mountain_index > 40) smi_color = YELLOW;
    if (metrics.shit_mountain_index > 60) smi_color = RED;

    std::cout << WHITE << "======================================================\n" << RESET;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "  Analysis Report for: " << CYAN << metrics.file_path << RESET << "\n";
    std::cout << "  Shit Mountain Index (SMI): " << smi_color << metrics.shit_mountain_index << RESET << " (Higher is worse)\n";
    std::cout << WHITE << "------------------------------------------------------\n" << RESET;

    if (metrics.functions.empty()) {
        std::cout << "  (No analyzable functions found in this file)\n";
        std::cout << "  Comment Coverage:          " << YELLOW << metrics.comment_coverage_ratio << "%" << RESET << " (" << metrics.comment_lines << "/" << metrics.total_lines << " lines)\n";
        std::cout << "  Naming Violations:         " << YELLOW << metrics.naming_violations << RESET << " found\n";
    } else {
        std::cout << "  Avg Function Length:       " << YELLOW << metrics.avg_function_length << RESET << " lines\n";
        std::cout << "  Avg Cyclomatic Complexity: " << YELLOW << metrics.avg_function_complexity << RESET << "\n";
        std::cout << "  Comment Coverage:          " << YELLOW << metrics.comment_coverage_ratio << "%" << RESET << " (" << metrics.comment_lines << "/" << metrics.total_lines << " lines)\n";
        std::cout << "  Naming Violations:         " << YELLOW << metrics.naming_violations << RESET << " found\n";
        std::cout << WHITE << "------------------------------------------------------\n" << RESET;
        std::cout << "Found " << metrics.functions.size() << " functions:\n\n";
        for (const auto& func : metrics.functions) {
            std::cout << "  - Function: " << YELLOW << func.name << RESET << "\n";
            std::cout << "    - Length: " << func.line_count << ", Complexity: " << func.complexity << "\n";
        }
    }
    std::cout << "\n\n";
}

/**
 * @brief Determines the programming language from a file's extension.
 * Correctly handles C++ header files (.h, .hpp).
 */
std::string getLanguageFromFile(const std::string& filePath) {
    auto const pos = filePath.find_last_of('.');
    if (pos == std::string::npos) return "unsupported";
    std::string ext = filePath.substr(pos);

    static const std::map<std::string, std::string> extension_map = {
        {".cpp", "cpp"}, {".hpp", "cpp"}, {".h", "cpp"}, {".cc", "cpp"}, {".cxx", "cpp"},
        {".c", "c"},
        {".py", "python"},
        {".java", "java"},
        {".rs", "rust"},
        {".go", "go"},
        {".js", "javascript"},
        {".ts", "typescript"}
    };

    auto it = extension_map.find(ext);
    return (it != extension_map.end()) ? it->second : "unsupported";
}

/**
 * @brief Reads, parses, and analyzes a single source file.
 */
FileMetrics analyzeFile(const std::string& filePath) {
    std::string language = getLanguageFromFile(filePath);
    if (language == "unsupported") return FileMetrics();

    std::cout << ".";

    std::ifstream file(filePath);
    if (!file.is_open()) return FileMetrics();
    std::string sourceCode((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    Parser parser;
    if (parser.parse(sourceCode, language)) {
        TSNode root = parser.getRootNode();
        auto strategy = createStrategy(language);
        if (!strategy) return FileMetrics();

        Analyzer analyzer(std::move(strategy));
        return analyzer.analyze(root, filePath, sourceCode);
    } else {
        std::cerr << "\n[Warning] Failed to parse: " << filePath << std::endl;
    }
    return FileMetrics();
}

/**
 * @brief Main function.
 */
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <path_to_source_file_or_directory>" << std::endl;
        return 1;
    }

    std::string path = argv[1];
    if (!fs::exists(path)) {
        std::cerr << "Error: Path does not exist: " << path << std::endl;
        return 1;
    }
    
    std::vector<FileMetrics> all_metrics;
    std::cout << "Analyzing files, please wait...";

    if (fs::is_directory(path)) {
        for (const auto& entry : fs::recursive_directory_iterator(path)) {
            if (entry.is_regular_file()) {
                FileMetrics result = analyzeFile(entry.path().string());
                if (!result.file_path.empty()) all_metrics.push_back(result);
            }
        }
    } else if (fs::is_regular_file(path)) {
        FileMetrics result = analyzeFile(path);
        if (!result.file_path.empty()) all_metrics.push_back(result);
    }
    std::cout << "\nAnalysis complete.\n\n";

    std::sort(all_metrics.begin(), all_metrics.end(), [](const FileMetrics& a, const FileMetrics& b) {
        return a.shit_mountain_index > b.shit_mountain_index;
    });
    
    std::cout << Color::WHITE << "=============== PROJECT ANALYSIS RANKING (WORST FILES FIRST) ===============\n\n" << Color::RESET;
    for (const auto& metrics : all_metrics) {
        printReport(metrics);
    }

    return 0;
}
