// src/Analyzer.h (Extended with new dimensions)
#ifndef ANALYZER_H
#define ANALYZER_H

#include "Metrics.h"
#include "LanguageStrategy.h"
#include <tree_sitter/api.h>
#include <string>
#include <memory>

class Analyzer {
public:
    Analyzer(std::unique_ptr<LanguageStrategy> strategy);
    FileMetrics analyze(TSNode rootNode, const std::string& filePath, const std::string& sourceCode);

private:
    std::unique_ptr<LanguageStrategy> langStrategy;

    // --- Traversal and Analysis Methods ---
    // Finds and analyzes all functions.
    void analyzeFunctions(TSNode node, FileMetrics& metrics, const std::string& sourceCode);
    void analyzeSingleFunction(TSNode funcNode, FileMetrics& metrics, const std::string& sourceCode);

    // Analyzes file-wide metrics like comments and total lines.
    void analyzeFileWideMetrics(TSNode node, FileMetrics& metrics);

    // Finds and analyzes all identifiers for naming conventions.
    void analyzeNaming(TSNode node, FileMetrics& metrics, const std::string& sourceCode);

    // --- Metric Calculation Methods ---
    int calculateComplexity(TSNode node);
    void calculateFinalScore(FileMetrics& metrics);
};

#endif // ANALYZER_H
