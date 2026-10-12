#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <iomanip>
#include "GeomFigury.h"
#include "FigureCounter.h"

using namespace std;

// вывод информации о фигуре через указатель на базовый класс
void printShape(const Shape* s) {
    s->name();
    cout << "Площадь: " << s->calc_area() << endl;
    cout << "Периметр: " << s->calc_perimetr() << endl << endl;
}

// 1. демонстрация всех классов и способов задания фигур
void demoShapes() {
    cout << "Демонстрация классов фигур" << endl << endl;

    try {
        vector<Shape*> list;

        // круг: центр - Point или числа (int, float, double), радиус
        list.push_back(new Circle(Point(0, 0), 5));
        list.push_back(new Circle(1, 2, 2.5f));

        // эллипс
        list.push_back(new Ellipse(Point(0, 0), 6, 3));
        list.push_back(new Ellipse(2.5, 1.5, 4, 2));

        // треугольник: по сторонам и по координатам вершин
        list.push_back(new Triangle(3, 4, 5));
        list.push_back(new Triangle(Point(0, 0), Point(4, 0), Point(0, 3)));
        list.push_back(new Triangle(0, 0, 4.5f, 0, 0, 2));

        // прямоугольник: по сторонам и по координатам вершин
        list.push_back(new Rectangle(10, 4));
        list.push_back(new Rectangle(Point(0, 0), Point(5, 0), Point(5, 2), Point(0, 2)));
        list.push_back(new Rectangle(0, 0, 3, 0, 3, 1.5, 0, 1.5));

        // многоугольник по вершинам (пятиугольник)
        vector<Point> pts;
        pts.push_back(Point(0, 0));
        pts.push_back(Point(4, 0));
        pts.push_back(Point(5, 3));
        pts.push_back(Point(2, 5));
        pts.push_back(Point(-1, 3));
        list.push_back(new Polygon(pts));

        for (size_t i = 0; i < list.size(); i++)
            printShape(list[i]);

        // удаляем фигуры (деструктор виртуальный, поэтому удаляется правильно)
        for (size_t i = 0; i < list.size(); i++)
            delete list[i];
    }
    catch (ShapeException& e) {
        cout << "Ошибка: " << e.message << endl;
    }

    // проверка неправильных параметров
    cout << "Проверка некорректных данных:" << endl;
    try {
        Triangle t(1, 2, 10);
    }
    catch (ShapeException& e) {
        cout << "Triangle(1, 2, 10) -> " << e.message << endl;
    }
    try {
        Rectangle r(Point(0, 0), Point(4, 0), Point(5, 2), Point(0, 2));
    }
    catch (ShapeException& e) {
        cout << "Rectangle по 4 точкам -> " << e.message << endl;
    }
    try {
        Circle c(0, 0, -1);
    }
    catch (ShapeException& e) {
        cout << "Circle(0, 0, -1) -> " << e.message << endl;
    }
    cout << endl;
}

// 2. площадь участка из файла с вершинами
void plotArea() {
    string fileName;
    cout << "Имя файла с границами участка (например granitsy-uchastka2.txt): ";
    getline(cin, fileName);
    if (fileName.empty()) fileName = "granitsy-uchastka2.txt";

    try {
        Polygon plot(fileName); // конструктор многоугольника из файла
        cout << "Прочитано вершин: " << plot.size() << endl;
        plot.name();
        cout << "Площадь участка: " << plot.calc_area() << endl;
        cout << "Периметр участка: " << plot.calc_perimetr() << endl;
    }
    catch (ShapeException& e) {
        cout << "Ошибка: " << e.message << endl;
    }
    cout << endl;
}

// 3. подсчет фигур во входном файле
void countInFile() {
    string inFile, outFile;
    cout << "Имя входного файла с рисунком 200x200: ";
    getline(cin, inFile);
    cout << "Имя выходного файла (пусто - output.txt): ";
    getline(cin, outFile);
    if (outFile.empty()) outFile = "output.txt";

    FigureCount res;
    if (!countFigures(inFile, res)) {
        cout << "Не удалось прочитать файл " << inFile << endl << endl;
        return;
    }
    if (!writeFigureCount(outFile, res)) {
        cout << "Не удалось записать файл " << outFile << endl << endl;
        return;
    }

    cout << "Rectangle = " << res.rectangles << endl;
    cout << "Circle = " << res.circles << endl;
    cout << "Triangle = " << res.triangles << endl;
    cout << "Noise = " << res.noise << endl;
    cout << "Результат записан в файл " << outFile << endl << endl;

    cout << "Площадь, занимаемая фигурами (в клетках):" << endl;
    cout << "  прямоугольники: " << res.areaRectangles << endl;
    cout << "  треугольники:   " << res.areaTriangles << endl;
    cout << "  круги:          " << res.areaCircles << endl;
    cout << "  всего:          " << res.areaRectangles + res.areaTriangles + res.areaCircles << endl << endl;
}

int main() {
#ifdef _WIN32
    // чтобы русский текст нормально отображался в консоли
    system("chcp 65001 > nul");
#endif

    // площади и периметры выводим с двумя знаками после запятой (без записи вида 1.2e+06)
    cout << fixed << setprecision(2);

    int choice = -1;
    while (choice != 0) {
        cout << "1 - демонстрация классов фигур" << endl;
        cout << "2 - площадь участка из файла (многоугольник)" << endl;
        cout << "3 - подсчет фигур в файле" << endl;
        cout << "0 - выход" << endl;
        cout << "Выберите пункт: ";

        string line;
        getline(cin, line);
        choice = line.empty() ? -1 : atoi(line.c_str());
        cout << endl;

        if (choice == 1) demoShapes();
        else if (choice == 2) plotArea();
        else if (choice == 3) countInFile();
    }
    return 0;
}
