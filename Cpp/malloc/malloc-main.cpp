#include <cstdlib>
#include <iterator>
#include <stdlib.h>

template<typename T>
class allocater {
public:
    allocater() {
        cur = (T*)malloc(sizeof(T));
    }

    void set(const T& a) {
        *cur = a;
    }

    ~allocater() {
        free(cur);
    }
private:
    T* cur;
};

int main() {
    int* p = (int*)malloc(sizeof(int));
    *p = 4;
    free(p);
    allocater<double> alloct;
    double a = 3.14;
    alloct.set(a);
    return 0;
}