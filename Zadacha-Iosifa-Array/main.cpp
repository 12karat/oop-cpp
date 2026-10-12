#include <iostream>
#include <ctime>
#include "Array.h"

// функция решения задачи
int solveJosephus(int n, int k) {
    Array circle(n); // создание массива (круга) размером n

    // заполняем круг людьми, нумеруя их от 1 до n
    for (int i = 1; i <= n; i++) {
        circle.insert(i);
    }

    // переменная для отслеживания текущей позиции в кругу
    int currentIndex = 0;
    // выполняем цикл до тех пор, пока в кругу не останется один человек
    while (circle.getSize() > 1) {
        // формула (index + k - 1) % size позволяет имитировать движение по кругу
        currentIndex = (currentIndex + k - 1) % circle.getSize();
        circle.remove(currentIndex); // удаляем человека из массива по вычисленному индексу
    }

    return circle[0]; // возвращаем номер последнего оставшегося человека
}

int main() {
    setlocale(LC_ALL, "Russian");
    // массив значений
    int N_values[] = { 1000, 5000, 10000, 50000, 100000, 500000, 1000000 };
    int k = 2; // шаг исключения

    cout << "N\t\tОтвет\t\tВремя (Сек)" << endl;
    cout << "--------------------------------------------" << endl;

    // перебор всех заданных значений N из массива
    for (int n : N_values) {
        clock_t start = clock();
        int result = solveJosephus(n, k);
        clock_t end = clock();

        // вычисляем затраченное время в секундах
        double duration = static_cast<double>(end - start) / CLOCKS_PER_SEC;
        cout << n << "\t\t" << result << "\t\t" << duration << endl;
    }

    return 0;
}
