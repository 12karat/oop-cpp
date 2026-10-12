#include <iostream>
#include "Array.h"

using namespace std;

int main(void)
{
	// тестируем
	setlocale(LC_ALL, "Russian");

	// создаем и заполняем
	Array arr(4);
	for (int i = 0; i < 4; i++)
		arr.insert(i + 1);
	cout << arr << endl;
	cout << " " << endl;

	// вставка новых элементов по конкретным индексам
	for (int i = 0; i < 8; i += 2)
		arr.insert(10 + i, i);
	cout << arr << endl;
	cout << " " << endl;

	// изменение элементов через оператор []
	for (int i = 1; i < 8; i += 2)
		arr[i] = 20 + i;
	cout << arr << endl;
	cout << " " << endl;

	// удаляем элементы из массива
	for (int i = 6; i >= 0; i -= 3)
		arr.remove(i);
	cout << arr << endl;
	cout << " " << endl;
}
