#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <cstdlib>
#include <cmath>
#include "Function.h"

using namespace std;

// очень маленькие числа (ошибка округления) выводим как 0, чтобы не было "-0.0000"
double tidy(double v) {
    return fabs(v) < 1e-9 ? 0.0 : v;
}

// полная демонстрация возможностей функции на интервале [a, b] и в точке x0
void demo(const Function* f, double a, double b, double x0) {
    cout << "----------------------------------------" << endl;
    f->printResult(x0);                 // вывод значения в точке
    cout << "Интервал [" << a << ", " << b << "]" << endl;

    try {
        double xMin, xMax;
        double mn = f->findMin(a, b, &xMin);
        double mx = f->findMax(a, b, &xMax);
        cout << "Минимум:  " << tidy(mn) << " (при x = " << xMin << ")" << endl;
        cout << "Максимум: " << tidy(mx) << " (при x = " << xMax << ")" << endl;
        cout << "Интеграл на [" << a << ", " << b << "]: " << tidy(f->integrate(a, b)) << endl;
        cout << "Производная в x = " << x0 << ": " << tidy(f->differentiate(x0)) << endl;
    }
    catch (FunctionException& e) {
        cout << "Ошибка: " << e.message << endl;
    }
    cout << endl;
}

// безопасный ввод числа с клавиатуры
double readDouble(const string& prompt) {
    double v;
    while (true) {
        cout << prompt;
        if (cin >> v) return v;
        cout << "Нужно ввести число, попробуйте еще раз" << endl;
        cin.clear();
        cin.ignore(10000, '\n');
    }
}

int main() {
#ifdef _WIN32
    // чтобы русский текст нормально отображался в консоли
    system("chcp 65001 > nul");
#endif
    cout << fixed << setprecision(4);

    // ---------- 1. Демонстрация ----------
    cout << "1. Демонстрация классов (виртуальные методы вызываются через Function*)" << endl;
    cout << endl;

    // создаем список функций через указатели на базовый класс
    vector<Function*> list;
    list.push_back(new Hiperbola(10, 2));                      // y = 10/x + 2
    list.push_back(new Parabola(1, -2, 1));                    // y = x^2 - 2x + 1
    list.push_back(new Exponenta(1, 0.5));                     // y = e^(0.5x)
    list.push_back(new Polinom({ 1, 2, 1 }));                  // y = 1 + 2x + x^2
    list.push_back(new Polinom({ 0, -3, 0, 1 }));              // y = x^3 - 3x      (3-й степени)
    list.push_back(new Polinom({ 2, 0, -5, 0, 1 }));           // y = x^4 - 5x^2 + 2 (4-й степени)

    // интервалы и точки для каждой функции
    double intervals[6][3] = {
        { 1, 4, 2 },     // гипербола: интервал без нуля
        { 0, 3, 1.5 },
        { 0, 2, 1.5 },
        { -3, 1, 0.5 },
        { -2, 2, 1.5 },
        { -3, 3, 1 }
    };
    for (size_t i = 0; i < list.size(); i++)
        demo(list[i], intervals[i][0], intervals[i][1], intervals[i][2]);

    // гипербола на интервале, где она не определена (в нуле)
    cout << "Гипербола на интервале с точкой x = 0:" << endl;
    demo(list[0], -1, 2, 1);

    // очистка памяти (деструктор виртуальный)
    for (size_t i = 0; i < list.size(); i++) delete list[i];

    // ---------- 2. Свои коэффициенты ----------
    cout << "2. Ввод собственных коэффициентов" << endl;
    int choice = -1;
    while (choice != 0) {
        cout << "1 - Hiperbola (k/x + b)" << endl;
        cout << "2 - Parabola (a*x^2 + b*x + c)" << endl;
        cout << "3 - Exponenta (a*e^(b*x))" << endl;
        cout << "4 - Polinom n-й степени" << endl;
        cout << "0 - выход" << endl;
        choice = (int)readDouble("Выберите функцию: ");

        Function* f = nullptr;
        try {
            if (choice == 1) {
                double k = readDouble("k = ");
                double b = readDouble("b = ");
                f = new Hiperbola(k, b);
            }
            else if (choice == 2) {
                double a = readDouble("a = ");
                double b = readDouble("b = ");
                double c = readDouble("c = ");
                f = new Parabola(a, b, c);
            }
            else if (choice == 3) {
                double a = readDouble("a = ");
                double b = readDouble("b = ");
                f = new Exponenta(a, b);
            }
            else if (choice == 4) {
                int n = (int)readDouble("Степень n = ");
                if (n < 0) {
                    cout << "Степень не может быть отрицательной" << endl << endl;
                    continue;
                }
                vector<double> c;
                for (int i = 0; i <= n; i++)
                    c.push_back(readDouble("Коэффициент при x^" + to_string(i) + " = "));
                f = new Polinom(c);
            }
            else if (choice != 0) {
                cout << "Нет такого пункта" << endl << endl;
                continue;
            }
            else {
                break;
            }

            double x0 = readDouble("Точка x = ");
            double a = readDouble("Начало интервала = ");
            double b = readDouble("Конец интервала = ");
            cout << endl;
            demo(f, a, b, x0);
        }
        catch (FunctionException& e) {
            cout << "Ошибка: " << e.message << endl << endl;
        }
        delete f;
    }

    return 0;
}
