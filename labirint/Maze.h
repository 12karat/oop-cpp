#pragma once
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

// BMP-изображение (внутри хранится как RGB, сверху вниз).
// Читает 1/4/8-битные (с палитрой), 24- и 32-битные несжатые BMP.
// Сохраняет всегда как 24-битный BMP.
class Bitmap
{
public:
    Bitmap() : m_w(0), m_h(0) {}

    bool load(const std::string& path, std::string& error);
    bool save(const std::string& path, std::string& error) const;

    int width() const { return m_w; }
    int height() const { return m_h; }

    void getPixel(int x, int y, uint8_t& r, uint8_t& g, uint8_t& b) const;
    void setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b);

private:
    int m_w;
    int m_h;
    std::vector<uint8_t> m_rgb;   // m_w * m_h * 3
};

// Поиск выхода из лабиринта волновым алгоритмом Ли.
// Зелёный кружок - вход, красный - выход, чёрные пиксели - стены.
class MazeSolver
{
public:
    explicit MazeSolver(Bitmap& bitmap);

    // Находит кратчайший путь и рисует его на изображении (синий цвет).
    bool solve(std::string& error);

    // Длина найденного пути в пикселях
    size_t pathLength() const { return m_pathLength; }

private:
    enum Cell : uint8_t { Free = 0, Wall = 1, Green = 2, Red = 3 };

    void classify();
    void markOutside(int sealRadius);
    void computeClearance();
    bool lee(int clearance, std::vector<int>& path);
    void drawPath(const std::vector<int>& path);

    Bitmap& m_bmp;
    int m_w, m_h;
    std::vector<uint8_t>  m_cell;       // тип клетки (Cell)
    std::vector<uint8_t>  m_outside;    // 1 - фон снаружи лабиринта
    std::vector<uint16_t> m_clear;      // расстояние до ближайшей стены
    std::vector<int>      m_wave;       // номер волны (-1 - не посещён)
    std::vector<int>      m_queue;
    size_t m_pathLength;
};
