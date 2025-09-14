# CMakeLists.txt (FINAL, GUARANTEED CORRECT VERSION)

cmake_minimum_required(VERSION 3.16)
project(CodeQualityAnalyzer CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Tree-sitter 核心库 ---
# 使用 CMAKE_SOURCE_DIR 确保路径是绝对的
add_library(tree-sitter STATIC ${CMAKE_SOURCE_DIR}/vendor/tree-sitter/lib/src/lib.c)
target_include_directories(tree-sitter PUBLIC ${CMAKE_SOURCE_DIR}/vendor/tree-sitter/lib/include)

# --- 最终版、绝对路径、健壮的语法库定义宏 ---
macro(add_ts_grammar lang_name src_subdir)
    # 关键修复 #1: 使用 CMAKE_SOURCE_DIR 构建所有路径，使其成为绝对路径
    set(SRC_DIR "${CMAKE_SOURCE_DIR}/vendor/tree-sitter-${lang_name}/${src_subdir}")
    
    # 关键修复 #2: 正确地初始化一个空的CMake列表
    set(GRAMMAR_SOURCES)
    set(HAS_CXX_SOURCE FALSE)

    # 定义所有可能的源文件绝对路径
    set(PARSER_PATH "${SRC_DIR}/src/parser.c")
    set(SCANNER_C_PATH "${SRC_DIR}/src/scanner.c")
    set(SCANNER_CC_PATH "${SRC_DIR}/src/scanner.cc")
    set(SCANNER_CXX_PATH "${SRC_DIR}/src/scanner.cxx")

    # 检查文件是否存在并添加到列表中
    if(EXISTS ${PARSER_PATH})
        list(APPEND GRAMMAR_SOURCES ${PARSER_PATH})
    endif()
    if(EXISTS ${SCANNER_C_PATH})
        list(APPEND GRAMMAR_SOURCES ${SCANNER_C_PATH})
    endif()
    if(EXISTS ${SCANNER_CC_PATH})
        list(APPEND GRAMMAR_SOURCES ${SCANNER_CC_PATH})
        set(HAS_CXX_SOURCE TRUE)
    endif()
    if(EXISTS ${SCANNER_CXX_PATH})
        list(APPEND GRAMMAR_SOURCES ${SCANNER_CXX_PATH})
        set(HAS_CXX_SOURCE TRUE)
    endif()
    
    # 使用构建好的列表创建库
    add_library(tree-sitter-${lang_name} STATIC ${GRAMMAR_SOURCES})

    # 正确的链接策略：如果库包含C++文件，则使用C++链接器
    if(HAS_CXX_SOURCE)
        set_target_properties(tree-sitter-${lang_name} PROPERTIES LINKER_LANGUAGE CXX)
    endif()

    target_include_directories(tree-sitter-${lang_name} PUBLIC "${SRC_DIR}/src")
    target_link_libraries(tree-sitter-${lang_name} PRIVATE tree-sitter)
endmacro()

# --- 添加所有语言的语法库 ---
add_ts_grammar(c ".")
add_ts_grammar(cpp ".")
add_ts_grammar(python ".")
add_ts_grammar(java ".")
add_ts_grammar(go ".")
add_ts_grammar(rust ".")
add_ts_grammar(javascript ".")
add_ts_grammar(typescript "typescript")

# TypeScript 依赖 JavaScript
target_link_libraries(tree-sitter-typescript PRIVATE tree-sitter-javascript)

# --- 构建主可执行文件 ---
# 同样使用绝对路径
add_executable(cqa
    ${CMAKE_SOURCE_DIR}/src/main.cpp
    ${CMAKE_SOURCE_DIR}/src/Parser.cpp
    ${CMAKE_SOURCE_DIR}/src/Analyzer.cpp
)

# --- 链接所有库 ---
target_link_libraries(cqa PRIVATE
    tree-sitter
    tree-sitter-c
    tree-sitter-cpp
    tree-sitter-python
    tree-sitter-java
    tree-sitter-go
    tree-sitter-rust
    tree-sitter-javascript
    tree-sitter-typescript
)

# 为我们的主程序添加头文件目录
target_include_directories(cqa PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/vendor/tree-sitter/lib/include
)

# --- 实现可移植性：静态链接 ---
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_CLANG)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
endif()

# --- 安装指令 ---
install(TARGETS cqa DESTINATION bin)
