#include "common.h"
// Файл: validation.cpp
// Содержит функции безопасного ввода данных от пользователя.
// Обеспечивают проверку корректности ввода (тип, диапазон, длина строки)

//---------------------------------------------------------------------------------------------------------------------------------------------------------
// Функция Безопасного ввода целого числа с проверкой диапазона.
// Параметры: Приглашение prompt, минимальное и максимальное допустимые значения.
// Возвращает корректное число, очищая буфер после ввода.
int get_valid_int(const char* prompt, int min, int max) {
    int value, valid;
    char next_char;
    do {
        printf("%s", prompt);
        valid = scanf("%d", &value);
        next_char = getchar();
        if (next_char != '\n') { // если это не Enter
            clean_input_buffer(); // очищаем остаток строки
            valid = 0; // считаем ввод невалидным
        }
        if (valid != 1) {
            print_error("Ошибка: Введено не целое число. Попробуйте снова\n");
        }
        else if (value < min || value > max) {
            print_error("Ошибка: Введенное число не попадает в заданный диапазон (%d;%d). Попробуйте снова\n", min, max);
        }

    } while (valid != 1 || value < min || value > max);

    return value;
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------
// Функция запроса продолжения действия.
// Выводит приглашение prompt1, ожидает ввод 0/1.
// Если пользователь ввел 0, выводит prompt2 и возвращает 0 (отмена).
// Если пользователь ввел 1 – возвращает 1 (продолжить).
int ask_continue(const char* prompt1, const char* prompt2) {
    int cont = get_valid_int(prompt1, 0, 1);
    if (!cont) {
        print_info("%s", prompt2);
        return 0;
    }
    return 1;
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------
// Функция Безопасного ввода вещественного числа с проверкой диапазона.
// Параметры: Приглашение prompt, минимальное и максимальное допустимые значения.
// Возвращает корректное число, очищая буфер после ввода.
double get_valid_double(const char* prompt, double min, double max) {
    double value;
    int valid;
    char next_char;
    do {
        printf("%s", prompt);
        valid = scanf("%lf", &value);
        next_char = getchar();
        if (next_char != '\n') { // если это не Enter
            clean_input_buffer(); // очищаем остаток строки
            valid = 0; // считаем ввод невалидным
        }
        if (valid != 1) {
            print_error("Ошибка: Введено не вещественное число. Попробуйте снова\n");
        }
        else if (value < min || value > max) {
            print_error("Ошибка: Введенное число не попадает в заданный диапазон (%g; %g). Попробуйте снова\n", min, max);
        }
    } while (valid != 1 || value < min || value > max);

    return value;
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------
// Функция ввода строки фиксированной максимальной длины (Не допускает ввод пустой строки).
// Приглашение prompt, max_length – максимальное количество символов (без учета '\0').
// destination – массив, в который сохраняется строка.
// Если ввод превышает max_length, выводится ошибка и ввод повторяется.
void get_fixed_string(const char* prompt, int max_length, char* destination) {
    char buffer[MAX_INPUT_BUFFER];
    int valid = 0;
    do {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            print_error("Ошибка ввода. Попробуйте снова\n");
            continue;
        }
        buffer[strcspn(buffer, "\n")] = '\0'; // Удаление символа новой строки

        if (strlen(buffer) == 0) {
            print_error("Ошибка: Строка не может быть пустой.\n");
            continue;
        }
        if (strlen(buffer) > max_length) {
            print_error("Ошибка: Введенная строка слишком длинная. Попробуйте снова\n");
            continue;
        }

        strcpy(destination, buffer);
        valid = 1;
    } while (valid != 1);
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------
// Функция ввода строки фиксированной максимальной длины (Допускает ввод пустой строки).
// Приглашение prompt, max_length – максимальное количество символов (без учета '\0').
// destination – массив, в который сохраняется строка.
// Если ввод превышает max_length, выводится ошибка и ввод повторяется.
void get_fixed_string_or_empty(const char* prompt, int max_length, char* destination) {
    char buffer[MAX_INPUT_BUFFER];
    int valid = 0;
    do {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            print_error("Ошибка ввода. Попробуйте снова\n");
            continue;
        }
        buffer[strcspn(buffer, "\n")] = '\0'; // Удаление символа новой строки

        if (strlen(buffer) > max_length) {
            print_error("Ошибка: Введенная строка слишком длинная. Попробуйте снова\n");
            continue;
        }

        strcpy(destination, buffer);
        valid = 1;
    } while (valid != 1);
}
