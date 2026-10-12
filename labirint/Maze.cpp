\xEF\xBB\xBF#define _CRT_SECURE_NO_WARNINGS
#include "Maze.h"
#include <algorithm>
#include <cstdio>
#include <cstdlib>

// ======================= Bitmap =======================

static uint32_t rd16(const std::vector<uint8_t>& b, size_t p) { return b[p] | (b[p + 1] << 8); }
static uint32_t rd32(const std::vector<uint8_t>& b, size_t p)
{
    return static_cast<uint32_t>(b[p]) | (static_cast<uint32_t>(b[p + 1]) << 8) |
           (static_cast<uint32_t>(b[p + 2]) << 16) | (static_cast<uint32_t>(b[p + 3]) << 24);
}

bool Bitmap::load(const std::string& path, std::string& error)
{
    FILE* f = std::fopen(path.c_str(), "rb");
    if (!f) { error = "Не удалось открыть файл: " + path; return false; }
    std::fseek(f, 0, SEEK_END);
    long size = std::ftell(f);
    std::fseek(f, 0, SEEK_SET);
    std::vector<uint8_t> b(size > 0 ? static_cast<size_t>(size) : 0);
    size_t got = b.empty() ? 0 : std::fread(&b[0], 1, b.size(), f);
    std::fclose(f);
    if (got != b.size() || b.size() < 54 || b[0] != 'B' || b[1] != 'M') {
        error = "Файл не является BMP";
        return false;
    }

    uint32_t dataOff  = rd32(b, 10);
    uint32_t dibSize  = rd32(b, 14);
    int32_t  w        = static_cast<int32_t>(rd32(b, 18));
    int32_t  h        = static_cast<int32_t>(rd32(b, 22));
    int      bpp      = static_cast<int>(rd16(b, 28));
    uint32_t compress = rd32(b, 30);
    uint32_t colors   = rd32(b, 46);

    bool topDown = h < 0;
    if (topDown) h = -h;
    if (w <= 0 || h <= 0 || dibSize < 40) { error = "Неподдерживаемый заголовок BMP"; return false; }
    if (compress != 0 && !(compress == 3 && bpp == 32)) {
        error = "Сжатые BMP не поддерживаются";
        return false;
    }
    if (bpp != 1 && bpp != 4 && bpp != 8 && bpp != 24 && bpp != 32) {
        error = "Неподдерживаемая глубина цвета BMP";
        return false;
    }

    // Палитра
    std::vector<uint8_t> pal;
    if (bpp <= 8) {
        size_t n = colors ? colors : (1u << bpp);
        size_t p = 14 + dibSize;
        if (p + n * 4 > b.size()) { error = "Повреждённая палитра BMP"; return false; }
        pal.assign(b.begin() + p, b.begin() + p + n * 4);
    }

    size_t stride = ((static_cast<size_t>(w) * bpp + 31) / 32) * 4;
    if (dataOff + stride * h > b.size()) { error = "Файл BMP обрезан"; return false; }

    m_w = w; m_h = h;
    m_rgb.assign(static_cast<size_t>(w) * h * 3, 0);

    for (int y = 0; y < h; ++y) {
        int srcRow = topDown ? y : (h - 1 - y);
        const uint8_t* row = &b[dataOff + stride * srcRow];
        uint8_t* dst = &m_rgb[static_cast<size_t>(y) * w * 3];
        for (int x = 0; x < w; ++x) {
            uint8_t r, g, bl;
            if (bpp == 24 || bpp == 32) {
                const uint8_t* px = row + x * (bpp / 8);
                bl = px[0]; g = px[1]; r = px[2];
            } else {
                unsigned idx;
                if (bpp == 8)      idx = row[x];
                else if (bpp == 4) idx = (x & 1) ? (row[x / 2] & 15) : (row[x / 2] >> 4);
                else               idx = (row[x / 8] >> (7 - (x & 7))) & 1;
                if (idx * 4 + 2 >= pal.size()) idx = 0;
                bl = pal[idx * 4]; g = pal[idx * 4 + 1]; r = pal[idx * 4 + 2];
            }
            dst[x * 3] = r; dst[x * 3 + 1] = g; dst[x * 3 + 2] = bl;
        }
    }
    return true;
}

