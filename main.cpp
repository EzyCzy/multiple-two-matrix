#include <atomic>
#include <iostream>
#include <pthread.h>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <cmath>

#define MAX_LEN 100
#define WELCOME_MSG_1 "Welcome to MatrixMultiplier A x B\n"
#define WELCOME_MSG_ROW "Please enter the row of matrix (1-100)"
#define WELCOME_MSG_COL "Please enter the col of matrix (1-100)"
#define INVALID_LENGTH "Invalid length (1-100)\n"
#define INVALID_PAIR "Invalid length pair of matrices\n"
#define INPUT_ENTER_MSG "Please enter the inputs for matrix or Input 'r' to generate random inputs."
#define MATRIX_A 'a'
#define MATRIX_B 'b'
#define MAX_THREADS 4
#define RANDOM_GEN 'r'

struct matrix_op {
    int start_row;
    int end_row;
    int col_fir;
    int row_sec;
};

long long matrixA[MAX_LEN][MAX_LEN];
long long matrixB[MAX_LEN][MAX_LEN];
long long matrixC[MAX_LEN][MAX_LEN];

void check_valid_matrix_length (int* len);
void check_valid_matrix_pair (int len_one, int len_two);
void matrix_input(int row, int col, char matrix);
void print_matrix(int row, int col);
void* multiplier(void* arg);

void check_valid_matrix_length (int* len) {
    std::cin >> *len;
    if(*len > 0 && *len <= MAX_LEN) return;
    std::cout << INVALID_LENGTH;
    exit(1);
}

void check_valid_matrix_pair (int len_one, int len_two) {

    if(len_one == len_two) return;
    std::cout << INVALID_PAIR;
    exit(1);
}

void matrix_input(int row, int col, char matrix) {

    char first_input;
    std::cin >> first_input;

    if(first_input == RANDOM_GEN) {

        for(int i = 0; i < row; i++) {
            for(int j = 0; j < col; j++) {
                if(matrix == MATRIX_A) matrixA[i][j] = std::rand() % static_cast<int>(std::sqrt(INT_MAX));
                if(matrix == MATRIX_B) matrixB[i][j] = std::rand() % static_cast<int>(std::sqrt(INT_MAX));
            }
        }
        return;
    }

    if(matrix == MATRIX_A) matrixA[0][0] = first_input - '0';
    if(matrix == MATRIX_B) matrixB[0][0] = first_input - '0';

    for(int i = 0; i < row; i++) {
        for(int j = 0; j < col; j++) {

            if(matrix == MATRIX_A && !(i == 0 && j == 0)) std::cin >> matrixA[i][j];
            if(matrix == MATRIX_B && !(i == 0 && j == 0)) std::cin >> matrixB[i][j];
        }
    }
}

void print_matrix(int row, int col, char matrix) {

    if(matrix == MATRIX_A) {
        for(int i = 0; i < row; i++) {
            for(int j = 0; j < col; j++) {
                std::cout << matrixA[i][j] << " ";
            }
            std::cout << '\n';
        }
    } else if (matrix == MATRIX_B) {
        for(int i = 0; i < row; i++) {
            for(int j = 0; j < col; j++) {
                std::cout << matrixB[i][j] << " ";
            }
            std::cout << '\n';
        }
    } else {
        for(int i = 0; i < row; i++) {
            for(int j = 0; j < col; j++) {
                std::cout << matrixC[i][j] << " ";
            }
            std::cout << '\n';
        }
    }

}

void* multiplier(void* arg) {
    struct matrix_op* operations = static_cast<struct matrix_op *>(arg);
    int begin = operations->start_row;
    int end = operations->end_row;
    for(int i = begin; i <= end; i++) {  // move next rowA
        for(int j = 0; j < operations->col_fir; j++) { // move next colB
            for(int k = 0; k < operations->row_sec; k++) { //move next rowB

                matrixC[i][k] += matrixA[i][j] * matrixB[j][k];
            }
        }
    }

    return nullptr;
}

int main() {

    int rowA, rowB, colA, colB;
    std::cout << WELCOME_MSG_1;
    std::cout << WELCOME_MSG_ROW << " A\n";
    check_valid_matrix_length(&rowA);

    std::cout << WELCOME_MSG_COL << " A\n";
    check_valid_matrix_length(&colA);

    std::cout << WELCOME_MSG_ROW << " B\n";
    check_valid_matrix_length(&rowB);

    std::cout << WELCOME_MSG_COL << " B\n";
    check_valid_matrix_length(&colB);

    check_valid_matrix_pair(colA, rowB);

    std::cout << INPUT_ENTER_MSG << " A (" << rowA << " x " << colA << "):\n";
    matrix_input(rowA, colA, MATRIX_A);
    print_matrix(rowA, colA, MATRIX_A);
    std::cout << INPUT_ENTER_MSG << " B (" << rowB << " x " << colB << "):\n";
    matrix_input(rowB, colB, MATRIX_B);
    print_matrix(rowB, colB, MATRIX_B);

    pthread_t threads[MAX_THREADS];
    struct matrix_op operations[MAX_THREADS];

    for(int i = 0; i < MAX_THREADS && i < rowA; i++) {

        int additional_op = rowA % MAX_THREADS;
        int min_op = rowA / MAX_THREADS;
        operations[i].start_row = (i ? operations[i-1].end_row + 1 : 0);
        operations[i].end_row = operations[i].start_row + min_op + (i < additional_op && rowA > MAX_THREADS ? 1 : 0);
        std::cout << "Thread " << i << ": has " << operations[i].end_row - operations[i].start_row + 1 << " rows\n";
        operations[i].col_fir = colA;
        operations[i].row_sec = rowB;
        pthread_create(&threads[i], nullptr, multiplier, &operations[i]);
    }

    for(int i = 0; i < MAX_THREADS; i++) {
        pthread_join(threads[i], nullptr);
    }

    std::cout << "Your result:\n";
    print_matrix(rowA, colB, 'c');

    return 0;
}
