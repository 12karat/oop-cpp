#ifndef DATETIME_H
#define DATETIME_H

#include <iostream>
#include <string>
using namespace std;

// Класс для работы с датой и временем.
// Дата хранится как количество дней от опорной даты 01.01.1900,
// время хранится как дробная часть суток (12:00:00 = 0.5).
class DateTime
{
private:
    int days;      // дней от 01.01.1900
    double time;   // доля суток (от 0 до 1)
    bool valid;    // корректна ли дата

    static bool isLeap(int year);                 // високосный ли год
    static int daysInMonth(int year, int month);  // сколько дней в месяце
    int compare(const DateTime& other) const;     // -1, 0 или 1

public:
    // конструкторы
    DateTime();
    DateTime(int y, int m, int d, int h = 0, int mi = 0, int s = 0);
    DateTime(const string& str);

    // проверка корректности
    static bool checkDate(int y, int m, int d);
    static bool checkTime(int h, int m, int s);
    bool isValid() const;

    // ввод даты
    bool setFromString(const string& str);  // из строки в одном из 3 форматов
    void input();                           // ввод с клавиатуры

    // получение частей даты и времени
    void getDate(int& y, int& m, int& d) const;
    void getTime(int& h, int& m, int& s) const;

    // день недели (1 - понедельник, ..., 7 - воскресенье)
    int dayOfWeek() const;
    string dayOfWeekName() const;

    // дополнительное задание: дата пасхи в году этой даты
    DateTime easterCatholic() const;   // западная (григорианская)
    DateTime easterOrthodox() const;   // православная

    // вывод в нужном формате
    string toString() const;      // yyyy-MM-ddThh:mm:ss
    string dateToString() const;  // yyyy-MM-dd
    string timeToString() const;  // hh:mm:ss

    // перегрузка операторов
    double operator-(const DateTime& other) const;  // разница в днях
    DateTime operator+(int d) const;                // добавить дни

    bool operator>(const DateTime& other) const;
    bool operator>=(const DateTime& other) const;
    bool operator<(const DateTime& other) const;
    bool operator<=(const DateTime& other) const;
    bool operator==(const DateTime& other) const;
    bool operator!=(const DateTime& other) const;

    friend ostream& operator<<(ostream& out, const DateTime& dt);
    friend istream& operator>>(istream& in, DateTime& dt);
};

#endif
