#include <climits>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>
#include <random>

using std::cout;
using std::endl;
using std::cin;

typedef int32_t I32;
typedef int64_t I64;

template <typename F>
struct dense {
    int32_t nrows = 0;
    int32_t ncols = 0;
    char type;
    std::vector<F> val;

    dense(int32_t nrows_, int32_t ncols_, char type_) : nrows(nrows_), ncols(ncols_), type(type_) {
        val.resize(nrows*ncols);
        for (int64_t it = 0; it < nrows * ncols; it++) {
            val.push_back(0);
        }
    }

    void fill() {
        std::random_device rd;
        std::mt19937 gen(rd());
//        std::uniform_int_distribution<> dis(-INT_MAX, INT_MAX);
//        std::normal_distribution<> dis(0, 1000);
        std::binomial_distribution<> dis(1000, 0.5);

        for (int32_t i = 0; i < nrows; i++) {
            for (int32_t j = 0; j < ncols; j++) {
                val[i * ncols + j] = std::round(dis(gen));
            }
        }
    }

    void print() const {
        cout << "Matrix, size: " << nrows << " " << ncols << endl;
        for (int32_t i = 0; i < nrows; i++) {
            for (int32_t j = 0; j < ncols; j++)
                cout << val[i * ncols + j] << " ";
            cout << endl;
        }
    }

};

template <typename F>
void write(std::string fname, const dense<F> M) {
    FILE *f = fopen(fname.c_str(), "w");

    fwrite(&M.type, sizeof(char), 1, f);
    fwrite(&M.ncols, sizeof(int32_t), 1, f);

    for (int32_t i = 0; i < M.nrows; i++) {
        for (int32_t j = 0; j < M.ncols; j++) {
            fwrite(&M.val[i * M.nrows + j], sizeof(F), 1, f);
        }
    }
    fclose(f);
}

int main(int argc, char** argv) {
    uint32_t row1, col1;
    cout << "Enter matrix dimensions:\n";
    cin >> row1;
    cin >> col1;

//    dense<I32> Matrix(row1, col1, 'd');
    dense<I64> Matrix(row1, col1, 'l');

    Matrix.fill();

    cout << "Enter matrix name\n";
    std::string filename;
    cin >> filename;
    write(filename, Matrix);

    return 0;
}
