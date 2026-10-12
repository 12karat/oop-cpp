#pragma once
#include <string>

// Класс для работы с датой (дд.мм.гггг) и временем (hh:mm:ss)
class DateTime
{
public:
    DateTime();
    DateTime(int year, int month, int day);

    // Разбор строки формата дд.мм.гггг. Возвращает false при неверном формате/дате.
    static bool tryParse(const std::string& text, DateTime& result);

    // Проверка корректности даты (с учётом високосных годов)
    bool isValid() const;

    static bool isLeapYear(int year);
    static int daysInMonth(int year, int month);

    int year() const  { return m_year; }
    int month() const { return m_month; }
    int day() const   { return m_day; }

    // "дд.мм.гггг"
    std::string toDisplayString() const;
    // "ггггммдд.dat" (посуточный файл, как в задании)
    std::string toFileName() const;
    // "moonгггг.dat" (годовой файл из архива Moon)
    std::string toYearFileName() const;

    // Номер дня от 01.01.1970 (для сравнения дат из разных лет)
    long long dayNumber() const;
    static long long dayNumber(int year, int month, int day);

    // Время в долях часа -> "hh:mm:ss"
    static std::string hoursToHMS(double hours);
    // Секунды от начала суток -> "hh:mm:ss"
    static std::string secondsToHMS(double seconds);

private:
    int m_year;
    int m_month;
    int m_day;
};
