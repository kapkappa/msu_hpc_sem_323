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
    uint32_t nrows = 0;
    uint32_t ncols = 0;
    std::vector<F> val;

    dense(uint32_t nrows_, uint32_t ncols_) : nrows(nrows_), ncols(ncols_) {
        val.resize(nrows*ncols);
        for (uint64_t it = 0; it < nrows * ncols; it++) {
            val.emplace_back(0);
        }
    }

    void fill() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, INT_MAX);

        for (uint32_t i = 0; i < nrows; i++) {
            for (uint32_t j = 0; j < ncols; j++) {
                val[i * ncols + j] = dis(gen);
            }
        }
    }

    void print() const {
        cout << "Matrix, size: " << nrows << " " << ncols << endl;
        for (uint32_t i = 0; i < nrows; i++) {
            for (uint32_t j = 0; j < ncols; j++)
                cout << val[i * ncols + j] << " ";
            cout << endl;
        }
    }

};

template <typename F>
void write(std::string fname, const dense<F> M) {
    FILE *f = fopen(fname.c_str(), "w");

    char type = 'd';
    fwrite(&type, sizeof(char), 1, f);
    fwrite(&M.ncols, sizeof(uint32_t), 1, f);

    for (uint32_t i = 0; i < M.nrows; i++) {
        for (uint32_t j = 0; j < M.ncols; j++) {
            fwrite(M.val.data(), sizeof(F), M.nrows * M.ncols, f);
        }
    }
    fclose(f);
}

/*
template <typename F>
void multiply(const dense<F> &A, const dense<F> &B, dense<F> &C) {
    if (A.ncols != B.nrows) {
        cout << "Mismatched dimensions" << endl;
    }

    for (uint32_t i = 0; i < A.nrows; i++)
        for (uint32_t j = 0; j < B.ncols; j++)
            for (uint32_t k = 0; k < A.ncols; k++)
                C.val[i * A.nrows + j] += A.val[i * A.nrows + k] * B.val[k * B.nrows + j];
}
*/

int main(int argc, char** argv) {
    uint32_t row1, col1;
    cout << "Enter matrix dimensions:\n";
    cin >> row1;
    cin >> col1;
    dense<I32> A(row1, col1);
    A.fill();
//    A.print();

    cout << "Enter matrix filename\n";
    std::string filename;
    cin >> filename;

    write(filename, A);
    return 0;
}
