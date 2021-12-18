#include <climits>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>
#include <random>
#include <cassert>

using std::cout;
using std::endl;
using std::cin;

typedef int32_t I32;
typedef int64_t I64;

struct dense {
    int32_t nrows = 0;
    int32_t ncols = 0;
    int32_t elems = 0;
    char elem_type = 0;
    enum {I32, I64} type;
    std::vector<int32_t> possible_d_val;
    std::vector<int64_t> possible_l_val;

    dense(int32_t nrows_, int32_t ncols_, char type_) : nrows(nrows_), ncols(ncols_), elems(ncols_ * nrows_), elem_type(type_) {
        if (type_ == 'd')
            type = I32;
        else if (type_ == 'l')
            type = I64;
    }

    void fill() {
        std::random_device rd;
        std::mt19937 gen(rd());
//        std::uniform_int_distribution<> dis(-INT_MAX, INT_MAX);
//        std::normal_distribution<> dis(0, 1000);
        std::binomial_distribution<> dis(1000, 0.5);
        if (type == I32) {
            possible_d_val.resize(0);
            for (int32_t i = 0; i < elems; i++) {
                possible_d_val.push_back(std::round(dis(gen)));
            }
        } else if (type == I64) {
            possible_l_val.resize(0);
            for (int32_t i = 0; i < elems; i++) {
                possible_l_val.push_back(std::round(dis(gen)));
            }
        }
    }

    void print() const {
        cout << "Matrix, size: " << nrows << " " << ncols << endl;
        if (type == I32) {
            for (int32_t i = 0; i < nrows; i++) {
                for (int32_t j = 0; j < ncols; j++)
                    cout << possible_d_val[i * ncols + j] << " ";
                cout << endl;
            }
        } else if (type == I64) {
            for (int32_t i = 0; i < nrows; i++) {
                for (int32_t j = 0; j < ncols; j++)
                    cout << possible_d_val[i * ncols + j] << " ";
                cout << endl;
            }
        }
    }

    void write(std::string);
};

void dense::write(std::string fname) {
    FILE *f = fopen(fname.c_str(), "w");

    int cnt = 0;
    cnt = fwrite(&elem_type, sizeof(char), 1, f);
    cnt += fwrite(&nrows, sizeof(int32_t), 1, f);
    cnt += fwrite(&ncols, sizeof(int32_t), 1, f);
    assert(cnt == 3);
    cnt = 0;
    if (type == I32) {
        for (int32_t i = 0; i < elems; i++) {
            cnt += fwrite(&possible_d_val[i], sizeof(int32_t), 1, f);
        }
    } else if (type == I64) {
        for (int32_t i = 0; i < elems; i++) {
            cnt += fwrite(&possible_l_val[i], sizeof(int64_t), 1, f);
        }
    }
    fclose(f);
    assert(cnt == elems);
}

int main(int argc, char** argv) {
    int32_t rows, cols;
    cout << "Enter matrix dimensions:\n";
    cin >> rows;
    cin >> cols;

    char matrix_type;
    cout << "Enter matrix type\n'd' for int32 and 'l' for int64\n";
    cin >> matrix_type;
    cout << endl;
    dense matrix_d(rows, cols, 'd');
    dense matrix_l(rows, cols, 'l');

    if (matrix_type == 'd')
        matrix_d.fill();
    else if (matrix_type == 'l')
        matrix_l.fill();

    cout << "Enter matrix name\n";
    std::string filename;
    cin >> filename;

    if (matrix_type == 'd')
        matrix_d.write(filename);
    else if (matrix_type == 'l')
        matrix_l.write(filename);

    return 0;
}
