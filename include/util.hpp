#include <cmath>
#include <fstream>
#include <sstream>
#include <vector>

struct Vector {
    float x, y, z;

    Vector(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
    Vector(float *src) : x(src[0]), y(src[1]), z(src[2]) {}
    Vector &operator=(const Vector &b) {
        x = b.x;
        y = b.y;
        z = b.z;
        return *this;
    }
    Vector operator+(const Vector &b) {
        float x_ = x + b.x;
        float y_ = y + b.y;
        float z_ = z + b.z;
        return Vector(x_, y_, z_);
    }
    Vector &operator+=(const Vector &b) {
        x += b.x;
        y += b.y;
        z += b.z;
        return *this;
    }
    Vector &operator*(float k) {
        x *= k;
        y *= k;
        z *= k;
        return *this;
    }
    Vector &operator*=(float k) {
        x *= k;
        y *= k;
        z *= k;
        return *this;
    }
    Vector operator*(const Vector &b) {
        float _x = y * b.z - z * b.y;
        float _y = z * b.x - x * b.z;
        float _z = x * b.y - y * b.x;
        return Vector(_x, _y, _z);
    }
    Vector operator-(const Vector &b) {
        float _x = x - b.x;
        float _y = y - b.y;
        float _z = z - b.z;
        return Vector(_x, _y, _z);
    }

    float len() { return sqrt(x * x + y * y + z * z); }
    Vector normilize() {
        float length = len();
        return Vector(x / length, y / length, z / length);
    }
    static void assign(float *acc, const Vector &vec) {
        acc[0] = vec.x;
        acc[1] = vec.y;
        acc[2] = vec.z;
    }
    static void add(float *acc, const Vector &vec) {
        acc[0] += vec.x;
        acc[1] += vec.y;
        acc[2] += vec.z;
    }
};

struct Point {
    Vector pos;
    Vector vel;
    Vector acc;

    Vector border;

    Point(Vector _pos, Vector _vel, Vector _acc, Vector _border)
        : pos(_pos), vel(_vel), acc(_acc), border(_border) {}

    void recount() {
        pos += vel;
        if (pos.x > border.x || pos.x < -border.x) {
            vel.x *= -1;
            return;
        }
        if (pos.y < border.y) {
            vel.y *= -1;
            return;
        }
        if (pos.z > border.z) {
            vel.z *= -1;
            return;
        }
        vel += acc;
    }
};

const int QL = 1500;

class TimeCounter {
   private:
    std::vector<float> quants;
    clock_t start_ = 0;

   public:
    int count = 0;

    TimeCounter(int _size) : quants(_size) {}

    void recount(float n_quant) {
        quants[count] = n_quant;
        count = (count + 1) % quants.size();
    };

    float average() {
        float sum = 0;
        for (float q : quants) {
            sum += q;
        }

        return sum / quants.size();
    }

    void start()
    {
        start_ = clock();
    }

    void end()
    {
        clock_t end_ = clock();
        double seconds = (double)(end_ - start_) / CLOCKS_PER_SEC;
        recount(1000*seconds);
    }
};

std::string readFile(const std::string &path) {
    std::ifstream f(path);
    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}