#pragma once
#include <vector>
#include <string>

// исключение для ошибок (функция не определена на интервале, неверные параметры и т.д.)
class FunctionException {
public:
    const char* message; // текст ошибки
    FunctionException(const char* msg) : message(msg) {}
};

// абстрактный базовый класс «Функция»
class Function {
protected:
    // общий алгоритм поиска экстремума (wantMax = true - максимум, false - минимум)
    double findExtremum(double a, double b, double* xAt, double step, bool wantMax) const;

public:
    virtual ~Function() {} // виртуальный деструктор

    // вычисление значения функции y = f(x) в точке x
    virtual double calculate(double x) const = 0;
    // название и формула функции
    virtual std::string getName() const = 0;
    virtual std::string getFormula() const = 0;

    // проверка, что функция определена на всем интервале [a, b]
    // (если нет - бросает исключение); по умолчанию функция определена везде
    virtual void checkInterval(double a, double b) const;

    // вывод результата на экран
    virtual void printResult(double x) const;
    // поиск максимума на интервале [a, b]; в xAt (если задан) записывается точка максимума
    virtual double findMax(double a, double b, double* xAt = nullptr, double step = 0.01) const;
    // поиск минимума на интервале [a, b]
    virtual double findMin(double a, double b, double* xAt = nullptr, double step = 0.01) const;
    // интегрирование на интервале [a, b] (метод Симпсона, n - число разбиений)
    virtual double integrate(double a, double b, int n = 1000) const;
    // дифференцирование: производная в точке x (центральная разность)
    virtual double differentiate(double x, double h = 1e-5) const;
};

// гипербола: y = k/x + b
class Hiperbola : public Function {
private:
    double k, b;
public:
    Hiperbola(double k, double b) : k(k), b(b) {}
    double calculate(double x) const;
    std::string getName() const { return "Hiperbola"; }
    std::string getFormula() const;
    void checkInterval(double a, double b) const; // в точке x = 0 функция не определена
};

// парабола: y = a*x^2 + b*x + c
class Parabola : public Function {
private:
    double a, b, c;
public:
    Parabola(double a, double b, double c) : a(a), b(b), c(c) {}
    double calculate(double x) const;
    std::string getName() const { return "Parabola"; }
    std::string getFormula() const;
};

// экспонента: y = a * e^(b*x)
class Exponenta : public Function {
private:
    double a, b;
public:
    Exponenta(double a, double b) : a(a), b(b) {}
    double calculate(double x) const;
    std::string getName() const { return "Exponenta"; }
    std::string getFormula() const;
};

// полином n-й степени: y = c0 + c1*x + c2*x^2 + ... + cn*x^n
class Polinom : public Function {
private:
    std::vector<double> coeffs; // coeffs[i] - коэффициент при x^i
public:
    Polinom(const std::vector<double>& c);
    int getDegree() const; // степень полинома
    double calculate(double x) const;
    std::string getName() const { return "Polinom"; }
    std::string getFormula() const;
};
