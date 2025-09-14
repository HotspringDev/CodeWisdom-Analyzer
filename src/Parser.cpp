// src/Parser.cpp (Refactored for higher quality)
#include "Parser.h"
#include <iostream>
#include <map> // Include the map header

extern "C" {
    TSLanguage* tree_sitter_c();
    TSLanguage* tree_sitter_cpp();
    TSLanguage* tree_sitter_python();
    TSLanguage* tree_sitter_java();
    TSLanguage* tree_sitter_rust();
    TSLanguage* tree_sitter_go();
    TSLanguage* tree_sitter_javascript();
    TSLanguage* tree_sitter_typescript();
}

Parser::Parser() : tree(nullptr) {
    parser = ts_parser_new();
}

Parser::~Parser() {
    if (tree) {
        ts_tree_delete(tree);
    }
    ts_parser_delete(parser);
}

// REFACTORED: Replaced an if-chain with a map for lower complexity and better maintainability.
const TSLanguage* Parser::getLanguage(const std::string& language) {
    // A static map is initialized only once and is highly efficient for lookups.
    static const std::map<std::string, TSLanguage*> language_map = {
        {"c", tree_sitter_c()},
        {"cpp", tree_sitter_cpp()},
        {"python", tree_sitter_python()},
        {"java", tree_sitter_java()},
        {"rust", tree_sitter_rust()},
        {"go", tree_sitter_go()},
        {"javascript", tree_sitter_javascript()},
        {"typescript", tree_sitter_typescript()}
    };

    auto it = language_map.find(language);
    if (it != language_map.end()) {
        return it->second;
    }

    return nullptr;
}

bool Parser::parse(const std::string& sourceCode, const std::string& language) {
    const TSLanguage* tsLanguage = getLanguage(language);
    if (tsLanguage == nullptr) {
        std::cerr << "Error: Unsupported language specified: " << language << std::endl;
        return false;
    }

    ts_parser_set_language(parser, tsLanguage);

    if (tree) {
        ts_tree_delete(tree);
    }

    tree = ts_parser_parse_string(
        parser,
        nullptr,
        sourceCode.c_str(),
                                  sourceCode.length()
    );

    return tree != nullptr && !ts_node_has_error(ts_tree_root_node(tree));
}

TSNode Parser::getRootNode() const {
    if (!tree) {
        return TSNode();
    }
    return ts_tree_root_node(tree);
}
