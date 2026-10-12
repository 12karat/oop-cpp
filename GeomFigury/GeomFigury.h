#pragma once
#include <iostream>
#include <string>
#include <vector>

// исключение для ошибок (неправильные параметры фигуры, не открылся файл и т.д.)
class ShapeException {
public:
    const char* message; // текст ошибки
    ShapeException(const char* msg) : message(msg) {}
};

// точка
class Point {
public:
    double x, y;
    Point();
    Point(double x, double y);
};

double dist(const Point& a, const Point& b);

// базовый (абстрактный) класс «Геометрическая фигура»
class Shape {
public:
    virtual ~Shape() {}                      // виртуальный деструктор (нужен для delete через Shape*)
    virtual double calc_area() const = 0;     // расчет площади фигуры
    virtual double calc_perimetr() const = 0; // вычисление периметра фигуры
    virtual void name() const = 0;            // выводит название фигуры
};

// круг
class Circle : public Shape {
private:
    Point c;
    double r;
public:
    Circle(Point c, double r);
    Circle(double x, double y, double r);
    double calc_area() const;
    double calc_perimetr() const;
    void name() const;
};

// эллипс
class Ellipse : public Shape {
private:
    Point c;
    double a, b;
public:
    Ellipse(Point c, double a, double b);
    Ellipse(double x, double y, double a, double b);
    double calc_area() const;
    double calc_perimetr() const;
    void name() const;
};

// треугольник
class Triangle : public Shape {
private:
    double a, b, c; // стороны
public:
    // по сторонам
    Triangle(double a, double b, double c);
    // по координатам вершин
    Triangle(Point p1, Point p2, Point p3);
    Triangle(double x1, double y1, double x2, double y2, double x3, double y3);
    double calc_area() const;
    double calc_perimetr() const;
    void name() const;
};

// прямоугольник
class Rectangle : public Shape {
private:
    double w, h; // стороны
public:
    // по сторонам
    Rectangle(double w, double h);
    // по координатам четырех вершин (в порядке обхода)
    Rectangle(Point p1, Point p2, Point p3, Point p4);
    Rectangle(double x1, double y1, double x2, double y2,
              double x3, double y3, double x4, double y4);
    double calc_area() const;
    double calc_perimetr() const;
    void name() const;
};

// многоугольник
class Polygon : public Shape {
private:
    std::vector<Point> pts; // вершины в порядке обхода
    void init(const std::vector<Point>& points);
public:
    // по координатам вершин
    Polygon(const std::vector<Point>& points);
    // из файла с координатами вершин (по одной вершине в строке: x y)
    Polygon(const std::string& file_name);
    int size() const { return (int)pts.size(); }
    double calc_area() const;
    double calc_perimetr() const;
    void name() const;
};
