// Умножение матриц: влияние порядка циклов на время выполнения.
// Фрагмент: A[i][j] = A[i][j] + B[i][k] * C[k][j]
// Сборка: g++ -O0 -std=c++17 matmul_orders.cpp -o matmul   (или -O2 для сравнения)
// Результат выводится в консоль и сохраняется в results.csv (разделитель ';',
// десятичная запятая - открывается в Excel без дополнительных настроек).

#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;
using Matrix = vector<vector<double>>;

// ---------- Шесть вариантов порядка циклов ----------
void mul_ijk(int n, Matrix& A, const Matrix& B, const Matrix& C) {
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            for (int k = 0; k < n; ++k)
                A[i][j] = A[i][j] + B[i][k] * C[k][j];
}
void mul_ikj(int n, Matrix& A, const Matrix& B, const Matrix& C) {
    for (int i = 0; i < n; ++i)
        for (int k = 0; k < n; ++k)
            for (int j = 0; j < n; ++j)
                A[i][j] = A[i][j] + B[i][k] * C[k][j];
}
void mul_jik(int n, Matrix& A, const Matrix& B, const Matrix& C) {
    for (int j = 0; j < n; ++j)
        for (int i = 0; i < n; ++i)
            for (int k = 0; k < n; ++k)
                A[i][j] = A[i][j] + B[i][k] * C[k][j];
}
void mul_jki(int n, Matrix& A, const Matrix& B, const Matrix& C) {
    for (int j = 0; j < n; ++j)
        for (int k = 0; k < n; ++k)
            for (int i = 0; i < n; ++i)
                A[i][j] = A[i][j] + B[i][k] * C[k][j];
}
void mul_kij(int n, Matrix& A, const Matrix& B, const Matrix& C) {
    for (int k = 0; k < n; ++k)
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
                A[i][j] = A[i][j] + B[i][k] * C[k][j];
}
void mul_kji(int n, Matrix& A, const Matrix& B, const Matrix& C) {
    for (int k = 0; k < n; ++k)
        for (int j = 0; j < n; ++j)
            for (int i = 0; i < n; ++i)
                A[i][j] = A[i][j] + B[i][k] * C[k][j];
}

struct Order {
    string name;
    void (*func)(int, Matrix&, const Matrix&, const Matrix&);
};

// Заполнение матрицы детерминированными значениями (одинаковые для всех порядков)
Matrix make_matrix(int n, int seed) {
    Matrix M(n, vector<double>(n));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            M[i][j] = ((i * 31 + j * 17 + seed) % 10) / 10.0;
    return M;
}

// Время одного запуска в секундах
double measure(const Order& ord, int n, const Matrix& B, const Matrix& C,
               double& checksum) {
    Matrix A(n, vector<double>(n, 0.0));
    auto t0 = chrono::steady_clock::now();
    ord.func(n, A, B, C);
    auto t1 = chrono::steady_clock::now();
    checksum = 0;
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j) checksum += A[i][j];
    return chrono::duration<double>(t1 - t0).count();
}

// Число с десятичной запятой для Excel
string fmt(double x) {
    string s = to_string(x);
    for (char& c : s)
        if (c == '.') c = ',';
    return s;
}

int main() {
    // ---------- Настройки эксперимента ----------
    // Три группы размеров. Если компьютер не справляется с большими
    // матрицами - уменьшите значения в группе "большие" или закомментируйте её.
    vector<pair<string, vector<int>>> groups = {
        {"малые",   {20, 40, 60, 80, 100}},
        {"средние", {100, 150, 200, 250, 300}},
        {"большие", {500, 600, 700}},
    };
    const int REPEATS = 5;  // сколько раз измерять каждый размер (берётся среднее)

    vector<Order> orders = {
        {"ijk", mul_ijk}, {"ikj", mul_ikj}, {"jik", mul_jik},
        {"jki", mul_jki}, {"kij", mul_kij}, {"kji", mul_kji},
    };

    ofstream csv("results.csv");
    csv << "Группа;Порядок циклов;Размер матрицы n;Время выполнения (сек)\n";

    cout << "Группа;Порядок;n;Время, сек\n";
    for (const auto& g : groups) {
        for (int n : g.second) {
            Matrix B = make_matrix(n, 1);
            Matrix C = make_matrix(n, 2);
            double reference = -1;  // контрольная сумма для проверки корректности

            for (const auto& ord : orders) {
                double total = 0, checksum = 0;
                for (int r = 0; r < REPEATS; ++r) {
                    total += measure(ord, n, B, C, checksum);
                }
                double avg = total / REPEATS;

                // Результат не должен зависеть от порядка циклов
                if (reference < 0) reference = checksum;
                bool ok = fabs(checksum - reference) < 1e-6 * fabs(reference) + 1e-9;

                csv << g.first << ';' << ord.name << ';' << n << ';' << fmt(avg) << '\n';
                cout << g.first << ';' << ord.name << ';' << n << ';' << fmt(avg)
                     << (ok ? "" : "  <-- РЕЗУЛЬТАТ НЕ СОВПАЛ!") << '\n';
            }
        }
    }

    cout << "\nГотово. Данные сохранены в results.csv\n";
    return 0;
}