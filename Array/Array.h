#pragma once
#include <iostream>

using namespace std;

// вместимость по умолчанию
const int DEFAULT_CAPACITY = 10;

// исключение для обработки ошибок индексации 
class ArrayException {};

class Array {
private:
    // указатель на массив в динамич. памяти
    int* ptr;
    // текущий размер
    int size;
    // вместимость
    int capacity;

    // функция для увеличения емкости 
    void increaseCapacity(int newCapacity);

public:
    // конструктор с аргументом по умолчанию
    explicit Array(int startCapacity = DEFAULT_CAPACITY);

    // деструктор 
    ~Array();

    // конструктор копирования
    Array(const Array& arr);

    // оператор присваивания 
    Array& operator =(const Array& arr);

    // оператор индексации 
    int& operator [](int index);

    // вставка элемента в произвольное место
    void insert(int elem, int index);
    // вставка элемента в конец массива
    void insert(int elem);

    // функция удаления элемента
    void remove(int index);

    // другие функции
    int getSize() const;
    friend ostream& operator <<(ostream& out, const Array& arr);
};
