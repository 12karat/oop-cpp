#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <cmath>
#include <algorithm>
#include "GeomFigury.h"
#include "FigureCounter.h"

using namespace std;

// Короткие черточки и точки не считаем: если фигура помещается в квадрат
// меньше MIN_FIGURE_SIZE x MIN_FIGURE_SIZE клеток, она игнорируется
static const int MIN_FIGURE_SIZE = 8;

// связная группа закрашенных клеток
struct Component {
    int label;
    vector<Point> pixels; // x - номер столбца, y - номер строки
    int minX, maxX, minY, maxY;
};

enum FigureType { FIG_RECTANGLE, FIG_CIRCLE, FIG_TRIANGLE, FIG_NOISE };

// ---------- чтение картинки ----------

// разбиение строки на слова по пробелам и табуляции
static vector<string> splitWords(const string& s) {
    vector<string> words;
    string cur;
    for (size_t i = 0; i <= s.size(); i++) {
        if (i == s.size() || s[i] == ' ' || s[i] == '\t') {
            if (!cur.empty()) words.push_back(cur);
            cur.clear();
        }
        else {
            cur += s[i];
        }
    }
    return words;
}

// читает файл и возвращает таблицу drawn[строка][столбец] (true - клетка нарисована)
static bool readImage(const string& fileName, vector<vector<bool> >& drawn) {
    ifstream file(fileName.c_str());
    if (!file.is_open()) return false;

    vector<string> lines;
    string line;
    bool first = true;
    while (getline(file, line)) {
        while (!line.empty() && (line[line.size() - 1] == '\r' || line[line.size() - 1] == '\n'))
            line.erase(line.size() - 1);
        if (first && line.size() >= 3 && (unsigned char)line[0] == 0xEF
            && (unsigned char)line[1] == 0xBB && (unsigned char)line[2] == 0xBF)
            line.erase(0, 3);
        first = false;
        lines.push_back(line);
    }
    while (!lines.empty() && lines[lines.size() - 1].empty())
        lines.pop_back();
    if (lines.empty()) return false;

    int rows = (int)lines.size();

    // формат 1: клетки разделены пробелами (в строке столько слов, сколько строк в файле)
    int tokenLines = 0;
    for (int i = 0; i < rows; i++)
        if ((int)splitWords(lines[i]).size() == rows) tokenLines++;
    bool tokenMode = (rows > 1 && tokenLines * 10 >= rows * 8);

    // table[r][c] - символ клетки; -1 - клетки нет (строка короче)
    vector<vector<int> > table(rows);
    int width = 0;
    for (int r = 0; r < rows; r++) {
        if (tokenMode) {
            vector<string> w = splitWords(lines[r]);
            for (size_t c = 0; c < w.size(); c++) table[r].push_back((unsigned char)w[c][0]);
        }
        else {
            for (size_t c = 0; c < lines[r].size(); c++) table[r].push_back((unsigned char)lines[r][c]);
        }
        width = max(width, (int)table[r].size());
    }

    // фон - самый частый символ в файле, все остальное считаем нарисованным
    map<int, int> freq;
    for (int r = 0; r < rows; r++)
        for (size_t c = 0; c < table[r].size(); c++)
            freq[table[r][c]]++;
    int background = ' ';
    int best = -1;
    for (map<int, int>::iterator it = freq.begin(); it != freq.end(); ++it)
        if (it->second > best) {
            best = it->second;
            background = it->first;
        }

    drawn.assign(rows, vector<bool>(width, false));
    for (int r = 0; r < rows; r++)
        for (size_t c = 0; c < table[r].size(); c++)
            drawn[r][c] = (table[r][c] != background);
    return true;
}

// ---------- геометрия ----------

static double cross(const Point& o, const Point& a, const Point& b) {
    return (a.x - o.x) * (b.y - o.y) - (a.y - o.y) * (b.x - o.x);
}

static bool lessPoint(const Point& a, const Point& b) {
    if (a.x != b.x) return a.x < b.x;
    return a.y < b.y;
}

// выпуклая оболочка (алгоритм Эндрю)
static vector<Point> convexHull(vector<Point> p) {
    sort(p.begin(), p.end(), lessPoint);
    int n = (int)p.size();
    if (n < 3) return p;

    vector<Point> h(2 * n);
    int k = 0;
    for (int i = 0; i < n; i++) {
        while (k >= 2 && cross(h[k - 2], h[k - 1], p[i]) <= 0) k--;
        h[k++] = p[i];
    }
    for (int i = n - 2, t = k + 1; i >= 0; i--) {
        while (k >= t && cross(h[k - 2], h[k - 1], p[i]) <= 0) k--;
        h[k++] = p[i];
    }
    h.resize(k - 1);
    return h;
}

// расстояние от точки до отрезка
static double distToSegment(const Point& p, const Point& a, const Point& b) {
    double dx = b.x - a.x, dy = b.y - a.y;
    double len2 = dx * dx + dy * dy;
    if (len2 == 0) return dist(p, a);
    double t = ((p.x - a.x) * dx + (p.y - a.y) * dy) / len2;
    if (t < 0) t = 0;
    if (t > 1) t = 1;
    return dist(p, Point(a.x + t * dx, a.y + t * dy));
}