bool Bitmap::save(const std::string& path, std::string& error) const
{
    size_t stride = ((static_cast<size_t>(m_w) * 24 + 31) / 32) * 4;
    size_t dataSize = stride * m_h;
    std::vector<uint8_t> out(54 + dataSize, 0);

    auto w16 = [&](size_t p, uint32_t v) { out[p] = v & 255; out[p + 1] = (v >> 8) & 255; };
    auto w32 = [&](size_t p, uint32_t v) { w16(p, v & 0xFFFF); w16(p + 2, v >> 16); };

    out[0] = 'B'; out[1] = 'M';
    w32(2, static_cast<uint32_t>(out.size()));
    w32(10, 54);
    w32(14, 40);
    w32(18, static_cast<uint32_t>(m_w));
    w32(22, static_cast<uint32_t>(m_h));
    w16(26, 1);
    w16(28, 24);
    w32(34, static_cast<uint32_t>(dataSize));
    w32(38, 2835);
    w32(42, 2835);

    for (int y = 0; y < m_h; ++y) {
        const uint8_t* src = &m_rgb[static_cast<size_t>(y) * m_w * 3];
        uint8_t* dst = &out[54 + stride * (m_h - 1 - y)];
        for (int x = 0; x < m_w; ++x) {
            dst[x * 3] = src[x * 3 + 2];
            dst[x * 3 + 1] = src[x * 3 + 1];
            dst[x * 3 + 2] = src[x * 3];
        }
    }

    FILE* f = std::fopen(path.c_str(), "wb");
    if (!f) { error = "Не удалось создать файл: " + path; return false; }
    size_t wr = std::fwrite(&out[0], 1, out.size(), f);
    std::fclose(f);
    if (wr != out.size()) { error = "Ошибка записи файла"; return false; }
    return true;
}

void Bitmap::getPixel(int x, int y, uint8_t& r, uint8_t& g, uint8_t& b) const
{
    const uint8_t* p = &m_rgb[(static_cast<size_t>(y) * m_w + x) * 3];
    r = p[0]; g = p[1]; b = p[2];
}

void Bitmap::setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
    uint8_t* p = &m_rgb[(static_cast<size_t>(y) * m_w + x) * 3];
    p[0] = r; p[1] = g; p[2] = b;
}

// ======================= MazeSolver =======================

MazeSolver::MazeSolver(Bitmap& bitmap)
    : m_bmp(bitmap), m_w(bitmap.width()), m_h(bitmap.height()), m_pathLength(0) {}

// Определяем тип каждого пикселя: стена / проход / зелёный (вход) / красный (выход)
void MazeSolver::classify()
{
    m_cell.assign(static_cast<size_t>(m_w) * m_h, Free);
    for (int y = 0; y < m_h; ++y) {
        for (int x = 0; x < m_w; ++x) {
            uint8_t r, g, b;
            m_bmp.getPixel(x, y, r, g, b);
            int mx = std::max(static_cast<int>(g), static_cast<int>(b));
            int mxg = std::max(static_cast<int>(r), static_cast<int>(b));
            uint8_t c;
            if (r - mx > 40)        c = Red;
            else if (g - mxg > 30)  c = Green;
            else {
                int lum = (r * 299 + g * 587 + b * 114) / 1000;
                c = (lum < 128) ? Wall : Free;
            }
            m_cell[static_cast<size_t>(y) * m_w + x] = c;
        }
    }

    // Края кружков (особенно в JPEG) бывают бледнее центра: присоединяем к кружку
    // соседние светлые пиксели в радиусе 2, чтобы вокруг него не оставалось "каймы".
    std::vector<uint8_t> src(m_cell);
    for (int y = 0; y < m_h; ++y)
        for (int x = 0; x < m_w; ++x) {
            uint8_t c = src[static_cast<size_t>(y) * m_w + x];
            if (c != Green && c != Red) continue;
            for (int dy = -2; dy <= 2; ++dy)
                for (int dx = -2; dx <= 2; ++dx) {
                    int ny = y + dy, nx = x + dx;
                    if (ny < 0 || ny >= m_h || nx < 0 || nx >= m_w) continue;
                    size_t i = static_cast<size_t>(ny) * m_w + nx;
                    if (src[i] == Free) m_cell[i] = c;
                }
        }
}

