#include <iostream>
#include <climits>
#include "Rational.h"

using namespace std;

// ---------- вспомогательные функции ----------

// наибольший общий делитель (алгоритм Евклида)
static long long gcdLL(long long a, long long b)
{
    if (a < 0) a = -a;
    if (b < 0) b = -b;
    while (b != 0)
    {
        long long t = a % b;
        a = b;
        b = t;
    }
    return a;
}

// целая часть квадратного корня из n (без double)
static int intSqrt(int n)
{
    long long lo = 0, hi = 46341; // 46341^2 уже больше INT_MAX
    while (lo < hi)
    {
        long long mid = (lo + hi + 1) / 2;
        if (mid * mid <= n) lo = mid;
        else hi = mid - 1;
    }
    return (int)lo;
}

// ---------- конструкторы ----------

// конструктор по умолчанию
Rational::Rational()
{
    numer = 0;
    denom = 1;
}

// конструктор из целого (преобразующий)
Rational::Rational(int number)
{
    setValue(number, 1);
}

// конструктор с двумя аргументами
Rational::Rational(int n, int d)
{
    setValue(n, d);
}

// запись дроби с сокращением и контролем переполнения
void Rational::setValue(long long n, long long d)
{
    if (d == 0)
        throw RationalException("Знаменатель равен нулю");
    if (d < 0)
    {
        n = -n;
        d = -d;
    }
    long long g = gcdLL(n, d); // для n = 0 получится g = d
    n /= g;
    d /= g;

    // контроль: числитель и знаменатель не должны превзойти допустимые значения для int
    if (n > INT_MAX || n < -INT_MAX || d > INT_MAX)
        throw RationalException("Переполнение: числитель или знаменатель не помещается в int");

    numer = (int)n;
    denom = (int)d;
}

// функция упрощения
void Rational::simplify()
{
    setValue(numer, denom);
}

// оператор присваивания
Rational& Rational::operator =(const Rational& r)
{
    this->numer = r.numer;
    this->denom = r.denom;

    return *this;
}

// ---------- арифметика ----------
// промежуточные результаты считаем в long long (произведение двух int в него помещается),
// а перед записью проверяем, что результат помещается в int

// оператор добавления
Rational& Rational::operator +=(const Rational& r)
{
    long long g = gcdLL(denom, r.denom);
    long long n = (long long)numer * (r.denom / g) + (long long)r.numer * (denom / g);
    long long d = (long long)denom / g * r.denom;
    setValue(n, d);
    return *this;
}

// оператор умножения
Rational& Rational::operator *=(const Rational& r)
{
    // сначала сокращаем "крест-накрест", чтобы числа были как можно меньше
    long long g1 = gcdLL(numer, r.denom);
    long long g2 = gcdLL(r.numer, denom);
    long long n = (long long)(numer / g1) * (r.numer / g2);
    long long d = (long long)(denom / g2) * (r.denom / g1);
    setValue(n, d);
    return *this;
}

// оператор умножения (результат - новый объект)
Rational Rational::operator *(const Rational& r) const
{
    Rational res(*this);
    return res *= r;
}

// оператор деления
Rational& Rational::operator /=(const Rational& r)
{
    if (r.numer == 0)
        throw RationalException("Деление на ноль");
    // делить на дробь - значит умножать на перевернутую дробь
    Rational inverse(r.denom, r.numer);
    return (*this *= inverse);
}

// оператор деления (результат - новый объект)
Rational Rational::operator /(const Rational& r) const
{
    Rational res(*this);
    return res /= r;
}

// оператор сложения
Rational Rational::operator +(const Rational& r) const
{
    Rational res(*this);
    // используем готовую операцию добавления
    return res += r;
}

// оператор отрицания (унарный минус)
Rational Rational::operator -() const
{
    Rational r(-numer, denom);
    return r;
}

// оператор вычитания
Rational Rational::operator -(const Rational& r) const
{
    return (*this + (-r));
}

