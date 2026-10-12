#define _CRT_SECURE_NO_WARNINGS
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#ifdef _WIN32
#include <windows.h>
#endif
#include "DateTime.h"

// Одна строка файла эфемерид
struct Sample {
    double t;   // абсолютное время, секунды от 01.01.1970 00:00 (по шкале файла)
    double el;  // угол места, град
};

static const double DAY = 86400.0;

// Быстрое чтение файла эфемерид.
// Формат строки: YMD HMS T R El Az FI LG (разделители - пробелы/табуляции).
// Время берём из YMD+HMS (столбец T округлён до 0.001 ч - для точности не годится).
static bool loadEphemeris(const std::string& path, std::vector<Sample>& out)
{
    FILE* f = std::fopen(path.c_str(), "rb");
    if (!f) return false;

    std::fseek(f, 0, SEEK_END);
    long size = std::ftell(f);
    std::fseek(f, 0, SEEK_SET);
    if (size <= 0) { std::fclose(f); return false; }

    std::string data(static_cast<size_t>(size), '\0');
    size_t got = std::fread(&data[0], 1, data.size(), f);
    std::fclose(f);
    data.resize(got);
    for (size_t i = 0; i < data.size(); ++i)
        if (data[i] == ',') data[i] = '.';          // десятичная запятая -> точка

    out.reserve(got / 60 + 16);
    const char* p = data.c_str();
    const char* end = p + data.size();

    // 1-я строка - заголовок
    while (p < end && *p != '\n') ++p;
    if (p < end) ++p;

    long long lastDayKey = -1, lastDayNum = 0;
    while (p < end) {
        const char* lineEnd = p;
        while (lineEnd < end && *lineEnd != '\n') ++lineEnd;

        char* q = nullptr;
        long ymd = std::strtol(p, &q, 10);          // YMD
        if (q != p) {
            const char* r = q;
            long hms = std::strtol(r, &q, 10);      // HMS
            if (q != r) {
                r = q;
                std::strtod(r, &q);                 // T (не используем)
                if (q != r) {
                    r = q;
                    std::strtod(r, &q);             // R
                    if (q != r) {
                        r = q;
                        double el = std::strtod(r, &q); // El
                        if (q != r) {
                            if (ymd != lastDayKey) {
                                lastDayKey = ymd;
                                lastDayNum = DateTime::dayNumber(
                                    static_cast<int>(ymd / 10000),
                                    static_cast<int>(ymd / 100 % 100),
                                    static_cast<int>(ymd % 100));
                            }
                            double sec = lastDayNum * DAY
                                       + (hms / 10000) * 3600.0
                                       + (hms / 100 % 100) * 60.0
                                       + (hms % 100);
                            out.push_back({ sec, el });
                        }
                    }
                }
            }
        }
        p = (lineEnd < end) ? lineEnd + 1 : end;
    }
    return !out.empty();
}

// Интерполяция Лагранжа по k точкам, начиная с индекса a (шаг в файле - 10 мин,
// поэтому линейной интерполяции для секундной точности недостаточно).
static double lagrange(const std::vector<Sample>& d, size_t a, int k, double x)
{
    const double x0 = d[a].t;
    double res = 0.0;
    for (int i = 0; i < k; ++i) {
        double term = d[a + i].el;
        double xi = d[a + i].t - x0;
        for (int j = 0; j < k; ++j) {
            if (j == i) continue;
            double xj = d[a + j].t - x0;
            term *= (x - x0 - xj) / (xi - xj);
        }
        res += term;
    }
    return res;
}

// Начало окна из k точек вокруг индекса c (с учётом границ массива)
static size_t windowStart(size_t n, size_t c, int k)
{
    long a = static_cast<long>(c) - k / 2;
    if (a < 0) a = 0;
    if (a + k > static_cast<long>(n)) a = static_cast<long>(n) - k;
    if (a < 0) a = 0;
    return static_cast<size_t>(a);
}

// Корень El(t)=0 на отрезке между строками i и i+1 (в них знак меняется)
static double findZero(const std::vector<Sample>& d, size_t i)
{
    const int k = 6;
    int kk = static_cast<int>(std::min<size_t>(k, d.size()));
    size_t a = windowStart(d.size(), i, kk);
    double lo = d[i].t, hi = d[i + 1].t;
    double flo = d[i].el;
    for (int it = 0; it < 60; ++it) {
        double mid = 0.5 * (lo + hi);
        double fm = lagrange(d, a, kk, mid);
        if ((fm < 0) == (flo < 0)) { lo = mid; flo = fm; }
        else hi = mid;
    }
    return 0.5 * (lo + hi);
}