// Фон вокруг лабиринта тоже белый, и он соединён со входом/выходом снаружи.
// Заливаем его от границ картинки (кружки входа/выхода "затыкают" проёмы)
// и в дальнейшем считаем стеной.
void MazeSolver::markOutside(int sealRadius)
{
    const size_t n = static_cast<size_t>(m_w) * m_h;
    std::vector<uint8_t> block(n, 0);
    for (int y = 0; y < m_h; ++y)
        for (int x = 0; x < m_w; ++x) {
            uint8_t c = m_cell[static_cast<size_t>(y) * m_w + x];
            if (c != Green && c != Red) continue;
            for (int dy = -sealRadius; dy <= sealRadius; ++dy)
                for (int dx = -sealRadius; dx <= sealRadius; ++dx) {
                    int ny = y + dy, nx = x + dx;
                    if (ny >= 0 && ny < m_h && nx >= 0 && nx < m_w)
                        block[static_cast<size_t>(ny) * m_w + nx] = 1;
                }
        }

    m_outside.assign(n, 0);
    m_queue.clear();
    m_queue.reserve(n);
    auto push = [&](int x, int y) {
        size_t i = static_cast<size_t>(y) * m_w + x;
        if (m_cell[i] == Wall || block[i] || m_outside[i]) return;
        m_outside[i] = 1;
        m_queue.push_back(static_cast<int>(i));
    };
    for (int x = 0; x < m_w; ++x) { push(x, 0); push(x, m_h - 1); }
    for (int y = 0; y < m_h; ++y) { push(0, y); push(m_w - 1, y); }

    for (size_t head = 0; head < m_queue.size(); ++head) {
        int i = m_queue[head];
        int x = i % m_w, y = i / m_w;
        if (x > 0)       push(x - 1, y);
        if (x < m_w - 1) push(x + 1, y);
        if (y > 0)       push(x, y - 1);
        if (y < m_h - 1) push(x, y + 1);
    }
}

// Расстояние (в пикселях, по Чебышёву) от каждой клетки до ближайшей стены.
// Нужно, чтобы путь шёл по центру коридора, а не прижимался к стенам.
void MazeSolver::computeClearance()
{
    const size_t n = static_cast<size_t>(m_w) * m_h;
    m_clear.assign(n, 1000);
    for (size_t i = 0; i < n; ++i)
        if (m_cell[i] == Wall || m_outside[i]) m_clear[i] = 0;

    for (int y = 0; y < m_h; ++y)
        for (int x = 0; x < m_w; ++x) {
            size_t i = static_cast<size_t>(y) * m_w + x;
            uint16_t d = m_clear[i];
            if (x > 0)             d = std::min<int>(d, m_clear[i - 1] + 1);
            if (y > 0) {
                d = std::min<int>(d, m_clear[i - m_w] + 1);
                if (x > 0)         d = std::min<int>(d, m_clear[i - m_w - 1] + 1);
                if (x < m_w - 1)   d = std::min<int>(d, m_clear[i - m_w + 1] + 1);
            }
            m_clear[i] = d;
        }
    for (int y = m_h - 1; y >= 0; --y)
        for (int x = m_w - 1; x >= 0; --x) {
            size_t i = static_cast<size_t>(y) * m_w + x;
            uint16_t d = m_clear[i];
            if (x < m_w - 1)       d = std::min<int>(d, m_clear[i + 1] + 1);
            if (y < m_h - 1) {
                d = std::min<int>(d, m_clear[i + m_w] + 1);
                if (x < m_w - 1)   d = std::min<int>(d, m_clear[i + m_w + 1] + 1);
                if (x > 0)         d = std::min<int>(d, m_clear[i + m_w - 1] + 1);
            }
            m_clear[i] = d;
        }
}

