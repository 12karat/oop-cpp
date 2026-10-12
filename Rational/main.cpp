// windows.h подключаем ПЕРВЫМ, до "using namespace std" (иначе конфликт имен byte)
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <iostream>
#include <climits>
#include "Rational.h"

using namespace std;

// решение одного квадратного уравнения с выводом результата
void solveAndPrint(const Rational& a, const Rational& b, const Rational& c) {
    Rational zero(0);
    cout << "Уравнение: (" << a << ")*x^2 + (" << b << ")*x + (" << c << ") = 0" << endl;

    try {
        // дискриминант (если a = 0, уравнение линейное, дискриминант не нужен)
        if (a != zero) {
            Rational d = Rational::discriminant(a, b, c);
            cout << "Дискриминант D = " << d << endl;
        }

        Rational x1, x2;
        int count = Rational::solveQuadratic(a, b, c, x1, x2);

        if (count == -1) {
            cout << "Корней бесконечно много (подходит любое x)" << endl;
        }
        else if (count == 0) {
            cout << "Действительных корней нет" << endl;
        }
        else if (count == 1) {
            // (double) используется только для красивого вывода, расчет шел в Rational
            cout << "Один корень: x = " << x1 << "  (" << (double)x1 << ")" << endl;
        }
        else {
            cout << "x1 = " << x1 << "  (" << (double)x1 << ")" << endl;
            cout << "x2 = " << x2 << "  (" << (double)x2 << ")" << endl;

            // если sqrt(D) не извлекается точно, корни получены приближенно
            Rational d = Rational::discriminant(a, b, c);
            Rational s = d.sqrt();
            bool exact = false;
            try {
                exact = (s * s == d);
            }
            catch (RationalException&) {
                exact = false;
            }
            if (!exact)
                cout << "(sqrt(D) - иррациональное число, корни найдены приближенно)" << endl;
        }
    }
    catch (RationalException& e) {
        cout << "Ошибка: " << e.message << endl;
    }
}

int main() {
#ifdef _WIN32
    // чтобы русский текст нормально отображался в консоли
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    // ---------- проверка класса Rational ----------

    // тестируем конструкторы и вывод
    Rational a(1, 2), b(-1, 6);
    cout << "Тест конструкторов и вывода" << endl;
    cout << "a = " << a << ", b = " << b << endl;
    cout << "Rational(2, 4) = " << Rational(2, 4) << " (дробь сокращается)" << endl;

    // тестируем сложение и упрощение
    cout << "\nТест сложения (a + b)" << endl;
    cout << "a + b = " << a + b << endl;

    // тестируем оператор += (через += и упрощение)
    cout << "\nТест присваивания (a += b)" << endl;
    a += b;
    cout << "Результат a += b: " << a << endl;

    // тестируем умножение и деление
    cout << "\nТест умножения и деления" << endl;
    Rational m(2, 3), n(3, 4);
    cout << "(" << m << ") * (" << n << ") = " << m * n << endl;
    cout << "(" << m << ") / (" << n << ") = " << m / n << endl;

    // тестируем сравнение
    cout << "\nТест сравнения (2/3 и 3/4)" << endl;
    cout << "2/3 < 3/4  : " << (m < n) << endl;
    cout << "2/3 >= 3/4 : " << (m >= n) << endl;
    cout << "2/3 <= 3/4 : " << (m <= n) << endl;
    cout << "2/4 == 1/2 : " << (Rational(2, 4) == Rational(1, 2)) << endl;

    // тестируем преобразование в double 
    cout << "\nТест double" << endl;
    cout << "Число b в десятичном виде: " << (double)b << endl;

    // тестируем преобразование из целого числа 
    Rational c = 3;
    cout << "\nТест преобразования из int" << endl;
    cout << "c = 3 -> " << c << endl;

    // тестируем сложение дробей
    Rational e(7, 8), f(5, 12);
    cout << "\nТест сложения" << endl;
    cout << "e = " << e << ", f = " << f << ", e + f = " << e + f << endl;

    // тестируем квадратный корень (пример из задания: корень из 5, x0 = 2)
    cout << "\nТест квадратного корня (Rational, без double)" << endl;
    Rational five(5);
    Rational root = five.sqrt();
    cout << "sqrt(5) = " << root << " = " << (double)root << endl;
    cout << "sqrt(9/16) = " << Rational(9, 16).sqrt() << endl;
    cout << "sqrt(2) = " << Rational(2).sqrt() << " = " << (double)Rational(2).sqrt() << endl;

    // тестируем контроль переполнения
    cout << "\nТест контроля переполнения" << endl;
    try {
        Rational big(INT_MAX);
        Rational sum = big + Rational(1); // здесь произойдет переполнение
        cout << "INT_MAX + 1 = " << sum << endl;
    }
    catch (RationalException& ex) {
        cout << "INT_MAX + 1 -> " << ex.message << endl;
    }

    // ---------- решение квадратного уравнения ----------

    cout << "\nРешение квадратного уравнения a*x^2 + b*x + c = 0" << endl;
    cout << "Коэффициенты вводятся как дроби: числитель и знаменатель через пробел" << endl;
    cout << "(например, 3 4 означает 3/4, а 5 1 означает 5)" << endl;

    int again = 1;
    while (again == 1) {
        Rational ca, cb, cc;

        cout << endl << "Введите a: ";
        cin >> ca;
        cout << "Введите b: ";
        cin >> cb;
        cout << "Введите c: ";
        cin >> cc;

        if (!cin) {
            cout << "Ошибка ввода (знаменатель не может быть 0, числа не должны быть слишком большими)" << endl;
            break;
        }

        solveAndPrint(ca, cb, cc);

        cout << endl << "Решить еще одно уравнение? (1 - да, 0 - нет): ";
        cin >> again;
    }

    return 0;
}
