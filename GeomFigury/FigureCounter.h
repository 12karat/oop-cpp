#pragma once
#include <string>

// результат подсчета фигур
struct FigureCount {
    int rectangles; // количество прямоугольников
    int circles;    // количество кругов
    int triangles;  // количество треугольников
    int noise;      // количество «каракуль»

    double areaRectangles; // общая площадь прямоугольников
    double areaCircles;    // общая площадь кругов
    double areaTriangles;  // общая площадь треугольников

    FigureCount() : rectangles(0), circles(0), triangles(0), noise(0),
        areaRectangles(0), areaCircles(0), areaTriangles(0) {}
};

// читает файл-картинку (200 x 200), находит фигуры и считает их по типам
// возвращает false, если файл не удалось открыть
bool countFigures(const std::string& inputFile, FigureCount& result);

// записывает результат в файл из четырех строк (Rectangle = , Circle = , Triangle = , Noise = )
bool writeFigureCount(const std::string& outputFile, const FigureCount& result);
