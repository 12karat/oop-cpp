#pragma once

// размеры поля
const int FIELD_WIDTH = 7;
const int FIELD_HEIGHT = 6;

// перечисление для состояния ячеек
enum Cell {
    EMPTY,
    RED,
    YELLOW
};

// игровое поле
class Field {
private:
    // двумерный массив для хранения игрового поля
    Cell cells[FIELD_WIDTH][FIELD_HEIGHT];
    // очередь хода
    bool isRedTurn;
    // кто на данный момент выиграл
    Cell winner;

    // метод для проверки победителя после каждого хода
    void checkWinner();

public:
    // конструктор
    Field(bool isRedFirst = true);

    // очистка поля для новой игры
    void clear(bool isRedFirst = true);

    // сделать ход в указанную колонку (1-7)
    bool makeTurn(int column);

    // проверка, завершена ли игра (победа или ничья)
    bool isOver() const;

    // возвращает true, если сейчас ход красного игрока
    bool isRedTurnNow() const;

    // получить состояние конкретной ячейки
    Cell getCell(int i, int j) const;

    // отображение игрового поля в консоли
    void print() const;

    // печать итогового результата игры
    void printResult() const;
};