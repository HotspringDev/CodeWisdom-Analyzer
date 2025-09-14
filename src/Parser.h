// src/Parser.h
#ifndef PARSER_H
#define PARSER_H

#include <string>
#include "tree_sitter/api.h"

// A wrapper class for the tree-sitter parsing library.
// It handles parser initialization, language loading, and parsing source code.
class Parser {
public:
    Parser();
    ~Parser();

    // Parses a given string of source code for a specified language.
    // @param sourceCode The source code to parse.
    // @param language A string identifier for the language (e.g., "cpp", "python").
    // @return True if parsing was successful, false otherwise.
    bool parse(const std::string& sourceCode, const std::string& language);

    // Retrieves the root node of the last successfully parsed syntax tree.
    // @return The root TSNode of the syntax tree.
    TSNode getRootNode() const;

private:
    TSParser* parser;
    TSTree* tree;

    // Retrieves the TSLanguage object for a given language identifier.
    const TSLanguage* getLanguage(const std::string& language);
};

#endif // PARSER_H