// Максимум El(t) на отрезке [d[i-1].t, d[i+1].t] (тернарный поиск)
static double findMax(const std::vector<Sample>& d, size_t i)
{
    const int k = 7;
    int kk = static_cast<int>(std::min<size_t>(k, d.size()));
    size_t a = windowStart(d.size(), i, kk);
    double lo = d[i - 1].t, hi = d[i + 1].t;
    for (int it = 0; it < 80; ++it) {
        double m1 = lo + (hi - lo) / 3.0;
        double m2 = hi - (hi - lo) / 3.0;
        if (lagrange(d, a, kk, m1) < lagrange(d, a, kk, m2)) lo = m1;
        else hi = m2;
    }
    return 0.5 * (lo + hi);
}

// Открыть файл эфемерид: сначала годовой moonгггг.dat, затем посуточный ггггммдд.dat
static bool openEphemeris(const std::string& dir, const DateTime& date,
                          std::vector<Sample>& d)
{
    return loadEphemeris(dir + date.toYearFileName(), d) ||
           loadEphemeris(dir + date.toFileName(), d);
}

int main(int argc, char* argv[])
{
#ifdef _WIN32
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
#endif

    // Необязательный аргумент - папка с файлами эфемерид
    std::string dir;
    if (argc > 1) {
        dir = argv[1];
        if (!dir.empty() && dir.back() != '/' && dir.back() != '\\') dir += '/';
    }

    std::cout << "Введите дату (дд.мм.гггг): ";
    std::string line;
    std::getline(std::cin, line);

    DateTime date;
    if (!DateTime::tryParse(line, date)) {
        std::cout << "Неверный формат даты. Ожидается дд.мм.гггг\n";
        return 1;
    }

    std::vector<Sample> d;
    if (!openEphemeris(dir + (dir.empty() ? "" : ""), date, d) &&
        !openEphemeris(dir + "Moon/", date, d)) {
        std::cout << "Не найден файл эфемерид " << date.toYearFileName()
                  << " (или " << date.toFileName() << ")\n";
        return 1;
    }

    const double S = date.dayNumber() * DAY;   // начало суток
    const double E = S + DAY;                  // начало следующих
    const size_t n = d.size();

    // Первая строка, относящаяся к дню (время >= S)
    size_t lo = static_cast<size_t>(std::lower_bound(d.begin(), d.end(), S,
                   [](const Sample& s, double v) { return s.t < v; }) - d.begin());
    if (lo >= n || d[lo].t >= E) {
        std::cout << "В файле нет данных за " << date.toDisplayString() << "\n";
        return 1;
    }

    bool hasRise = false, hasSet = false, hasCulm = false;
    double tRise = 0, tSet = 0, tCulm = 0, bestEl = -1e9;

    for (size_t i = (lo > 0 ? lo - 1 : 0); i + 1 < n && d[i].t < E; ++i) {
        double e0 = d[i].el, e1 = d[i + 1].el;

        // Восход: угол места меняется с отрицательного на положительный
        // Заход:  угол места меняется с положительного на отрицательный
        bool rise = (e0 < 0 && e1 >= 0);
        bool set  = (e0 > 0 && e1 <= 0);
        if (rise || set) {
            double t = findZero(d, i);
            if (t >= S && t < E) {
                if (rise && !hasRise) { hasRise = true; tRise = t; }
                if (set  && !hasSet)  { hasSet  = true; tSet  = t; }
            }
        }

        // Кульминация: локальный максимум угла места внутри суток
        size_t c = i + 1;
        if (c >= 1 && c + 1 < n && d[c].el >= d[c - 1].el && d[c].el > d[c + 1].el) {
            double t = findMax(d, c);
            if (t >= S && t < E) {
                const int kk = static_cast<int>(std::min<size_t>(7, n));
                double v = lagrange(d, windowStart(n, c, kk), kk, t);
                if (v > bestEl) { bestEl = v; hasCulm = true; tCulm = t; }
            }
        }
    }

    std::cout << "Дата: " << date.toDisplayString() << "\n";
    std::cout << "Восход Луны: "      << (hasRise ? DateTime::secondsToHMS(tRise - S) : "нет") << "\n";
    std::cout << "Кульминация Луны: " << (hasCulm ? DateTime::secondsToHMS(tCulm - S) : "нет") << "\n";
    std::cout << "Заход Луны: "       << (hasSet  ? DateTime::secondsToHMS(tSet  - S) : "нет") << "\n";
    return 0;
}
