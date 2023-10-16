#include <algorithm>
#include <cassert>
#include <vector>

using std::vector;

#define COLOR 254
const int PIXEL_SIZE = 16;

struct Point {
    int x = 0;
    int y = 0;

    void reverse() {
        int tmp = x;
        x = y;
        y = tmp;
    }
};

struct Polygon {
    vector<Point> data;

    bool isFinal = false;

    Point &operator[](int i) { return data[i]; }
    int size() { return data.size(); }
    void clear() { data.clear(); }
};

class Field {
   public:
    Field(int width, int height);
    Field(const Field &other);
    Field(Field &&other);
    Field &operator=(const Field &other);
    Field &operator=(Field &&other);
    ~Field() { delete[] data; }

    void clear() {
        for (int i = 0; i < width * height; ++i) data[i] = 0;
    }
    void resize(int new_width, int new_height);
    void put_pixel(int i, int j, int level);
    char get_pixel(int i, int j) {
        if (polar) return data[i * width + j];
        return data[j * width + i];
    }
    char *get_buffer() { return data; }
    int get_height() { return height; }
    int get_width() { return width; }
    void set_polar(bool p) { polar = p; }

    void line(int y, int start, int end);
    void draw_line(Point start, Point end, int I);
    void draw_scan_line(Point start, Point end, int I,
                        vector<vector<int>> &list);
    void put_line(Point start, Point end, vector<vector<int>> &list);
    void put_and_fill(Polygon p, int size);

   private:
    int height;
    int width;
    bool polar;
    char *data;

    void fill(vector<vector<int>> &slist);
};

Field::Field(int width, int height)
    : height(height), width(width), polar(false) {
    data = new char[height * width]{0};
};

Field::Field(const Field &other)
    : height(other.height), width(other.width), polar(other.polar) {
    data = new char[other.height * other.width];

    for (int i = 0; i < height * width; ++i) {
        data[i] = other.data[i];
    }
};

Field::Field(Field &&other) {
    height = other.height;
    width = other.width;
    data = other.data;
    polar = other.polar;
    other.data = nullptr;
}

Field &Field::operator=(const Field &other) {
    height = other.height;
    width = other.width;
    polar = other.polar;

    for (int i = 0; i < height * width; ++i) {
        data[i] = other.data[i];
    }

    return *this;
};

Field &Field::operator=(Field &&other) {
    height = other.height;
    width = other.width;
    polar = other.polar;

    std::swap(data, other.data);
    return *this;
}

void Field::resize(int new_width, int new_height) {
    std::cout << "resize" << std::endl;
    char *new_data = new char[new_height * new_width]{0};
    int min_height = height;
    int min_width = width;

    if (min_height > new_height) min_height = new_height;
    if (min_width > new_width) min_width = new_width;

    for (int j = 0; j < min_height; ++j) {
        for (int i = 0; i < min_height; ++i) {
            int p1 = j * width + i;
            int p2 = j * new_width + i;
            new_data[p2] = data[p1];
        }
    }
    delete[] data;
    data = new_data;
    height = new_height;
    width = new_width;
}

void Field::put_pixel(int i, int j, int level) {
    if (polar) {
        for (int k = 0; k < PIXEL_SIZE; ++k) {
            for (int g = 0; g < PIXEL_SIZE; ++g) {
                data[PIXEL_SIZE * i * width + k * width + PIXEL_SIZE * j + g] =
                    level;
            };
        };
        return;
    }

    for (int k = 0; k < PIXEL_SIZE; ++k) {
        for (int g = 0; g < PIXEL_SIZE; ++g) {
            data[PIXEL_SIZE * j * width + k * width + PIXEL_SIZE * i + g] =
                level;
        };
    };
}

void Field::line(int y, int start, int end) {
    for (int x = start; x <= end; ++x) {
        put_pixel(x, y, COLOR);
    }
}

void Field::draw_line(Point start, Point end, int I) {
    int Dy = end.y - start.y;
    int Dx = end.x - start.x;

    bool f = abs(Dy) > abs(Dx);

    if (f) {
        start.reverse();
        end.reverse();

        set_polar(true);
    }

    if (start.x > end.x) std::swap(start, end);

    Dy = end.y - start.y;
    Dx = end.x - start.x;

    int m = 2 * I * abs(Dy);
    int e = I * abs(Dx);
    int de = m;
    int w = 2 * abs(Dx) * I - m;

    int x = start.x;
    int y = start.y;

    int dy = Dy >= 0 ? 1 : -1;

    while (x <= end.x) {
        ++x;
        if (e >= w) {
            y += dy;
            e -= w;
        } else {
            e += de;
        }
        int level = e / (2 * Dx) * COLOR / I;
        put_pixel(x, y, level);
    }

    set_polar(false);
}

