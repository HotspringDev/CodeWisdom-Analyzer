/**
 * @file Analyzer.cpp
 * @brief Implements the core code analysis engine and the scoring model.
 *
 * This file contains the implementation of the Analyzer class and all concrete
 * LanguageStrategy classes. It is responsible for the actual metric extraction
 * and the logic for calculating the final Shit Mountain Index (SMI).
 *
 * @author HotspringDev
 * @date 2025-09-14
 */

#include "Analyzer.h"
#include "LanguageStrategy.h"
#include <cstring>
#include <algorithm>
#include <vector>
#include <memory>
#include <numeric>
#include <string>
#include <set>

// --- Helper Functions ---

/**
 * @brief Extracts the source text corresponding to a given tree-sitter node.
 */
static std::string getNodeText(TSNode node, const std::string &source)
{
    if (ts_node_is_null(node))
        return "";
    uint32_t start = ts_node_start_byte(node);
    uint32_t end = ts_node_end_byte(node);
    return source.substr(start, end - start);
}

/**
 * @brief Recursively searches a node and its children for the first `identifier` node.
 * This is a robust way to find function names within complex declarator nodes in C/C++.
 */
static TSNode findIdentifierRecursive(TSNode node)
{
    if (strcmp(ts_node_type(node), "identifier") == 0)
    {
        return node;
    }
    for (uint32_t i = 0; i < ts_node_child_count(node); ++i)
    {
        TSNode result = findIdentifierRecursive(ts_node_child(node, i));
        if (!ts_node_is_null(result))
        {
            return result;
        }
    }
    return TSNode();
}

/**
 * @brief Recursively searches for a child node of a specific type.
 * This is far more robust than checking direct children only.
 */
static bool hasChildOfTypeRecursive(TSNode node, const char *type)
{
    if (strcmp(ts_node_type(node), type) == 0)
    {
        return true;
    }
    for (uint32_t i = 0; i < ts_node_child_count(node); ++i)
    {
        if (hasChildOfTypeRecursive(ts_node_child(node, i), type))
        {
            return true;
        }
    }
    return false;
}

// ======================================================
// Language Strategy Implementations
// ======================================================

bool LanguageStrategy::isLogicalOperator(TSNode node) const
{
    if (strcmp(ts_node_type(node), "binary_expression") == 0)
    {
        TSNode op = ts_node_child_by_field_name(node, "operator", 8);
        if (!ts_node_is_null(op))
        {
            const char *opType = ts_node_type(op);
            return strcmp(opType, "&&") == 0 || strcmp(opType, "||") == 0;
        }
    }
    return false;
}

// Default implementation: by default, no functions are considered special.
bool LanguageStrategy::isSpecialFunction(TSNode functionNode) const
{
    return false;
}

// --- C/C++ Strategy ---
bool CStrategy::isSpecialFunction(TSNode functionNode) const
{
    TSNode declarator = ts_node_child_by_field_name(functionNode, "declarator", 10);
    if (ts_node_is_null(declarator))
    {
        declarator = ts_node_child_by_field_name(functionNode, "abstract_declarator", 19);
    }
    if (ts_node_is_null(declarator))
    {
        return false;
    }

    // Use the new recursive helper to reliably find special nodes, no matter how deeply nested.
    return hasChildOfTypeRecursive(declarator, "operator_name") ||
           hasChildOfTypeRecursive(declarator, "destructor_name");
}

std::vector<std::string> CStrategy::getFunctionDefinitionTypes() const { return {"function_definition"}; }
std::string CStrategy::extractFunctionName(TSNode node, const std::string &source) const
{
    TSNode declaratorNode = ts_node_child_by_field_name(node, "declarator", 10);
    if (!ts_node_is_null(declaratorNode))
    {
        TSNode identifierNode = findIdentifierRecursive(declaratorNode);
        if (!ts_node_is_null(identifierNode))
        {
            return getNodeText(identifierNode, source);
        }
    }
    return "[extraction_failed]";
}
std::vector<std::string> CStrategy::getComplexityNodeTypes() const
{
    return {"if_statement", "for_statement", "while_statement", "do_statement", "case_statement", "catch_clause", "conditional_expression"};
}

// --- Python Strategy ---
std::vector<std::string> PythonStrategy::getFunctionDefinitionTypes() const { return {"function_definition"}; }
std::string PythonStrategy::extractFunctionName(TSNode node, const std::string &source) const { return getNodeText(ts_node_child_by_field_name(node, "name", 4), source); }
std::vector<std::string> PythonStrategy::getComplexityNodeTypes() const { return {"if_statement", "for_statement", "while_statement", "except_clause", "conditional_expression", "elif_clause"}; }
bool PythonStrategy::isLogicalOperator(TSNode node) const { return strcmp(ts_node_type(node), "boolean_operator") == 0; }

