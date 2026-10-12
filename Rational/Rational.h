#pragma once
#include <iostream>

using namespace std;

// исключение для обработки ошибок (деление на ноль, переполнение int и т.д.)
class RationalException {
public:
    const char* message; // текст ошибки
    RationalException(const char* msg) : message(msg) {}
};

class Rational {
private:
    // числитель
    int numer;
    // знаменатель
    int denom;
    // функция упрощения
    void simplify();
    // запись дроби n/d: сокращает дробь и проверяет, что числитель и знаменатель
    // не превзошли допустимые значения для int (иначе бросает исключение)
    void setValue(long long n, long long d);

public:
    // конструктор по умолчанию
    Rational();
    // конструктор из целого (преобразующий)
    Rational(int number);
    // конструктор с двумя аргументами
    Rational(int n, int d);

    // арифметические операции
    Rational& operator =(const Rational& r);
    Rational& operator +=(const Rational& r);
    Rational operator +(const Rational& r) const;
    Rational operator -() const;
    Rational operator -(const Rational& r) const;
    Rational& operator -=(const Rational& r);
    Rational& operator *=(const Rational& r);
    Rational operator *(const Rational& r) const;
    Rational& operator /=(const Rational& r);
    Rational operator /(const Rational& r) const;

    // операторы инкремента
    Rational& operator ++(); // префикс
    Rational operator ++(int); // постфикс

    // операторы сравнения
    bool operator ==(const Rational& r) const;
    bool operator !=(const Rational& r) const;
    bool operator >(const Rational& r) const;
    bool operator <(const Rational& r) const;
    bool operator >=(const Rational& r) const;
    bool operator <=(const Rational& r) const;

    // Преобразование типов
    operator int() const;
    operator double() const;

    // ---------- для решения квадратного уравнения ----------

    // квадратный корень (итерационный алгоритм Древнего Вавилона), все вычисления в Rational
    Rational sqrt() const;

    // дискриминант D = b^2 - 4ac
    static Rational discriminant(const Rational& a, const Rational& b, const Rational& c);

    // решение уравнения a*x^2 + b*x + c = 0
    // возвращает количество корней: 0, 1 или 2 (-1 - корней бесконечно много)
    // корни записываются в x1 и x2
    static int solveQuadratic(const Rational& a, const Rational& b, const Rational& c,
                              Rational& x1, Rational& x2);

    // Ввод-вывод 
    friend istream& operator >>(istream& in, Rational& r);
    friend ostream& operator <<(ostream& out, const Rational& r);
};