// упрощение выпуклого многоугольника: убираем вершины, которые почти не влияют на форму
static vector<Point> simplifyHull(vector<Point> h, double eps) {
    while (h.size() > 3) {
        int n = (int)h.size();
        int bestIndex = -1;
        double bestCost = 1e18;
        for (int i = 0; i < n; i++) {
            double cost = distToSegment(h[i], h[(i + n - 1) % n], h[(i + 1) % n]);
            if (cost < bestCost) {
                bestCost = cost;
                bestIndex = i;
            }
        }
        if (bestCost >= eps) break;
        h.erase(h.begin() + bestIndex);
    }
    return h;
}

// похож ли четырехугольник на прямоугольник (с допуском на неточность пикселей)
static bool looksLikeRectangle(const vector<Point>& q, double& w, double& h) {
    double len[4];
    for (int i = 0; i < 4; i++)
        len[i] = dist(q[i], q[(i + 1) % 4]);

    for (int i = 0; i < 4; i++) {
        const Point& a = q[i];
        const Point& b = q[(i + 1) % 4];
        const Point& c = q[(i + 2) % 4];
        double dot = (b.x - a.x) * (c.x - b.x) + (b.y - a.y) * (c.y - b.y);
        double cosine = fabs(dot) / (len[i] * len[(i + 1) % 4]);
        if (cosine > 0.2) return false; // угол заметно отличается от прямого
    }
    if (fabs(len[0] - len[2]) > 0.2 * max(len[0], len[2])) return false;
    if (fabs(len[1] - len[3]) > 0.2 * max(len[1], len[3])) return false;

    w = (len[0] + len[2]) / 2;
    h = (len[1] + len[3]) / 2;
    return true;
}

// ---------- распознавание ----------

// определяет тип фигуры по набору ее клеток; для найденной фигуры создает объект
// соответствующего класса (shape), иначе shape = nullptr
static FigureType classify(const Component& comp, const vector<vector<int> >& label, Shape*& shape) {
    shape = nullptr;

    vector<Point> hull = convexHull(comp.pixels);
    if (hull.size() < 3) return FIG_NOISE; // все клетки на одной прямой

    Polygon hullPoly(hull);
    double area = hullPoly.calc_area();
    double perimeter = hullPoly.calc_perimetr();
    if (area < 1) return FIG_NOISE;

    int n = (int)hull.size();
    int pixelCount = (int)comp.pixels.size();

    // 1) какая доля клеток лежит вдоль границы оболочки (признак контура фигуры)
    int onBorder = 0;
    for (size_t i = 0; i < comp.pixels.size(); i++) {
        for (int j = 0; j < n; j++) {
            if (distToSegment(comp.pixels[i], hull[j], hull[(j + 1) % n]) <= 1.5) {
                onBorder++;
                break;
            }
        }
    }
    double outlineRatio = (double)onBorder / pixelCount;

    // 2) насколько фигура закрашена внутри (признак залитой фигуры)
    double fillRatio = pixelCount / (area + perimeter / 2 + 1);

    // 3) какая часть границы оболочки реально нарисована (нет ли разрывов)
    int samples = 0, covered = 0;
    int rows = (int)label.size();
    int cols = (int)label[0].size();
    for (int j = 0; j < n; j++) {
        const Point& a = hull[j];
        const Point& b = hull[(j + 1) % n];
        int steps = max(1, (int)ceil(dist(a, b)));
        for (int s = 0; s < steps; s++) {
            double x = a.x + (b.x - a.x) * s / steps;
            double y = a.y + (b.y - a.y) * s / steps;
            bool hit = false;
            for (int iy = (int)floor(y - 1.5); iy <= (int)ceil(y + 1.5) && !hit; iy++)
                for (int ix = (int)floor(x - 1.5); ix <= (int)ceil(x + 1.5) && !hit; ix++)
                    if (iy >= 0 && iy < rows && ix >= 0 && ix < cols && label[iy][ix] == comp.label
                        && dist(Point(ix, iy), Point(x, y)) <= 1.5)
                        hit = true;
            samples++;
            if (hit) covered++;
        }
    }
    double coverage = (double)covered / samples;

    // залитая фигура: граница проходит по краю клеток, а не по их центрам,
    // поэтому к размерам такой фигуры добавляем половину клетки
    bool filled = (fillRatio >= 0.85);

    // каракуля: границы нет, внутри не закрашено, или контур разорван
    if (coverage < 0.9) return FIG_NOISE;
    if (outlineRatio < 0.9 && fillRatio < 0.85) return FIG_NOISE;

    // --- круг: оболочка почти идеально круглая ---
    double circularity = 4 * 3.14159265358979323846 * area / (perimeter * perimeter);
    Point center(0, 0);
    for (size_t i = 0; i < comp.pixels.size(); i++) {
        center.x += comp.pixels[i].x;
        center.y += comp.pixels[i].y;
    }
    center.x /= pixelCount;
    center.y /= pixelCount;
    double rMin = 1e18, rMax = 0;
    for (int j = 0; j < n; j++) {
        double d = dist(hull[j], center);
        rMin = min(rMin, d);
        rMax = max(rMax, d);
    }
    // оставляем только настоящие углы оболочки: у круга их много (7 и больше),
    // у шестиугольника, пятиугольника, прямоугольника и треугольника - меньше
    double diameter = max(comp.maxX - comp.minX, comp.maxY - comp.minY) + 1;
    vector<Point> corners = simplifyHull(hull, max(1.2, 0.05 * diameter));

    // круг: либо у оболочки много углов и она круглая, либо она очень близка к окружности
    // (маленькие круги дают мало углов, но круглость у них выше, чем у шестиугольника)
    bool manyCorners = (circularity >= 0.92 && rMax <= 1.2 * rMin && corners.size() >= 7);
    bool veryRound = (circularity >= 0.96 && rMax <= 1.12 * rMin);
    if (manyCorners || veryRound) {
        double circleArea = filled ? area + perimeter / 2 + 1 : area;
        shape = new Circle(center, sqrt(circleArea / 3.14159265358979323846));
        return FIG_CIRCLE;
    }

    // --- треугольник и прямоугольник ---
    try {
        if (corners.size() == 3) {
            Triangle t(corners[0], corners[1], corners[2]);
            if (filled) {
                // увеличиваем треугольник так, чтобы его стороны сдвинулись на полклетки наружу
                double a = dist(corners[0], corners[1]);
                double b = dist(corners[1], corners[2]);
                double c = dist(corners[2], corners[0]);
                double inRadius = t.calc_area() / ((a + b + c) / 2);
                double k = (inRadius + 0.5) / inRadius;
                shape = new Triangle(a * k, b * k, c * k);
            }
            else {
                shape = new Triangle(corners[0], corners[1], corners[2]);
            }
            return FIG_TRIANGLE;
        }
        if (corners.size() == 4) {
            double w, h;
            if (looksLikeRectangle(corners, w, h)) {
                if (filled) {
                    w += 1;
                    h += 1;
                }
                shape = new Rectangle(w, h);
                return FIG_RECTANGLE;
            }
        }
    }
    catch (ShapeException&) {
        shape = nullptr;
    }
    return FIG_NOISE;
}