// --- Java Strategy ---
std::vector<std::string> JavaStrategy::getFunctionDefinitionTypes() const { return {"method_declaration", "constructor_declaration"}; }
std::string JavaStrategy::extractFunctionName(TSNode node, const std::string &source) const { return getNodeText(ts_node_child_by_field_name(node, "name", 4), source); }
std::vector<std::string> JavaStrategy::getComplexityNodeTypes() const { return {"if_statement", "for_statement", "while_statement", "do_statement", "switch_expression", "catch_clause", "ternary_expression"}; }

// --- Rust Strategy ---
std::vector<std::string> RustStrategy::getFunctionDefinitionTypes() const { return {"function_item"}; }
std::string RustStrategy::extractFunctionName(TSNode node, const std::string &source) const { return getNodeText(ts_node_child_by_field_name(node, "name", 4), source); }
std::vector<std::string> RustStrategy::getComplexityNodeTypes() const { return {"if_expression", "for_expression", "while_expression", "match_arm", "loop_expression"}; }

// --- Go Strategy ---
std::vector<std::string> GoStrategy::getFunctionDefinitionTypes() const { return {"function_declaration", "method_declaration"}; }
std::string GoStrategy::extractFunctionName(TSNode node, const std::string &source) const { return getNodeText(ts_node_child_by_field_name(node, "name", 4), source); }
std::vector<std::string> GoStrategy::getComplexityNodeTypes() const { return {"if_statement", "for_statement", "switch_statement", "select_statement"}; }

// --- JavaScript / TypeScript Strategy ---
std::vector<std::string> JSStrategy::getFunctionDefinitionTypes() const { return {"function_declaration", "function", "arrow_function", "method_definition"}; }
std::string JSStrategy::extractFunctionName(TSNode node, const std::string &source) const
{
    const char *type = ts_node_type(node);
    TSNode nameNode = ts_node_child_by_field_name(node, "name", 4);
    if (!ts_node_is_null(nameNode))
        return getNodeText(nameNode, source);
    if (strcmp(type, "arrow_function") == 0)
    {
        TSNode parent = ts_node_parent(node);
        if (parent.id && (strcmp(ts_node_type(parent), "variable_declarator") == 0))
        {
            return getNodeText(ts_node_child_by_field_name(parent, "name", 4), source);
        }
    }
    return "[anonymous function]";
}
std::vector<std::string> JSStrategy::getComplexityNodeTypes() const { return {"if_statement", "for_statement", "for_in_statement", "while_statement", "do_statement", "switch_case", "catch_clause", "ternary_expression"}; }

// --- Strategy Factory ---
std::unique_ptr<LanguageStrategy> createStrategy(const std::string &language)
{
    if (language == "c")
        return std::make_unique<CStrategy>();
    if (language == "cpp")
        return std::make_unique<CppStrategy>();
    if (language == "python")
        return std::make_unique<PythonStrategy>();
    if (language == "java")
        return std::make_unique<JavaStrategy>();
    if (language == "rust")
        return std::make_unique<RustStrategy>();
    if (language == "go")
        return std::make_unique<GoStrategy>();
    if (language == "javascript")
        return std::make_unique<JSStrategy>();
    if (language == "typescript")
        return std::make_unique<TSStrategy>();
    return nullptr;
}

// ======================================================
// Analyzer Implementation
// ======================================================

Analyzer::Analyzer(std::unique_ptr<LanguageStrategy> strategy) : langStrategy(std::move(strategy)) {}

FileMetrics Analyzer::analyze(TSNode rootNode, const std::string &filePath, const std::string &sourceCode)
{
    FileMetrics metrics;
    metrics.file_path = filePath;
    if (langStrategy)
    {
        analyzeFunctions(rootNode, metrics, sourceCode);
        analyzeFileWideMetrics(rootNode, metrics);
        analyzeNaming(rootNode, metrics, sourceCode);
    }
    calculateFinalScore(metrics);
    return metrics;
}

void Analyzer::analyzeFunctions(TSNode node, FileMetrics &metrics, const std::string &sourceCode)
{
    const auto &funcTypes = langStrategy->getFunctionDefinitionTypes();
    const char *nodeType = ts_node_type(node);
    if (std::find(funcTypes.begin(), funcTypes.end(), std::string(nodeType)) != funcTypes.end())
    {
        if (!langStrategy->isSpecialFunction(node))
        {
            analyzeSingleFunction(node, metrics, sourceCode);
        }
        return;
    }
    for (uint32_t i = 0; i < ts_node_child_count(node); ++i)
    {
        analyzeFunctions(ts_node_child(node, i), metrics, sourceCode);
    }
}

void Analyzer::analyzeFileWideMetrics(TSNode node, FileMetrics &metrics)
{
    if (ts_node_is_null(ts_node_parent(node)))
    {
        metrics.total_lines = ts_node_end_point(node).row + 1;
    }
    if (strcmp(ts_node_type(node), "comment") == 0)
    {
        metrics.comment_lines += (ts_node_end_point(node).row - ts_node_start_point(node).row + 1);
    }
    for (uint32_t i = 0; i < ts_node_child_count(node); ++i)
    {
        analyzeFileWideMetrics(ts_node_child(node, i), metrics);
    }
}