// Волновой алгоритм Ли: волна от входа (зелёный) расходится по проходимым
// клеткам, пока не дойдёт до выхода (красный); затем путь восстанавливается
// обратным ходом по номерам волн. Проходимы только клетки, удалённые от стен
// больше чем на `clearance` пикселей (кружки - всегда проходимы).
bool MazeSolver::lee(int clearance, std::vector<int>& path)
{
    const size_t n = static_cast<size_t>(m_w) * m_h;
    m_wave.assign(n, -1);
    m_queue.clear();

    auto passable = [&](size_t i) {
        if (m_cell[i] == Green || m_cell[i] == Red) return true;
        return m_cell[i] == Free && !m_outside[i] && m_clear[i] > clearance;
    };

    for (size_t i = 0; i < n; ++i)
        if (m_cell[i] == Green) { m_wave[i] = 0; m_queue.push_back(static_cast<int>(i)); }

    int target = -1;
    for (size_t head = 0; head < m_queue.size() && target < 0; ++head) {
        int i = m_queue[head];
        if (m_cell[i] == Red) { target = i; break; }
        int x = i % m_w, y = i / m_w;
        int nb[4]; int cnt = 0;
        if (x > 0)       nb[cnt++] = i - 1;
        if (x < m_w - 1) nb[cnt++] = i + 1;
        if (y > 0)       nb[cnt++] = i - m_w;
        if (y < m_h - 1) nb[cnt++] = i + m_w;
        for (int k = 0; k < cnt; ++k) {
            int j = nb[k];
            if (m_wave[j] < 0 && passable(j)) {
                m_wave[j] = m_wave[i] + 1;
                m_queue.push_back(j);
            }
        }
    }
    if (target < 0) return false;

    // Обратный ход: от выхода к входу по убывающим номерам волны.
    // Стараемся идти в прежнем направлении - путь получается прямым.
    path.clear();
    int cur = target;
    int lastStep = 0;
    const int steps[4] = { 1, -1, m_w, -m_w };
    while (true) {
        path.push_back(cur);
        if (m_wave[cur] == 0) break;
        int next = -1;
        int order[5] = { lastStep, steps[0], steps[1], steps[2], steps[3] };
        for (int k = 0; k < 5 && next < 0; ++k) {
            int s = order[k];
            if (s == 0) continue;
            int j = cur + s;
            if (j < 0 || j >= static_cast<int>(n)) continue;
            int cx = cur % m_w, jx = j % m_w;
            if ((s == 1 || s == -1) && std::abs(cx - jx) != 1) continue;   // переход через край строки
            if (m_wave[j] == m_wave[cur] - 1) { next = j; lastStep = s; }
        }
        if (next < 0) return false;
        cur = next;
    }
    std::reverse(path.begin(), path.end());
    return true;
}

// Рисуем путь синей линией толщиной 3 пикселя (стены и кружки не трогаем)
void MazeSolver::drawPath(const std::vector<int>& path)
{
    for (size_t k = 0; k < path.size(); ++k) {
        int x = path[k] % m_w, y = path[k] / m_w;
        for (int dy = -1; dy <= 1; ++dy)
            for (int dx = -1; dx <= 1; ++dx) {
                int nx = x + dx, ny = y + dy;
                if (nx < 0 || ny < 0 || nx >= m_w || ny >= m_h) continue;
                size_t i = static_cast<size_t>(ny) * m_w + nx;
                if (m_cell[i] == Free && !m_outside[i]) m_bmp.setPixel(nx, ny, 0, 0, 255);
            }
    }
}

bool MazeSolver::solve(std::string& error)
{
    m_pathLength = 0;
    classify();

    bool hasGreen = false, hasRed = false;
    for (size_t i = 0; i < m_cell.size(); ++i) {
        if (m_cell[i] == Green) hasGreen = true;
        if (m_cell[i] == Red)   hasRed = true;
    }
    if (!hasGreen || !hasRed) {
        error = "На изображении не найдены зелёный (вход) и красный (выход) кружки";
        return false;
    }

    const int seals[3] = { 2, 4, 7 };
    std::vector<int> path;
    for (int s = 0; s < 3; ++s) {
        markOutside(seals[s]);
        computeClearance();
        // Сначала ищем путь строго по центру коридоров, затем допускаем ближе к стенам
        for (int clearance = 8; clearance >= 0; --clearance) {
            if (lee(clearance, path)) {
                m_pathLength = path.size();
                drawPath(path);
                return true;
            }
        }
    }
    error = "Путь из лабиринта не найден";
    return false;
}
