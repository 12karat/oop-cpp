#include <iostream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include "Function.h"

using namespace std;

// ---------- вспомогательные функции для вывода формул ----------

// число в строку
static string num(double v) {
    ostringstream s;
    s << v;
    return s.str();
}

// число со знаком для формулы: " + 3" или " - 3"
static string signedNum(double v) {
    if (v < 0) return " - " + num(-v);
    return " + " + num(v);
}

// ---------- общие методы класса Function ----------

// по умолчанию функция определена на любом интервале
void Function::checkInterval(double, double) const {
}

// вывод результата
void Function::printResult(double x) const {
    cout << getName() << ": " << getFormula() << endl;
    cout << "f(" << x << ") = " << calculate(x) << endl;
}

// общий алгоритм: перебираем точки интервала с шагом step и ищем наибольшее/наименьшее значение
double Function::findExtremum(double a, double b, double* xAt, double step, bool wantMax) const {
    if (step <= 0)
        throw FunctionException("Шаг должен быть больше нуля");
    if (a > b) swap(a, b);
    checkInterval(a, b);

    int n = (int)ceil((b - a) / step - 1e-9); // число шагов
    if (n < 1) n = 1;
    double h = (b - a) / n;

    double bestX = a;
    double best = calculate(a);
    for (int i = 1; i <= n; i++) {
        double x = (i == n) ? b : a + i * h; // последняя точка - ровно b
        double val = calculate(x);
        if (wantMax ? (val > best) : (val < best)) {
            best = val;
            bestX = x;
        }
    }
    if (xAt != nullptr) *xAt = bestX;
    return best;
}

// поиск максимума
double Function::findMax(double a, double b, double* xAt, double step) const {
    return findExtremum(a, b, xAt, step, true);
}

// поиск минимума
double Function::findMin(double a, double b, double* xAt, double step) const {
    return findExtremum(a, b, xAt, step, false);
}

// интегрирование (формула Симпсона)
double Function::integrate(double a, double b, int n) const {
    if (n < 1)
        throw FunctionException("Число разбиений должно быть больше нуля");
    if (n % 2 == 1) n++; // для формулы Симпсона нужно четное число разбиений

    double lo = min(a, b), hi = max(a, b);
    checkInterval(lo, hi);

    double h = (b - a) / n; // ширина одного шага (если a > b, интеграл получится со знаком минус)
    double sum = calculate(a) + calculate(b);
    for (int i = 1; i < n; i++) {
        double x = a + i * h;
        sum += (i % 2 == 1 ? 4 : 2) * calculate(x);
    }
    return sum * h / 3;
}

// дифференцирование
double Function::differentiate(double x, double h) const {
    if (h <= 0)
        throw FunctionException("Шаг h должен быть больше нуля");
    checkInterval(x - h, x + h);
    return (calculate(x + h) - calculate(x - h)) / (2 * h); // формула центральной разности
}

// ---------- гипербола ----------

double Hiperbola::calculate(double x) const {
    if (x == 0)
        throw FunctionException("Гипербола не определена в точке x = 0");
    return k / x + b;
}

void Hiperbola::checkInterval(double lo, double hi) const {
    if (lo <= 0 && hi >= 0)
        throw FunctionException("Гипербола не определена в точке x = 0, а она лежит внутри интервала");
}

string Hiperbola::getFormula() const {
    return "y = " + num(k) + "/x" + signedNum(b);
}

// ---------- парабола ----------

double Parabola::calculate(double x) const {
    return a * x * x + b * x + c;
}

string Parabola::getFormula() const {
    return "y = " + num(a) + "*x^2" + signedNum(b) + "*x" + signedNum(c);
}

// ---------- экспонента ----------

double Exponenta::calculate(double x) const {
    return a * exp(b * x);
}

string Exponenta::getFormula() const {
    return "y = " + num(a) + "*e^(" + num(b) + "*x)";
}

// ---------- полином ----------

Polinom::Polinom(const vector<double>& c) : coeffs(c) {
    if (coeffs.empty())
        throw FunctionException("У полинома должен быть хотя бы один коэффициент");
}

int Polinom::getDegree() const {
    for (int i = (int)coeffs.size() - 1; i > 0; i--)
        if (coeffs[i] != 0) return i;
    return 0;
}

// схема Горнера: c0 + x*(c1 + x*(c2 + ...))
double Polinom::calculate(double x) const {
    double res = 0;
    for (int i = (int)coeffs.size() - 1; i >= 0; i--)
        res = res * x + coeffs[i];
    return res;
}

string Polinom::getFormula() const {
    string s = "y =";
    bool first = true;
    for (int i = (int)coeffs.size() - 1; i >= 0; i--) {
        if (coeffs[i] == 0) continue;
        if (first) {
            s += " " + num(coeffs[i]);
        }
        else {
            s += signedNum(coeffs[i]);
        }
        if (i >= 1) s += "*x";
        if (i >= 2) s += "^" + to_string(i);
        first = false;
    }
    if (first) s += " 0"; // все коэффициенты нулевые
    return s;
}
