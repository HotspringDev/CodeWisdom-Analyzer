# CodeWisdom Analyzer

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen)](https://github.com/HotspringDev/CodeWisdom-Analyzer)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Version](https://img.shields.io/badge/version-1.0.0-blue.svg)](https://github.com/HotspringDev/CodeWisdom-Analyzer)
[![C++ Standard](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://isocpp.org/)
[![Platform](https://img.shields.io/badge/platform-linux%20%7C%20windows%20%7C%20macos-lightgrey.svg)](https://github.com/HotspringDev/CodeWisdom-Analyzer)

**CodeWisdom Analyzer** is a blazingly fast, portable, single-file code quality analysis tool engineered in modern C++. It dissects your codebase to identify "code smells" and technical debt, providing a quantitative metric—the **"Legacy Code Index"**—to rank files by their need for refactoring.

Built on the powerful `tree-sitter` parsing framework, it offers deep, syntax-aware analysis for a wide array of popular programming languages.

---

## ✨ Features

- **Blazing Fast Analysis**: Leverages the performance of C++ and the efficiency of `tree-sitter` for rapid, incremental parsing.
- **Single-File Portability**: Compiles into a single, dependency-free executable. Drop it anywhere and run it instantly.
- **Extensive Language Support**: Provides accurate, syntax-aware analysis for 8+ major programming languages.
- **Multi-Dimensional Metrics**: Evaluates code quality based on a sophisticated, weighted model of key metrics:
  - **Cyclomatic Complexity**: Measures the logical complexity of functions.
  - **Function Length**: Identifies overly long and unwieldy functions.
  - *(Coming Soon)*: Comment Coverage, Naming Conventions, Error Handling, and Code Duplication.
- **Intelligent Ranking**: Calculates a **Legacy Code Index (LCI)** for each file, allowing you to prioritize refactoring efforts on the code that needs it most.
- **Elegant Terminal UI**: Beautiful, color-coded terminal reports for intuitive and actionable insights.

## 🌐 Language Support

CodeWisdom Analyzer is built to be language-agnostic. Support for the following languages is included out-of-the-box:

| Language       | File Extensions                        | Status      |
|----------------|----------------------------------------|-------------|
| **C**          | `.c`, `.h`                             | ✅ Supported |
| **C++**        | `.cpp`, `.hpp`, `.cc`, `.cxx`, `.h`    | ✅ Supported |
| **Python**     | `.py`                                  | ✅ Supported |
| **Java**       | `.java`                                | ✅ Supported |
| **Rust**       | `.rs`                                  | ✅ Supported |
| **Go**         | `.go`                                  | ✅ Supported |
| **JavaScript** | `.js`                                  | ✅ Supported |
| **TypeScript** | `.ts`                                  | ✅ Supported |

Adding a new language is as simple as integrating its `tree-sitter` grammar.

## 🚀 Getting Started

### Prerequisites

- A modern C++ compiler (GCC, Clang, or MSVC) that supports C++17.
- [CMake](https://cmake.org/) (version 3.16 or higher).
- [Git](https://git-scm.com/) (for fetching dependencies).

### Building from Source

1.  **Clone the repository:**
    ```bash
    git clone --recursive https://github.com/HotspringDev/CodeWisdom-Analyzer.git
    cd CodeWisdom-Analyzer
    ```
    > **Note:** The `--recursive` flag is essential to download the `tree-sitter` grammar submodules. If you cloned without it, run `git submodule update --init --recursive`.

2.  **Configure and build with CMake:**
    ```bash
    mkdir build
    cd build
    cmake ..
    cmake --build .
    ```

3.  **Find the executable:**
    The final, portable executable `cqa` (or `cqa.exe` on Windows) will be located in the `build/` directory.

## 💻 Usage

Run the analyzer by pointing it to a source file or a directory. It will recursively scan for supported files and generate a ranked report.

```bash
# Analyze a single file
./cqa /path/to/your/source/file.cpp

# Analyze an entire project directory
./cqa /path/to/your/project
```

### Example Output

The tool will generate a beautifully formatted, color-coded report directly in your terminal, ranking files from the highest Legacy Code Index (worst) to the lowest (best).

```bash
Analyzing files, please wait...........................
Analysis complete.

=============== PROJECT ANALYSIS RANKING (WORST FILES FIRST) ===============


======================================================
  Analysis Report for: src/core/complex_logic.rs
  Legacy Code Index (LCI): 78.52 (Higher is worse)
------------------------------------------------------
  Avg Function Length: 125.40 lines
  Avg Cyclomatic Complexity: 22.80
======================================================
Found 5 functions:

  - Function: process_main_event_loop
    - Length: 250, Complexity: 45
  - Function: calculate_transform_matrix
    - Length: 98, Complexity: 18
...

======================================================
  Analysis Report for: src/utils/simple_math.rs
  Legacy Code Index (LCI): 5.15 (Higher is worse)
------------------------------------------------------
  Avg Function Length: 12.50 lines
  Avg Cyclomatic Complexity: 2.10
======================================================
Found 4 functions:

  - Function: add
    - Length: 5, Complexity: 1
  - Function: subtract
    - Length: 5, Complexity: 1
...
```

## 🛠️ How It Works

The **Legacy Code Index (LCI)** is a weighted score from 0 to 100, where a higher score indicates a greater need for refactoring. It is calculated as `100 - QualityScore`.

The `QualityScore` is derived from mapping raw metrics (like average function length and complexity) to a normalized 0-100 scale and then combining them using predefined weights.

-   **Cyclomatic Complexity Weight**: 70%
-   **Function Length Weight**: 30%

This model ensures that overly complex logic is penalized more heavily than simple, albeit verbose, code.

## 🌱 Contributing

Contributions are what make the open-source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement".

1.  **Fork the Project**
2.  **Create your Feature Branch** (`git checkout -b feature/AmazingFeature`)
3.  **Commit your Changes** (`git commit -m 'Add some AmazingFeature'`)
4.  **Push to the Branch** (`git push origin feature/AmazingFeature`)
5.  **Open a Pull Request**

## 📜 License

Distributed under the MIT License. See `LICENSE` for more information.
