#define _CRT_SECURE_NO_WARNINGS
#include <chrono>
#include <iostream>
#include <string>
#ifdef _WIN32
#include <windows.h>
#endif
#include "Maze.h"

// Убираем кавычки и пробелы по краям (при перетаскивании файла в консоль путь приходит в кавычках)
static std::string cleanPath(std::string s)
{
    while (!s.empty() && (s.back() == ' ' || s.back() == '\r' || s.back() == '\n' || s.back() == '"')) s.pop_back();
    size_t b = 0;
    while (b < s.size() && (s[b] == ' ' || s[b] == '"')) ++b;
    return s.substr(b);
}

// "путь/maze.bmp" -> "путь/maze_solved.bmp"
static std::string solvedName(const std::string& in)
{
    size_t dot = in.find_last_of('.');
    size_t slash = in.find_last_of("/\\");
    if (dot == std::string::npos || (slash != std::string::npos && dot < slash))
        return in + "_solved.bmp";
    return in.substr(0, dot) + "_solved.bmp";
}

int main(int argc, char* argv[])
{
#ifdef _WIN32
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
#endif

    std::string inPath, outPath;
    if (argc > 1) {
        inPath = cleanPath(argv[1]);
    }
    else {
        std::cout << "Введите путь к BMP файлу лабиринта: ";
        std::getline(std::cin, inPath);
        inPath = cleanPath(inPath);
    }
    outPath = (argc > 2) ? cleanPath(argv[2]) : solvedName(inPath);

    auto t0 = std::chrono::steady_clock::now();

    Bitmap bmp;
    std::string error;
    if (!bmp.load(inPath, error)) {
        std::cout << "Ошибка: " << error << "\n";
        return 1;
    }

    MazeSolver solver(bmp);
    if (!solver.solve(error)) {
        std::cout << "Ошибка: " << error << "\n";
        return 1;
    }

    if (!bmp.save(outPath, error)) {
        std::cout << "Ошибка: " << error << "\n";
        return 1;
    }

    auto t1 = std::chrono::steady_clock::now();
    double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();

    std::cout << "Выход найден. Длина пути: " << solver.pathLength() << " пикселей\n";
    std::cout << "Результат сохранён: " << outPath << "\n";
    std::cout << "Время: " << ms << " мс\n";
    return 0;
}
