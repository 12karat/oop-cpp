#include <iostream>
#include <windows.h>
#include "DateTime.h"
using namespace std;

// Быстрая сортировка (по возрастанию)
void quickSort(DateTime arr[], int left, int right)
{
    int i = left;
    int j = right;
    DateTime pivot = arr[(left + right) / 2];   // опорный элемент

    while (i <= j)
    {
        while (arr[i] < pivot) i++;
        while (arr[j] > pivot) j--;

        if (i <= j)
        {
            DateTime tmp = arr[i];
            arr[i] = arr[j];
            arr[j] = tmp;
            i++;
            j--;
        }
    }

    if (left < j) quickSort(arr, left, j);
    if (i < right) quickSort(arr, i, right);
}

int main()
{
    // чтобы русский текст нормально отображался в консоли
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    // ---------- 1. Ввод даты ----------
    cout << "1. Ввод даты" << endl;
    cout << "Введите дату (yyyy-MM-dd, или yyyy-MM-ddThh:mm:ss, или hh:mm:ss): ";
    DateTime a;
    cin >> a;
    if (a.isValid())
        cout << "Вы ввели: " << a << endl;
    else
        cout << "Дата введена некорректно, будет использована 2021-12-21T07:54:34" << endl;
    if (!a.isValid())
        a = DateTime("2021-12-21T07:54:34");

    // ---------- 2. Проверка корректности ----------
    cout << endl << "2. Проверка корректности даты" << endl;
    string tests[5] = { "2021-12-21", "2021-02-29", "2020-02-29", "2021-13-05", "2021-12-21T25:00:00" };
    for (int i = 0; i < 5; i++)
    {
        DateTime t(tests[i]);
        cout << tests[i] << " - ";
        if (t.isValid()) cout << "корректна" << endl;
        else cout << "некорректна" << endl;
    }

    // ---------- 3. Форматы вывода ----------
    cout << endl << "3. Форматы вывода" << endl;
    DateTime b("2021-12-21T07:54:34");
    cout << "yyyy-MM-ddThh:mm:ss : " << b.toString() << endl;
    cout << "yyyy-MM-dd          : " << b.dateToString() << endl;
    cout << "hh:mm:ss            : " << b.timeToString() << endl;

    // ---------- 4. Разница между датами ----------
    cout << endl << "4. Разница между датами (оператор -)" << endl;
    DateTime d1("2022-03-01");
    DateTime d2("2021-12-21");
    cout << d1 << " - " << d2 << " = " << d1 - d2 << " дн." << endl;
    cout << d2 << " - " << d1 << " = " << d2 - d1 << " дн." << endl;
    DateTime d3("2021-12-21T12:00:00");
    cout << d3 << " - " << d2 << " = " << d3 - d2 << " дн." << endl;

    // ---------- 5. Добавление дней ----------
    cout << endl << "5. Добавление дней (оператор +)" << endl;
    cout << d2 << " + 10 = " << d2 + 10 << endl;
    cout << d2 << " + 365 = " << d2 + 365 << endl;
    DateTime leap("2020-02-28");
    cout << leap << " + 1 = " << leap + 1 << endl;
    cout << leap << " + 2 = " << leap + 2 << endl;

    // ---------- 6. Сравнение дат ----------
    cout << endl << "6. Сравнение дат" << endl;
    DateTime x("2021-12-21");
    DateTime y("2022-01-15");
    cout << "x = " << x << ", y = " << y << endl;
    cout << "x >  y : " << (x > y) << endl;
    cout << "x >= y : " << (x >= y) << endl;
    cout << "x <  y : " << (x < y) << endl;
    cout << "x <= y : " << (x <= y) << endl;
    cout << "x == y : " << (x == y) << endl;
    cout << "x == x : " << (x == x) << endl;

    // ---------- 7. День недели ----------
    cout << endl << "7. День недели" << endl;
    string wd[3] = { "2021-12-21", "2000-01-01", "1995-01-01" };
    for (int i = 0; i < 3; i++)
    {
        DateTime t(wd[i]);
        cout << t.dateToString() << " - " << t.dayOfWeekName()
             << " (" << t.dayOfWeek() << "-й день недели)" << endl;
    }

    // ---------- 8. Быстрая сортировка (Приложение №1) ----------
    cout << endl << "8. Быстрая сортировка дат" << endl;
    const int N = 26;
    string data[N] = {
        "1995-01-01", "1994-04-26", "1992-12-05", "1994-07-12", "1993-09-11",
        "1992-07-21", "1997-06-29", "1997-02-06", "1995-10-31", "1993-08-18",
        "1995-03-02", "1996-08-12", "1997-01-28", "1995-12-15", "1997-06-09",
        "1993-11-10", "1994-07-05", "1993-06-06", "1997-08-21", "1996-09-28",
        "1993-01-14", "1992-10-02", "1997-06-13", "1996-08-18", "1994-06-08",
        "1993-05-04"
    };

    DateTime arr[N];
    for (int i = 0; i < N; i++)
        arr[i] = DateTime(data[i]);

    cout << "До сортировки:" << endl;
    for (int i = 0; i < N; i++)
        cout << arr[i].dateToString() << endl;

    quickSort(arr, 0, N - 1);

    cout << "После сортировки:" << endl;
    for (int i = 0; i < N; i++)
        cout << arr[i].dateToString() << endl;

    // ---------- 9. Дополнительное задание: пасха ----------
    cout << endl << "9. Дата пасхи" << endl;
    int years[4] = { 2021, 2022, 2023, 2024 };
    for (int i = 0; i < 4; i++)
    {
        DateTime t(years[i], 1, 1);
        cout << years[i] << ": католическая - " << t.easterCatholic().dateToString()
             << ", православная - " << t.easterOrthodox().dateToString() << endl;
    }

    return 0;
}
