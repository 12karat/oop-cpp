#include <iostream>
#include "LineList.h"

using namespace std;

int main(void) {
    // тестируем
    setlocale(LC_ALL, ".UTF8");

    // использование класса
    LineList<int> list;

    cout << "Начало: " << list << endl;

    // вставка элементов
    list.insertFirst(10);
    LineListElem<int>* ptr = list.getStart();

    list.insertAfter(ptr, 15);
    list.insertAfter(ptr->getNext(), 12);
    list.insertFirst(7);

    cout << "Шаг 1: " << list << endl;

    // нахождение суммы
    int sum = 0;
    // используем переменную-итератор
    for (LineListElem<int>* it = list.getStart(); it; it = it->getNext()) {
        sum += it->getData();
    }

    cout << "Сумма = " << sum << endl;

    return 0;
}
