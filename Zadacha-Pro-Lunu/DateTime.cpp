#define _CRT_SECURE_NO_WARNINGS
#include "DateTime.h"
#include <cstdio>
#include <cmath>
#include <cctype>

DateTime::DateTime() : m_year(1970), m_month(1), m_day(1) {}

DateTime::DateTime(int year, int month, int day)
    : m_year(year), m_month(month), m_day(day) {}

bool DateTime::isLeapYear(int year)
{
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int DateTime::daysInMonth(int year, int month)
{
    static const int days[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if (month < 1 || month > 12) return 0;
    if (month == 2 && isLeapYear(year)) return 29;
    return days[month - 1];
}

bool DateTime::isValid() const
{
    if (m_year < 1 || m_year > 9999) return false;
    if (m_month < 1 || m_month > 12) return false;
    return m_day >= 1 && m_day <= daysInMonth(m_year, m_month);
}

bool DateTime::tryParse(const std::string& text, DateTime& result)
{
    // Ожидается строго дд.мм.гггг (допускаются пробелы по краям)
    size_t b = 0, e = text.size();
    while (b < e && std::isspace(static_cast<unsigned char>(text[b]))) ++b;
    while (e > b && std::isspace(static_cast<unsigned char>(text[e - 1]))) --e;
    std::string s = text.substr(b, e - b);

    if (s.size() != 10 || s[2] != '.' || s[5] != '.') return false;
    for (size_t i = 0; i < s.size(); ++i) {
        if (i == 2 || i == 5) continue;
        if (!std::isdigit(static_cast<unsigned char>(s[i]))) return false;
    }

    int d = (s[0] - '0') * 10 + (s[1] - '0');
    int m = (s[3] - '0') * 10 + (s[4] - '0');
    int y = (s[6] - '0') * 1000 + (s[7] - '0') * 100 + (s[8] - '0') * 10 + (s[9] - '0');

    DateTime dt(y, m, d);
    if (!dt.isValid()) return false;
    result = dt;
    return true;
}

std::string DateTime::toDisplayString() const
{
    char buf[16];
    std::snprintf(buf, sizeof(buf), "%02d.%02d.%04d", m_day, m_month, m_year);
    return buf;
}

std::string DateTime::toFileName() const
{
    char buf[24];
    std::snprintf(buf, sizeof(buf), "%04d%02d%02d.dat", m_year, m_month, m_day);
    return buf;
}

std::string DateTime::toYearFileName() const
{
    char buf[24];
    std::snprintf(buf, sizeof(buf), "moon%04d.dat", m_year);
    return buf;
}

long long DateTime::dayNumber(int year, int month, int day)
{
    // days_from_civil (Howard Hinnant)
    year -= month <= 2;
    long long era = (year >= 0 ? year : year - 399) / 400;
    long long yoe = year - era * 400;
    long long doy = (153LL * (month + (month > 2 ? -3 : 9)) + 2) / 5 + day - 1;
    long long doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return era * 146097 + doe - 719468;
}

long long DateTime::dayNumber() const
{
    return dayNumber(m_year, m_month, m_day);
}

std::string DateTime::hoursToHMS(double hours)
{
    return secondsToHMS(hours * 3600.0);
}

std::string DateTime::secondsToHMS(double seconds)
{
    long long total = std::llround(seconds);
    total %= 86400;
    if (total < 0) total += 86400;
    int h = static_cast<int>(total / 3600);
    int m = static_cast<int>((total % 3600) / 60);
    int s = static_cast<int>(total % 60);
    char buf[16];
    std::snprintf(buf, sizeof(buf), "%02d:%02d:%02d", h, m, s);
    return buf;
}