// оператор уменьшения
Rational& Rational::operator -=(const Rational& r)
{
    return (*this += (-r));
}

// префикс
Rational& Rational::operator ++()
{
    setValue((long long)numer + denom, denom);
    return *this;
}

// постфикс
Rational Rational::operator ++(int)
{
    Rational r(*this);
    ++(*this);
    return r;
}

// ---------- сравнение ----------
// дроби всегда сокращены, поэтому == можно сравнивать по частям

bool Rational::operator ==(const Rational& r) const
{
    return (numer == r.numer) && (denom == r.denom);
}

bool Rational::operator !=(const Rational& r) const
{
    return !(*this == r);
}

// для >, < приводим к общему знаменателю в long long, чтобы не было переполнения
bool Rational::operator >(const Rational& r) const
{
    return ((long long)numer * r.denom) > ((long long)r.numer * denom);
}

bool Rational::operator <(const Rational& r) const
{
    return ((long long)numer * r.denom) < ((long long)r.numer * denom);
}

bool Rational::operator >=(const Rational& r) const
{
    return !(*this < r);
}

bool Rational::operator <=(const Rational& r) const
{
    return !(*this > r);
}

// преобразования типов
Rational::operator int() const
{
    return numer / denom;
}

Rational::operator double() const
{
    return ((double)numer) / denom;
}

// ---------- квадратное уравнение ----------

// квадратный корень методом Древнего Вавилона:
// x0 - начальное приближение, x(n+1) = (x(n) + a / x(n)) / 2
Rational Rational::sqrt() const
{
    if (numer < 0)
        throw RationalException("Корень из отрицательного числа");
    if (numer == 0)
        return Rational(0);

    // начальное приближение: целый корень числителя / целый корень знаменателя
    // (для 5 получится x0 = 2, как в примере из задания)
    Rational x(intSqrt(numer), intSqrt(denom));

    const int MAX_ITER = 100;
    for (int i = 0; i < MAX_ITER; i++)
    {
        Rational next;
        try
        {
            next = (x + *this / x) / Rational(2);
        }
        catch (RationalException&)
        {
            // следующее приближение уже не помещается в int, останавливаемся
            // и возвращаем последнее найденное значение
            break;
        }
        if (next == x) // значение больше не меняется (корень найден точно)
            break;
        x = next;
    }
    return x;
}

// дискриминант
Rational Rational::discriminant(const Rational& a, const Rational& b, const Rational& c)
{
    return b * b - Rational(4) * a * c;
}

// решение квадратного уравнения a*x^2 + b*x + c = 0
int Rational::solveQuadratic(const Rational& a, const Rational& b, const Rational& c,
                             Rational& x1, Rational& x2)
{
    Rational zero(0);

    // a = 0: уравнение линейное b*x + c = 0
    if (a == zero)
    {
        if (b == zero)
        {
            if (c == zero) return -1; // 0 = 0, подходит любое x
            return 0;                 // c = 0 при c != 0, корней нет
        }
        x1 = (-c) / b;
        x2 = x1;
        return 1;
    }

    Rational d = discriminant(a, b, c);
    Rational twoA = Rational(2) * a;

    if (d < zero)
        return 0; // действительных корней нет

    if (d == zero)
    {
        x1 = (-b) / twoA;
        x2 = x1;
        return 1;
    }

    Rational s = d.sqrt();
    x1 = ((-b) + s) / twoA;
    x2 = ((-b) - s) / twoA;
    return 2;
}

// ---------- ввод-вывод ----------

// операторы ввода-вывода
istream& operator >>(istream& in, Rational& r)
{
    int n, d;
    in >> n >> d;
    if (in)
    {
        try
        {
            r.setValue(n, d);
        }
        catch (RationalException&)
        {
            in.setstate(ios::failbit); // нулевой знаменатель или слишком большое число
        }
    }
    return in;
}

ostream& operator <<(ostream& out, const Rational& r)
{
    out << r.numer << "/" << r.denom;
    return out;
}