void Analyzer::analyzeNaming(TSNode node, FileMetrics &metrics, const std::string &sourceCode)
{
    const char *type = ts_node_type(node);
    if (strcmp(type, "identifier") == 0)
    {
        std::string name = getNodeText(node, sourceCode);
        if (name.length() <= 2)
        {
            static const std::set<std::string> whitelist = {
                "i", "j", "k", "x", "y", "z", "os", "fs", "it", "c", "ts", "js"};
            if (whitelist.find(name) == whitelist.end())
            {
                metrics.naming_violations++;
            }
        }
    }
    for (uint32_t i = 0; i < ts_node_child_count(node); ++i)
    {
        analyzeNaming(ts_node_child(node, i), metrics, sourceCode);
    }
}

void Analyzer::analyzeSingleFunction(TSNode funcNode, FileMetrics &metrics, const std::string &sourceCode)
{
    FunctionMetric func;
    func.line_start = ts_node_start_point(funcNode).row + 1;
    func.line_end = ts_node_end_point(funcNode).row + 1;
    func.line_count = func.line_end - func.line_start + 1;
    func.name = langStrategy->extractFunctionName(funcNode, sourceCode);
    if (func.name.empty())
        func.name = "[anonymous/unknown]";
    func.complexity = calculateComplexity(funcNode);
    metrics.functions.push_back(func);
}

int Analyzer::calculateComplexity(TSNode node)
{
    int complexity = 0;
    const char *type = ts_node_type(node);
    const auto &complexityTypes = langStrategy->getComplexityNodeTypes();
    if (std::find(complexityTypes.begin(), complexityTypes.end(), std::string(type)) != complexityTypes.end())
    {
        complexity = 1;
    }
    else if (langStrategy->isLogicalOperator(node))
    {
        complexity = 1;
    }
    for (uint32_t i = 0; i < ts_node_child_count(node); ++i)
    {
        complexity += calculateComplexity(ts_node_child(node, i));
    }
    const auto &funcTypes = langStrategy->getFunctionDefinitionTypes();
    if (std::find(funcTypes.begin(), funcTypes.end(), std::string(type)) != funcTypes.end())
    {
        return complexity + 1;
    }
    return complexity;
}

void Analyzer::calculateFinalScore(FileMetrics &metrics)
{
    // --- Step 1: Calculate raw aggregated metrics (unchanged) ---
    if (!metrics.functions.empty())
    {
        double total_length = 0, total_complexity = 0;
        for (const auto &func : metrics.functions)
        {
            total_length += func.line_count;
            total_complexity += func.complexity;
        }
        metrics.avg_function_length = total_length / metrics.functions.size();
        metrics.avg_function_complexity = total_complexity / metrics.functions.size();
    }
    if (metrics.total_lines > 0)
    {
        metrics.comment_coverage_ratio = (double)metrics.comment_lines / metrics.total_lines * 100.0;
    }

    // --- Step 2: Calculate 0-100 quality scores for each dimension ---
    double naming_score = std::max(0.0, 100.0 - (double)metrics.naming_violations * 5.0);

    // --- Step 3: Apply the correct scoring model based on context ---

    // MODEL A: For files with no analyzable functions (e.g., header files, interfaces)
    if (metrics.functions.empty())
    {
        // For headers, high comment coverage is ALWAYS good. We use a simple linear score.
        // A ratio of 30% or more gets a perfect score. This rewards well-documented headers.
        double comment_score = std::min(100.0, (metrics.comment_coverage_ratio / 30.0) * 100.0);

        // Quality is determined only by comments and naming.
        double total_quality_score = (comment_score * 0.7) + (naming_score * 0.3);
        metrics.shit_mountain_index = 100.0 - total_quality_score;
        return;
    }

    // MODEL B: For files WITH analyzable functions (e.g., source files)
    // Complexity Score (bell curve, 1 is best)
    double complexity_score = std::max(0.0, 100.0 - (metrics.avg_function_complexity - 1.0) / (20.0 - 1.0) * 100.0);

    // Length Score (bell curve, 10 is best)
    double length_score = std::max(0.0, 100.0 - (metrics.avg_function_length - 10.0) / (100.0 - 10.0) * 100.0);

    // Comment Score (bell curve, 15% is ideal)
    double comment_score = std::max(0.0, 100.0 - std::abs(metrics.comment_coverage_ratio - 15.0) / 15.0 * 100.0);

    // --- Step 4: Calculate final weighted score for Model B ---
    const double COMPLEXITY_WEIGHT = 0.50;
    const double LENGTH_WEIGHT = 0.15;
    const double COMMENT_WEIGHT = 0.15;
    const double NAMING_WEIGHT = 0.20;

    double total_quality_score = (complexity_score * COMPLEXITY_WEIGHT) +
                                 (length_score * LENGTH_WEIGHT) +
                                 (comment_score * COMMENT_WEIGHT) +
                                 (naming_score * NAMING_WEIGHT);

    metrics.shit_mountain_index = 100.0 - total_quality_score;
}
