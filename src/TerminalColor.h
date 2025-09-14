/**
 * @file TerminalColor.h
 * @brief Provides a cross-platform, header-only utility for adding color to terminal output.
 *
 * This file defines a simple `TerminalColor` namespace with a `Color` enum and an overloaded
 * `operator<<` for `std::ostream`. This allows for intuitive, chainable color manipulation
 * on both Windows (via Win32 API) and Unix-like systems (via ANSI escape codes).
 *
 * @author HotspringDev
 * @date 2025-09-14
 */

#ifndef TERMINAL_COLOR_H
#define TERMINAL_COLOR_H

#include <iostream>

// Platform-specific includes for Windows console API
#if defined(_WIN32)
#include <windows.h>
#endif

namespace TerminalColor {

    /**
     * @enum Color
     * @brief Defines the set of available terminal colors for styling output.
     */
    enum class Color {
        RESET,
        RED,
        GREEN,
        YELLOW,
        CYAN,
        WHITE
    };

    /**
     * @brief Overloads the stream insertion operator to apply color settings to an ostream.
     * @param os The output stream (e.g., std::cout).
     * @param c The Color to apply.
     * @return The modified output stream.
     */
    inline std::ostream& operator<<(std::ostream& os, Color c) {
        #if defined(_WIN32)
        // Windows implementation using the Win32 Console API.
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

        // Save the default console attributes once to allow resetting.
        static CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
        static bool defaults_saved = false;
        if (!defaults_saved) {
            GetConsoleScreenBufferInfo(hConsole, &bufferInfo);
            defaults_saved = true;
        }

        switch (c) {
            case Color::RED:
                SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
                break;
            case Color::GREEN:
                SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                break;
            case Color::YELLOW:
                SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                break;
            case Color::CYAN:
                SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
                break;
            case Color::WHITE:
                SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
                break;
            case Color::RESET:
            default:
                // Restore the default console attributes.
                SetConsoleTextAttribute(hConsole, bufferInfo.wAttributes);
                break;
        }
        #else
        // Linux/macOS implementation using ANSI escape codes.
        switch (c) {
            case Color::RED:    os << "\033[1;31m"; break;
            case Color::GREEN:  os << "\033[1;32m"; break;
            case Color::YELLOW: os << "\033[1;33m"; break;
            case Color::CYAN:   os << "\033[1;36m"; break;
            case Color::WHITE:  os << "\033[1;37m"; break;
            case Color::RESET:
            default:            os << "\033[0m"; break; // Reset all terminal attributes.
        }
        #endif
        return os;
    }
} // namespace TerminalColor

#endif // TERMINAL_COLOR_H