void Field::draw_scan_line(Point start, Point end, int I,
                           vector<vector<int>> &list) {
    int Dy = end.y - start.y;
    int Dx = end.x - start.x;

    bool inv = abs(Dy) > abs(Dx);

    if (inv) {
        start.reverse();
        end.reverse();
    }

    bool dir = start.x > end.x;

    if (dir) std::swap(start, end);

    Dy = end.y - start.y;
    Dx = end.x - start.x;

    int m = 2 * I * abs(Dy);
    int e = I * abs(Dx);
    int de = m;
    int w = 2 * abs(Dx) * I - m;

    int x = start.x;
    int y = start.y;

    int dy = Dy >= 0 ? 1 : -1;

    if (inv) {
        put_pixel(y, x, m / (2 * Dx) * COLOR / I);
        list[x].push_back(y);
        while (x < end.x) {
            ++x;

            list[x].push_back(y);
            if (e >= w) {
                y += dy;
                e -= w;
            } else {
                e += de;
            }
            int level = e / Dx * COLOR / I;
            put_pixel(y, x, level);
        }
    } else {
        if ((dir && dy > 0) || (!dir && dy < 0)) {
            put_pixel(x, y, m / (2 * Dx) * COLOR / I);
            list[y].push_back(x);

            while (x < end.x) {
                ++x;
                if (e >= w) {
                    y += dy;
                    list[y].push_back(x - 1);
                    e -= w;
                } else {
                    e += de;
                }
                int level = e / Dx * COLOR / I;
                put_pixel(x, y, level);
            }
        } else {
            put_pixel(x, y, m / (2 * Dx) * COLOR / I);

            while (x < end.x) {
                ++x;
                if (e >= w) {
                    list[y].push_back(x - 1);
                    y += dy;
                    e -= w;
                } else {
                    e += de;
                }
                int level = e / Dx * COLOR / I;
                put_pixel(x, y, level);
            }
        }
    }
}

void Field::put_line(Point start, Point end, vector<vector<int>> &list) {
    int Dy = end.y - start.y;
    int Dx = end.x - start.x;

    bool f = abs(Dy) > abs(Dx);

    if (f) {
        start.reverse();
        end.reverse();
    }

    if (start.x > end.x) std::swap(start, end);

    int x0 = start.x;
    int y0 = start.y;
    int x1 = end.x;
    int y1 = end.y;
    int deltax = abs(x1 - x0);
    int deltay = abs(y1 - y0);
    int error = 0;
    int deltaerr = (deltay + 1);
    int y = y0;
    int diry = y1 - y0;
    if (diry > 0) diry = 1;
    if (diry < 0) diry = -1;

    if (f) {
        for (int x = x0; x <= x1; ++x) {
            put_pixel(y, x, COLOR);
            list[x].push_back(y);
            error = error + deltaerr;
            if (error >= (deltax + 1)) {
                y = y + diry;
                error = error - (deltax + 1);
            }
        }
    } else {
        for (int x = x0; x <= x1; ++x) {
            put_pixel(x, y, COLOR);
            error = error + deltaerr;
            if (error >= (deltax + 1)) {
                list[y].push_back(x);
                y = y + diry;
                error = error - (deltax + 1);
            }
        }
    }
}

void Field::put_and_fill(Polygon p, int size) {
    assert(size % 2 == 0);

    vector<vector<int>> slist(height);

    for (int i = 0; 2 * i + 1 < size; ++i) {
        draw_scan_line(p[2 * i], p[2 * i + 1], 256, slist);
    }

    if (p.isFinal) {
        fill(slist);
    }
}

void Field::fill(vector<vector<int>> &slist) {
    for (int i = 0; i < height; ++i) {
        std::sort(slist[i].begin(), slist[i].end());
        int size = slist[i].size();
        int y = i;

        for (int j = 0; 2 * j + 1 < size; ++j) {
            int x1 = slist[i][2 * j];
            int x2 = slist[i][2 * j + 1];
            line(y, x1, x2);
        }
    }
}

