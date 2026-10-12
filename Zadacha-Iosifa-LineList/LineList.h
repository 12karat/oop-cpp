#pragma once
#include <iostream>

using namespace std;

// исключение для обработки ошибок
class LineListException {};

// элемент списка
template <class T> class LineListElem
{
private:
    T data; // данные типа T
    LineListElem* next; // указатель на следующий элемент
public:
    // конструктор элемента
    LineListElem(const T& adata,
        LineListElem* anext);
    // получение ссылки на хранящиеся данные
    const T& getData() const;
    // указатель на следующий элемент
    LineListElem* getNext();

    template <class T> friend class LineList;
};

// список
template <class T> class LineList {
private:
    // указатель на первый элемент
    LineListElem<T>* start;
    // закрытый конструктор копирования
    LineList(const LineList& list);

public:
    // конструктор
    LineList();
    // деструктор
    ~LineList();
    // получение элементов
    LineListElem<T>* getStart();
    // удаление первого элемента
    void deleteFirst();
    // удаление после данного элемента
    void deleteAfter(LineListElem<T>* ptr);
    // вставка первого элемента
    void insertFirst(const T& data);
    // вставка после данного элемента
    void insertAfter(LineListElem<T>* ptr, const T& data);
    // функция вывода
    template <class T> friend ostream& operator<<(ostream& out, const LineList<T>& list);
};


// определения функций класса

// конструктор элемента
template <class T> LineListElem<T>::LineListElem(
    const T& adata, LineListElem<T>* anext)
{
    data = adata;
    next = anext;
}

// получение ссылки на хранящиеся данные
template <class T>
const T& LineListElem<T>::getData() const
{
    return data;
}

// указатель на следующий элемент
template <class T>
LineListElem<T>* LineListElem<T>::getNext()
{
    return next;
}

// конструктор
template <class T> LineList<T>::LineList()
{
    start = 0;
}

// деструктор
template <class T> LineList<T>::~LineList()
{
    while (start)
        deleteFirst();
}

// получение элементов
template <class T>
LineListElem<T>* LineList<T>::getStart()
{
    return start;
}

// удаление первого элемента
template <class T> void LineList<T>::deleteFirst()
{
    if (start)
    {
        LineListElem<T>* temp = start->next;
        delete start;
        start = temp;
    }
    else throw LineListException();
}

// удаление после данного элемента
template <class T> void LineList<T>::deleteAfter(
    LineListElem<T>* ptr) {
    if (ptr && ptr->next) {
        LineListElem<T>* temp = ptr->next;
        ptr->next = ptr->next->next;
        delete temp;
    }
    else throw LineListException();
}

// вставка первого элемента
template <class T> void LineList<T>::insertFirst(
    const T& data)
{
    LineListElem<T>* second = start;
    start = new LineListElem<T>(data, second);
}

// вставка после данного элемента
template <class T> void LineList<T>::insertAfter(
    LineListElem<T>* ptr, const T& data) {
    if (ptr) {
        LineListElem<T>* temp = ptr->next;
        ptr->next = new LineListElem<T>(data, temp);
    }
}

// функция вывода
template <class T> ostream& operator <<
(ostream& out, const LineList<T>& list)
{
    LineListElem<T>* ptr = list.start;
    if (!ptr)
        out << "EMPTY ";
    else while (ptr)
    {
        out << ptr->getData() << ' ';
        ptr = ptr->getNext();
    }
    return out;
}