// ---------- основная функция ----------

bool countFigures(const string& inputFile, FigureCount& result) {
    result = FigureCount();

    vector<vector<bool> > drawn;
    if (!readImage(inputFile, drawn)) return false;

    int rows = (int)drawn.size();
    int cols = (int)drawn[0].size();

    // поиск связных групп клеток (соседние по сторонам и по диагонали)
    vector<vector<int> > label(rows, vector<int>(cols, 0));
    int labelCount = 0;

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            if (!drawn[r][c] || label[r][c] != 0) continue;

            Component comp;
            comp.label = ++labelCount;
            comp.minX = comp.maxX = c;
            comp.minY = comp.maxY = r;

            vector<pair<int, int> > stack;
            stack.push_back(make_pair(r, c));
            label[r][c] = comp.label;
            while (!stack.empty()) {
                pair<int, int> cur = stack.back();
                stack.pop_back();
                comp.pixels.push_back(Point(cur.second, cur.first));
                comp.minX = min(comp.minX, cur.second);
                comp.maxX = max(comp.maxX, cur.second);
                comp.minY = min(comp.minY, cur.first);
                comp.maxY = max(comp.maxY, cur.first);

                for (int dr = -1; dr <= 1; dr++)
                    for (int dc = -1; dc <= 1; dc++) {
                        int nr = cur.first + dr, nc = cur.second + dc;
                        if (nr < 0 || nr >= rows || nc < 0 || nc >= cols) continue;
                        if (!drawn[nr][nc] || label[nr][nc] != 0) continue;
                        label[nr][nc] = comp.label;
                        stack.push_back(make_pair(nr, nc));
                    }
            }

            // короткие черточки и точки в расчет не принимаем
            int w = comp.maxX - comp.minX + 1;
            int h = comp.maxY - comp.minY + 1;
            if (max(w, h) < MIN_FIGURE_SIZE) continue;

            Shape* shape = nullptr;
            FigureType type = classify(comp, label, shape);

            if (type == FIG_RECTANGLE) {
                result.rectangles++;
                result.areaRectangles += shape->calc_area();
            }
            else if (type == FIG_CIRCLE) {
                result.circles++;
                result.areaCircles += shape->calc_area();
            }
            else if (type == FIG_TRIANGLE) {
                result.triangles++;
                result.areaTriangles += shape->calc_area();
            }
            else {
                result.noise++;
            }
            delete shape;
        }
    }
    return true;
}

bool writeFigureCount(const string& outputFile, const FigureCount& result) {
    ofstream out(outputFile.c_str());
    if (!out.is_open()) return false;
    out << "Rectangle = " << result.rectangles << "\n";
    out << "Circle = " << result.circles << "\n";
    out << "Triangle = " << result.triangles << "\n";
    out << "Noise = " << result.noise << "\n";
    return true;
}
