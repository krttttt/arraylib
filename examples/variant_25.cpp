#include "arraylib.h"
#include <cstddef>
#include <iomanip>
#include <iostream>

int main() {
    // Исходные данные (умножены на 100)
    int concentration[] = { 124, 131, 118, 145, 139, 127, 152, 133 };
    const std::size_t n = sizeof(concentration) / sizeof(concentration[0]);

    // Настраиваем вывод: 2 знака после запятой
    std::cout << std::fixed << std::setprecision(2);

    std::cout << "Количество проб: " << n << "\n\n";

    // Вызовы функций библиотеки (с делением на 100.0 для перевода в реальные значения)
    double min_val = arr_min(concentration, n) / 100.0;
    double max_val = arr_max(concentration, n) / 100.0;
    double avg_val = arr_average(concentration, n) / 100.0;
    double median_val = arr_median(concentration, n) / 100.0;

    std::cout << "Минимальная концентрация: " << min_val << "\n";
    std::cout << "Максимальная концентрация: " << max_val << "\n";
    std::cout << "Средняя концентрация: " << avg_val << "\n";
    std::cout << "Медиана: " << median_val << "\n";

    // Подсчет проб выше 1.30 
    int count_above = 0;
    for (std::size_t i = 0; i < n; ++i) {
        if (concentration[i] > 130) {
            ++count_above;
        }
    }
    std::cout << "Количество проб выше 1.30: " << count_above << "\n";

    return 0;
}