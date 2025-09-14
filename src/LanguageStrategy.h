// src/LanguageStrategy.h
#ifndef LANGUAGE_STRATEGY_H
#define LANGUAGE_STRATEGY_H

#include <string>
#include <vector>
#include <memory>
#include <tree_sitter/api.h>

// Defines the interface for a language-specific analysis strategy.
// This allows the main Analyzer to be language-agnostic.
class LanguageStrategy {
public:
    virtual ~LanguageStrategy() = default;

    // Returns a list of tree-sitter node types that represent a function definition.
    virtual std::vector<std::string> getFunctionDefinitionTypes() const = 0;

    // Extracts the function name from a function definition node.
    virtual std::string extractFunctionName(TSNode functionNode, const std::string& sourceCode) const = 0;

    // Returns a list of node types that increase cyclomatic complexity.
    virtual std::vector<std::string> getComplexityNodeTypes() const = 0;

    // Checks if a node represents a logical operator (e.g., &&, ||, and, or).
    virtual bool isLogicalOperator(TSNode node) const;

    virtual bool isSpecialFunction(TSNode functionNode) const;
};

// --- Concrete Strategy Declarations ---

class CStrategy : public LanguageStrategy {
public:
    std::vector<std::string> getFunctionDefinitionTypes() const override;
    std::string extractFunctionName(TSNode functionNode, const std::string& sourceCode) const override;
    std::vector<std::string> getComplexityNodeTypes() const override;
    // C++ specific logic for special functions.
    bool isSpecialFunction(TSNode functionNode) const override;
};

// C++ strategy inherits most of its logic from the C strategy.
class CppStrategy : public CStrategy {};

class PythonStrategy : public LanguageStrategy {
public:
    std::vector<std::string> getFunctionDefinitionTypes() const override;
    std::string extractFunctionName(TSNode functionNode, const std::string& sourceCode) const override;
    std::vector<std::string> getComplexityNodeTypes() const override;
    bool isLogicalOperator(TSNode node) const override;

};

class JavaStrategy : public LanguageStrategy {
public:
    std::vector<std::string> getFunctionDefinitionTypes() const override;
    std::string extractFunctionName(TSNode functionNode, const std::string& sourceCode) const override;
    std::vector<std::string> getComplexityNodeTypes() const override;
};

class RustStrategy : public LanguageStrategy {
public:
    std::vector<std::string> getFunctionDefinitionTypes() const override;
    std::string extractFunctionName(TSNode functionNode, const std::string& sourceCode) const override;
    std::vector<std::string> getComplexityNodeTypes() const override;
};

class GoStrategy : public LanguageStrategy {
public:
    std::vector<std::string> getFunctionDefinitionTypes() const override;
    std::string extractFunctionName(TSNode functionNode, const std::string& sourceCode) const override;
    std::vector<std::string> getComplexityNodeTypes() const override;
};

class JSStrategy : public LanguageStrategy {
public:
    std::vector<std::string> getFunctionDefinitionTypes() const override;
    std::string extractFunctionName(TSNode functionNode, const std::string& sourceCode) const override;
    std::vector<std::string> getComplexityNodeTypes() const override;
};

// TypeScript strategy inherits its logic from the JavaScript strategy.
class TSStrategy : public JSStrategy {};


// Factory function to create the appropriate strategy based on the language name.
std::unique_ptr<LanguageStrategy> createStrategy(const std::string& language);

#endif // LANGUAGE_STRATEGY_H
