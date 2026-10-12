#include <iostream>
#include <ctime>
#include "LineList.h"

using namespace std;

// функция решения задачи
int solveJosephus(int N, int k) {
    LineList<int> circle; // создание объекта линейного списка

    // заполняем список числами от 1 до N
    for (int i = N; i >= 1; --i) {
        circle.insertFirst(i);
    }

    LineListElem<int>* prev = nullptr; // указатель на предыдущий элемент
    LineListElem<int>* curr = circle.getStart(); // указатель на текущий элемент

    // цикл удаления, пока не останется один элемент
    for (int count = 0; count < N - 1; ++count) {
        // отсчитываем k шагов
        for (int i = 1; i < k; ++i) {
            prev = curr;
            curr = curr->getNext();
            // если дошли до конца — возвращаемся в начало
            if (curr == nullptr) {
                prev = nullptr;
                curr = circle.getStart();
            }
        }

        // удаление текущего элемента (curr)
        if (prev == nullptr) {
            // если prev == nullptr, значит curr — это первый элемент
            circle.deleteFirst();
            curr = circle.getStart();
        }
        else {
            // иначе удаляем элемент после prev (то есть сам curr)
            circle.deleteAfter(prev);
            curr = prev->getNext();
            // если удалили последний элемент, переходим в начало
            if (curr == nullptr) curr = circle.getStart();
        }
    }

    return circle.getStart()->getData(); // возвращаем последний элемент
}

int main() {
    setlocale(LC_ALL, ".UTF8");

    // ---------- 1. Ввод пользователя ----------
    int N, k;
    cout << "Введите N (количество элементов): ";
    cin >> N;
    cout << "Введите k (шаг выбывания): ";
    cin >> k;

    if (!cin || N < 1 || k < 1) {
        cout << "Ошибка: N и k должны быть целыми числами больше 0" << endl;
        return 1;
    }

    clock_t start = clock();
    int result = solveJosephus(N, k);
    clock_t end = clock();
    double duration = (double)(end - start) / CLOCKS_PER_SEC;

    cout << "Остался элемент: " << result << endl;
    cout << "Время расчета: " << duration << " сек" << endl;

    // ---------- 2. Контрольная проверка (таблица 1, k = 2) ----------
    cout << endl << "Контрольная проверка (k = 2)" << endl;
    int N_values[] = { 1000, 5000, 10000, 50000, 100000, 500000, 1000000 };

    cout << "N\tОтвет\tВремя (сек)" << endl;
    for (int n : N_values) {
        clock_t s = clock();
        int res = solveJosephus(n, 2);
        clock_t e = clock();

        double d = (double)(e - s) / CLOCKS_PER_SEC;
        cout << n << "\t" << res << "\t" << d << endl;
    }

    return 0;
}
