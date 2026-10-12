#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include "GeomFigury.h"

using namespace std;

const double PI = 3.14159265358979323846;

// ---------- точка ----------

Point::Point() {
    x = 0;
    y = 0;
}

Point::Point(double x, double y) {
    this->x = x;
    this->y = y;
}

double dist(const Point& a, const Point& b) {
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

// ---------- круг ----------

Circle::Circle(Point c, double r) {
    if (r <= 0)
        throw ShapeException("Радиус должен быть больше нуля");
    this->c = c;
    this->r = r;
}

Circle::Circle(double x, double y, double r) {
    if (r <= 0)
        throw ShapeException("Радиус должен быть больше нуля");
    this->c = Point(x, y);
    this->r = r;
}

double Circle::calc_area() const {
    return PI * r * r;
}

double Circle::calc_perimetr() const {
    return 2 * PI * r;
}

void Circle::name() const {
    cout << "Circle\n";
}

// ---------- эллипс ----------

Ellipse::Ellipse(Point c, double a, double b) {
    if (a <= 0 || b <= 0)
        throw ShapeException("Радиусы эллипса должны быть больше нуля");
    this->c = c;
    this->a = a;
    this->b = b;
}

Ellipse::Ellipse(double x, double y, double a, double b) {
    if (a <= 0 || b <= 0)
        throw ShapeException("Радиусы эллипса должны быть больше нуля");
    this->c = Point(x, y);
    this->a = a;
    this->b = b;
}

double Ellipse::calc_area() const {
    return PI * a * b;
}

// периметр эллипса (приближенная формула Рамануджана)
double Ellipse::calc_perimetr() const {
    return PI * (3 * (a + b) - sqrt((3 * a + b) * (a + 3 * b)));
}

void Ellipse::name() const {
    cout << "Ellipse\n";
}

// ---------- треугольник ----------

Triangle::Triangle(double a, double b, double c) {
    // условие существования треугольника
    if (a <= 0 || b <= 0 || c <= 0 || a + b <= c || a + c <= b || b + c <= a)
        throw ShapeException("Треугольник с такими сторонами не существует");
    this->a = a;
    this->b = b;
    this->c = c;
}

Triangle::Triangle(Point p1, Point p2, Point p3) {
    // площадь через векторное произведение: если 0, вершины лежат на одной прямой
    double cross = (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x);
    if (fabs(cross) < 1e-12)
        throw ShapeException("Вершины лежат на одной прямой, это не треугольник");
    this->a = dist(p1, p2);
    this->b = dist(p2, p3);
    this->c = dist(p3, p1);
}

Triangle::Triangle(double x1, double y1, double x2, double y2, double x3, double y3) {
    Triangle t(Point(x1, y1), Point(x2, y2), Point(x3, y3));
    *this = t;
}

double Triangle::calc_area() const {
    // формула Герона
    double p = (a + b + c) / 2;
    double s = p * (p - a) * (p - b) * (p - c);
    if (s < 0) s = 0;
    return sqrt(s);
}

double Triangle::calc_perimetr() const {
    return a + b + c;
}

void Triangle::name() const {
    cout << "Triangle\n";
}

// ---------- прямоугольник ----------

Rectangle::Rectangle(double w, double h) {
    if (w <= 0 || h <= 0)
        throw ShapeException("Стороны прямоугольника должны быть больше нуля");
    this->w = w;
    this->h = h;
}

Rectangle::Rectangle(Point p1, Point p2, Point p3, Point p4) {
    double ab = dist(p1, p2);
    double bc = dist(p2, p3);
    double cd = dist(p3, p4);
    double da = dist(p4, p1);
    if (ab <= 0 || bc <= 0)
        throw ShapeException("Стороны прямоугольника должны быть больше нуля");

    // противоположные стороны равны и угол между соседними сторонами прямой
    double dot = (p2.x - p1.x) * (p3.x - p2.x) + (p2.y - p1.y) * (p3.y - p2.y);
    double eps = 1e-6;
    if (fabs(ab - cd) > eps * ab || fabs(bc - da) > eps * bc || fabs(dot) > eps * ab * bc)
        throw ShapeException("Эти четыре точки не образуют прямоугольник");

    this->w = ab;
    this->h = bc;
}

Rectangle::Rectangle(double x1, double y1, double x2, double y2,
                     double x3, double y3, double x4, double y4) {
    Rectangle r(Point(x1, y1), Point(x2, y2), Point(x3, y3), Point(x4, y4));
    *this = r;
}

double Rectangle::calc_area() const {
    return w * h;
}

double Rectangle::calc_perimetr() const {
    return 2 * (w + h);
}

void Rectangle::name() const {
    cout << "Rectangle\n";
}

// ---------- многоугольник ----------

// ----- чтение координат вершин из файла -----

// превращение слова в число (десятичная запятая тоже допускается)
static bool parseNumber(string tok, double& value) {
    while (!tok.empty() && (tok[tok.size() - 1] == ',' || tok[tok.size() - 1] == ';'))
        tok.erase(tok.size() - 1);
    while (!tok.empty() && (tok[0] == ',' || tok[0] == ';'))
        tok.erase(0, 1);
    for (size_t i = 0; i < tok.size(); i++)
        if (tok[i] == ',') tok[i] = '.';
    if (tok.empty()) return false;

    char* end;
    value = strtod(tok.c_str(), &end);
    return *end == '\0';
}

// все числа из одной строки файла (разделители: пробел, табуляция, ';' или ',')
static vector<double> numbersInLine(string s) {
    bool hasSemicolon = s.find(';') != string::npos;
    bool hasSpace = s.find_first_of(" \t") != string::npos;
    // "1.5,2.5" - запятая здесь разделитель
    if (!hasSemicolon && !hasSpace)
        for (size_t i = 0; i < s.size(); i++)
            if (s[i] == ',') s[i] = ' ';
    for (size_t i = 0; i < s.size(); i++)
        if (s[i] == ';' || s[i] == '\t' || s[i] == '\r') s[i] = ' ';

    vector<double> res;
    string tok;
    for (size_t i = 0; i <= s.size(); i++) {
        if (i == s.size() || s[i] == ' ') {
            double v;
            if (!tok.empty() && parseNumber(tok, v)) res.push_back(v);
            tok.clear();
        }
        else {
            tok += s[i];
        }
    }
    return res;
}

// чтение вершин: заголовки и нечисловые слова пропускаются
static vector<Point> readPointsFromFile(const string& fileName) {
    ifstream file(fileName.c_str());
    if (!file.is_open())
        throw ShapeException("Не удалось открыть файл с вершинами многоугольника");

    vector<vector<double> > rows;
    string line;
    bool first = true;
    while (getline(file, line)) {
        // убираем BOM в начале файла
        if (first && line.size() >= 3 && (unsigned char)line[0] == 0xEF
            && (unsigned char)line[1] == 0xBB && (unsigned char)line[2] == 0xBF)
            line.erase(0, 3);
        first = false;
        vector<double> nums = numbersInLine(line);
        if (!nums.empty()) rows.push_back(nums);
    }

    bool all1 = true, all3 = true;
    for (size_t i = 0; i < rows.size(); i++) {
        if (rows[i].size() != 1) all1 = false;
        if (rows[i].size() != 3) all3 = false;
    }

    vector<Point> pts;
    if (all1) {
        // числа идут по одному в строке: x, y, x, y, ...
        for (size_t i = 0; i + 1 < rows.size(); i += 2)
            pts.push_back(Point(rows[i][0], rows[i + 1][0]));
    }
    else if (all3) {
        // номер точки, x, y
        for (size_t i = 0; i < rows.size(); i++)
            pts.push_back(Point(rows[i][1], rows[i][2]));
    }
    else {
        // обычный случай: в строке x y (лишние числа игнорируем)
        for (size_t i = 0; i < rows.size(); i++)
            if (rows[i].size() >= 2)
                pts.push_back(Point(rows[i][0], rows[i][1]));
    }
    return pts;
}

void Polygon::init(const vector<Point>& points) {
    pts = points;
    // если последняя точка повторяет первую (замкнутый контур), лишнюю убираем
    if (pts.size() > 1 && pts[0].x == pts[pts.size() - 1].x && pts[0].y == pts[pts.size() - 1].y)
        pts.pop_back();
    if (pts.size() < 3)
        throw ShapeException("У многоугольника должно быть не меньше трех вершин");
}

Polygon::Polygon(const vector<Point>& points) {
    init(points);
}

Polygon::Polygon(const string& file_name) {
    init(readPointsFromFile(file_name));
}

// площадь по формуле Гаусса (формула площади Гаусса / «шнурования»)
double Polygon::calc_area() const {
    double s = 0;
    int n = (int)pts.size();
    for (int i = 0; i < n; i++) {
        const Point& p = pts[i];
        const Point& q = pts[(i + 1) % n];
        s += p.x * q.y - q.x * p.y;
    }
    return fabs(s) / 2;
}

double Polygon::calc_perimetr() const {
    double p = 0;
    int n = (int)pts.size();
    for (int i = 0; i < n; i++)
        p += dist(pts[i], pts[(i + 1) % n]);
    return p;
}

void Polygon::name() const {
    cout << "Polygon\n";
}
