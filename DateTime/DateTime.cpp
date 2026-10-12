#define _CRT_SECURE_NO_WARNINGS
#include "DateTime.h"
#include <cstdio>
#include <cmath>

// ---------- вспомогательные функции ----------

bool DateTime::isLeap(int year)
{
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int DateTime::daysInMonth(int year, int month)
{
    int d[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if (month == 2 && isLeap(year))
        return 29;
    return d[month - 1];
}

int DateTime::compare(const DateTime& other) const
{
    if (days < other.days) return -1;
    if (days > other.days) return 1;
    if (fabs(time - other.time) < 1e-9) return 0;
    if (time < other.time) return -1;
    return 1;
}

// ---------- конструкторы ----------

DateTime::DateTime()
{
    days = 0;
    time = 0;
    valid = true;   // 01.01.1900 00:00:00
}

DateTime::DateTime(int y, int m, int d, int h, int mi, int s)
{
    days = 0;
    time = 0;
    valid = false;

    if (!checkDate(y, m, d) || !checkTime(h, mi, s))
        return;

    // считаем дни от 01.01.1900
    int count = 0;
    for (int i = 1900; i < y; i++)
    {
        if (isLeap(i)) count += 366;
        else count += 365;
    }
    for (int i = 1; i < m; i++)
        count += daysInMonth(y, i);
    count += d - 1;

    days = count;
    time = (h * 3600 + mi * 60 + s) / 86400.0;
    valid = true;
}

DateTime::DateTime(const string& str)
{
    setFromString(str);
}

// ---------- проверки ----------

bool DateTime::checkDate(int y, int m, int d)
{
    if (y < 1900) return false;           // раньше опорной даты нельзя
    if (m < 1 || m > 12) return false;
    if (d < 1 || d > daysInMonth(y, m)) return false;
    return true;
}

bool DateTime::checkTime(int h, int m, int s)
{
    if (h < 0 || h > 23) return false;
    if (m < 0 || m > 59) return false;
    if (s < 0 || s > 59) return false;
    return true;
}

bool DateTime::isValid() const
{
    return valid;
}

// ---------- ввод ----------

bool DateTime::setFromString(const string& str)
{
    int y, m, d, h, mi, s;

    // yyyy-MM-ddThh:mm:ss
    if (str.length() == 19 && sscanf(str.c_str(), "%d-%d-%dT%d:%d:%d", &y, &m, &d, &h, &mi, &s) == 6)
    {
        *this = DateTime(y, m, d, h, mi, s);
        return valid;
    }
    // yyyy-MM-dd
    if (str.length() == 10 && sscanf(str.c_str(), "%d-%d-%d", &y, &m, &d) == 3)
    {
        *this = DateTime(y, m, d);
        return valid;
    }
    // hh:mm:ss  (дата при этом 01.01.1900)
    if (str.length() == 8 && sscanf(str.c_str(), "%d:%d:%d", &h, &mi, &s) == 3)
    {
        *this = DateTime(1900, 1, 1, h, mi, s);
        return valid;
    }

    // формат не подошел
    days = 0;
    time = 0;
    valid = false;
    return false;
}

void DateTime::input()
{
    string str;
    cin >> str;
    setFromString(str);
}

// ---------- получение частей ----------

void DateTime::getDate(int& y, int& m, int& d) const
{
    int rest = days;

    y = 1900;
    while (rest >= (isLeap(y) ? 366 : 365))
    {
        rest -= isLeap(y) ? 366 : 365;
        y++;
    }

    m = 1;
    while (rest >= daysInMonth(y, m))
    {
        rest -= daysInMonth(y, m);
        m++;
    }

    d = rest + 1;
}

void DateTime::getTime(int& h, int& m, int& s) const
{
    int total = (int)(time * 86400.0 + 0.5);   // округляем до секунд
    if (total >= 86400) total = 86399;

    h = total / 3600;
    m = (total % 3600) / 60;
    s = total % 60;
}

// ---------- день недели ----------

int DateTime::dayOfWeek() const
{
    // 01.01.1900 - понедельник
    return days % 7 + 1;
}

string DateTime::dayOfWeekName() const
{
    string names[7] = { "понедельник", "вторник", "среда", "четверг",
                        "пятница", "суббота", "воскресенье" };
    return names[days % 7];
}

// ---------- пасха ----------

DateTime DateTime::easterCatholic() const
{
    int y, m, d;
    getDate(y, m, d);

    // алгоритм Гаусса (григорианский календарь)
    int a = y % 19;
    int b = y / 100;
    int c = y % 100;
    int dd = b / 4;
    int e = b % 4;
    int f = (b + 8) / 25;
    int g = (b - f + 1) / 3;
    int h = (19 * a + b - dd - g + 15) % 30;
    int i = c / 4;
    int k = c % 4;
    int l = (32 + 2 * e + 2 * i - h - k) % 7;
    int mm = (a + 11 * h + 22 * l) / 451;
    int month = (h + l - 7 * mm + 114) / 31;
    int day = (h + l - 7 * mm + 114) % 31 + 1;

    return DateTime(y, month, day);
}

DateTime DateTime::easterOrthodox() const
{
    int y, m, d;
    getDate(y, m, d);

    // алгоритм Меуса (юлианский календарь)
    int a = y % 4;
    int b = y % 7;
    int c = y % 19;
    int dd = (19 * c + 15) % 30;
    int e = (2 * a + 4 * b - dd + 34) % 7;
    int month = (dd + e + 114) / 31;
    int day = (dd + e + 114) % 31 + 1;

    // переводим в григорианский календарь (+13 дней для 1900-2099 гг.)
    return DateTime(y, month, day) + 13;
}

// ---------- вывод ----------

string DateTime::dateToString() const
{
    int y, m, d;
    getDate(y, m, d);

    char buf[20];
    sprintf(buf, "%04d-%02d-%02d", y, m, d);
    return string(buf);
}

string DateTime::timeToString() const
{
    int h, m, s;
    getTime(h, m, s);

    char buf[20];
    sprintf(buf, "%02d:%02d:%02d", h, m, s);
    return string(buf);
}

string DateTime::toString() const
{
    return dateToString() + "T" + timeToString();
}

// ---------- операторы ----------

double DateTime::operator-(const DateTime& other) const
{
    return (days + time) - (other.days + other.time);
}

DateTime DateTime::operator+(int d) const
{
    DateTime result = *this;
    result.days = days + d;
    if (result.days < 0)        // ушли раньше 01.01.1900
        result.valid = false;
    return result;
}

bool DateTime::operator>(const DateTime& other) const
{
    return compare(other) > 0;
}

bool DateTime::operator>=(const DateTime& other) const
{
    return compare(other) >= 0;
}

bool DateTime::operator<(const DateTime& other) const
{
    return compare(other) < 0;
}

bool DateTime::operator<=(const DateTime& other) const
{
    return compare(other) <= 0;
}

bool DateTime::operator==(const DateTime& other) const
{
    return compare(other) == 0;
}

bool DateTime::operator!=(const DateTime& other) const
{
    return compare(other) != 0;
}

ostream& operator<<(ostream& out, const DateTime& dt)
{
    if (dt.valid)
        out << dt.toString();
    else
        out << "некорректная дата";
    return out;
}

istream& operator>>(istream& in, DateTime& dt)
{
    string str;
    in >> str;
    dt.setFromString(str);
    return in;
}
