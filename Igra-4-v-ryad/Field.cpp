#include <iostream>
#include "Field.h"

using namespace std;

// конструктор
Field::Field(bool isRedFirst) {
    clear(isRedFirst); // очистка поля для новой игры
}

// очистка поля для новой игры
void Field::clear(bool isRedFirst) {
    isRedTurn = isRedFirst;
    winner = EMPTY;
    // заполняем все ячейки двумерного массива пустыми значениями
    for (int i = 0; i < FIELD_WIDTH; i++) {
        for (int j = 0; j < FIELD_HEIGHT; j++) {
            cells[i][j] = EMPTY;
        }
    }
}

// сделать ход в указанную колонку (1-7)
bool Field::makeTurn(int column) {
    // валидация хода
    if (winner != EMPTY || column < 1 || column > FIELD_WIDTH) {
        return false;
    }

    int i = column - 1;
    // поиск самой нижней свободной ячейки в колонке
    for (int j = 0; j < FIELD_HEIGHT; j++) {
        if (cells[i][j] == EMPTY) {
            cells[i][j] = isRedTurn ? RED : YELLOW;
            checkWinner();
            isRedTurn = !isRedTurn;
            return true;
        }
    }
    return false;
}

// метод для проверки победителя после каждого хода
void Field::checkWinner() {
    const int DIR_NUMBER = 4; // количество направлений для проверки
    const int di[] = { 1, 0, 1, 1 }; // смещение по горизонтали для направлений
    const int dj[] = { 0, 1, -1, 1 }; // смещение по вертикали для направлений
    const int WIN_LENGTH = 4; // сколько фишек нужно собрать для победы

    // перебираем каждую ячейку поля в качестве потенциального начала линии
    for (int i = 0; i < FIELD_WIDTH; i++) {
        for (int j = 0; j < FIELD_HEIGHT; j++) {
            Cell start = cells[i][j];
            if (start == EMPTY) continue; // пустая ячейка не может быть началом победной линии

            // проверяем 4 направления: вправо, вверх, вправо-вниз, вправо-вверх
            for (int dir = 0; dir < DIR_NUMBER; dir++) {
                int length = 0;
                int iline = i, jline = j;

                // считаем количество одинаковых фишек подряд в данном направлении
                while (++length < WIN_LENGTH) {
                    iline += di[dir];
                    jline += dj[dir];

                    // проверка выхода за границы поля
                    if (iline < 0 || iline >= FIELD_WIDTH ||
                        jline < 0 || jline >= FIELD_HEIGHT) {
                        break;
                    }
                    // если встретили фишку другого цвета или пустоту — линия прервана
                    if (cells[iline][jline] != start) {
                        break;
                    }
                }

                // если нашли 4 фишки подряд — фиксируем победителя
                if (length == WIN_LENGTH) {
                    winner = start;
                    return;
                }
            }
        }
    }
}

// проверка, завершена ли игра (победа или ничья)
bool Field::isOver() const {
    if (winner != EMPTY) {
        return true;
    }
    // если находим хотя бы одну пустую ячейку — игра продолжается
    for (int i = 0; i < FIELD_WIDTH; i++) {
        for (int j = 0; j < FIELD_HEIGHT; j++) {
            if (cells[i][j] == EMPTY) {
                return false;
            }
        }
    }
    return true;
}

// возвращает true, если сейчас ход красного игрока
bool Field::isRedTurnNow() const {
    return isRedTurn;
}

// получить состояние конкретной ячейки
Cell Field::getCell(int i, int j) const {
    return cells[i][j];
}

// отображение игрового поля в консоли
void Field::print() const {
    cout << "\n  1   2   3   4   5   6   7\n";
    cout << "-----------------------------\n";
    // выводим поле «сверху вниз»
    for (int j = FIELD_HEIGHT - 1; j >= 0; j--) {
        for (int i = 0; i < FIELD_WIDTH; i++) {
            cout << "| ";
            if (cells[i][j] == RED) cout << "R ";
            else if (cells[i][j] == YELLOW) cout << "Y ";
            else cout << ". ";
        }
        cout << "|\n";
    }
    cout << "-----------------------------\n";
}

// печать итогового результата игры
void Field::printResult() const {
    print();
    if (winner == RED) {
        cout << "Игра окончена! Победил красный игрок (R)!\n";
    }
    else if (winner == YELLOW) {
        cout << "Игра окончена! Победил желтый игрок (Y)!\n";
    }
    else {
        cout << "Игра окончена! Ничья!\n";
    }
}