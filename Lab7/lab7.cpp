#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <time.h>
#include <windows.h>
#include <string.h>
//---------------------------------------------------------------------------------------------------------------------------------------------------------
#define LINE_SIZE 512
#define MAX_ENTRIES 1000
#define STRING_SIZE 100
#define DESCRIPTION_SIZE 300
#define RESULT_FILE "results.txt"
//---------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct Literature {
    char title[STRING_SIZE];
    char author[STRING_SIZE];
    char genre[STRING_SIZE];
    char description[DESCRIPTION_SIZE];
};

Literature entries[MAX_ENTRIES];
int entry_count = 0;
//---------------------------------------------------------------------------------------------------------------------------------------------------------
//Прототипы функций
// Валидация и вспомогательные функции
void clean_input_buffer(void);
int get_valid_int(const char* prompt, int min, int max);
int ask_continue(const char* prompt1, const char* prompt2);
double get_valid_double(const char* prompt, double min, double max);
void get_fixed_string(const char* prompt, int max_length, char* destination);
void get_fixed_string_or_empty(const char* prompt, int max_length, char* destination);

// Функции обработки строк
void to_lower_str(char* str);
int compare_ignore_case(const char* a, const char* b);
unsigned char tolower_1251(unsigned char c);
char* delete_spaces(char* str);

// Функции работы с файлом данных
void create_empty_file(const char* file_path);
int load_file(const char* file_path);
void add_new_entry(const char* file_path);

// Функции фильтрации и выбора записи
const char* get_genre_filter(int choice);
int count_filtered_entries(const char* genre_filter);
int get_random_filtered_entry_index(const char* genre_filter);

// Игровые режимы и меню
void play_guess_author(const char* genre_filter);
void play_guess_title(const char* genre_filter);
void game_menu(const char* genreFilter);
//---------------------------------------------------------------------------------------------------------------------------------------------------------
// Функция очистки стандартного входного потока
void clean_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
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
            printf("Ошибка: Введено не целое число. Попробуйте снова\n");
        }
        else if (value < min || value > max) {
            printf("Ошибка: Введенное число не попадает в заданный диапазон (%d;%d). Попробуйте снова\n", min, max);
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
        printf("%s", prompt2);
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
            printf("Ошибка: Введено не вещественное число. Попробуйте снова\n");
        }
        else if (value < min || value > max) {
            printf("Ошибка: Введенное число не попадает в заданный диапазон (%g; %g). Попробуйте снова\n", min, max);
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
    char buffer[STRING_SIZE];
    int valid = 0;
    do {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("Ошибка ввода. Попробуйте снова\n");
            continue;
        }
        buffer[strcspn(buffer, "\n")] = '\0'; // Удаление символа новой строки

        if (strlen(buffer) == 0) {
            printf("Ошибка: Строка не может быть пустой.\n");
            continue;
        }
        if (strlen(buffer) > max_length) {
            printf("Ошибка: Введенная строка слишком длинная. Попробуйте снова\n");
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
    char buffer[STRING_SIZE];
    int valid = 0;
    do {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("Ошибка ввода. Попробуйте снова\n");
            continue;
        }
        buffer[strcspn(buffer, "\n")] = '\0'; // Удаление символа новой строки

        if (strlen(buffer) > max_length) {
            printf("Ошибка: Введенная строка слишком длинная. Попробуйте снова\n");
            continue;
        }

        strcpy(destination, buffer);
        valid = 1;
    } while (valid != 1);
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------
// Функция для приведения всей строки в нижний регистр
// Аргументы char* str - строка для модификации 
void to_lower_str(char* str) {
    for (int i = 0; str[i]; i++)
        str[i] = tolower_1251((unsigned char)str[i]);
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------
// Функция преобразования символа в нижний регистр для кодировки Windows-1251.
// Используется в stristr для регистронезависимого поиска русских букв.
// Аргументы: c - исходный символ (unsigned char, чтобы избежать проблем со знаковым расширением).
// Возвращает: символ в нижнем регистре, если возможно, иначе исходный символ.
unsigned char tolower_1251(unsigned char c) {
    if (c >= 0xC0 && c <= 0xDF) // Русские заглавные буквы от А до Я (Кроме Ё)
        return c + 0x20;
    if (c == 0xA8) // Заглавная буква Ё
        return 0xB8;
    return tolower(c); // Для латиницы и других символов
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------
// Регистронезависимое сравнение сравнение строк
// Аргументы: const char* str1, const char* str2
// Возвращает 0 - при совпадении, 1 - при различии
int compare_ignore_case(const char* str1, const char* str2) {
    while (*str1 && *str2) {
        if (tolower_1251((unsigned char)*str1) != tolower_1251((unsigned char)*str2))
            return 1;
        str1++;
        str2++;
    }
    return (*str1 == *str2) ? 0 : 1;
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------
// Функция для удаления всех пробелов в начале и конце строки
// Аргументы: char* str - строка для модификации
char* delete_spaces(char* str) {
    char* end;
    // Пропускаем пробелы в начале
    while (isspace((unsigned char)*str)) 
        str++;
    if (*str == 0) // Если строка состоит только из пробелов, возвращаем пустую
        return str;
    end = str + strlen(str) - 1; // Находим конец строки
    while (end > str && isspace((unsigned char)*end)) 
        end--;
    *(end + 1) = '\0'; // Ставим завершающий ноль после последнего значащего символа
    return str;
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------
// Функция для создания пустого файла
// Аргументы: const char* file_path - путь к файлу
void create_empty_file(const char* file_path) {
    FILE* file_ptr = fopen(file_path, "w");
    if (file_ptr == NULL) {
        printf("Не удалось создать файл %s\n", file_path);
        return;
    }
    fclose(file_ptr);
    printf("Создан пустой файл %s\n", file_path);
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------
// Функция для загрузки записей из файла в массив entries
// Аргументы: const char* file_path - путь к файлу
// Возвращает: количество записей 
int load_file(const char* file_path) {
    FILE* file_ptr = fopen(file_path, "r");

    if (file_ptr == NULL) {
        printf("Ошибка открытия файла\n");
        return -1;
    }

    char line[LINE_SIZE];
    entry_count = 0;

    // Читаем файл построчно, пропуская пустые строки
    while (fgets(line, sizeof(line), file_ptr) && entry_count < MAX_ENTRIES) {
        if (line[0] == '\n' || line[0] == '\0') continue;

        char* title = strtok(line, ";");
        char* author = strtok(NULL, ";");
        char* genre = strtok(NULL, ";");
        char* description = strtok(NULL, ";\n");

        // Если все четыре поля присутствуют
        if (title && author && genre && description) {
            strncpy(entries[entry_count].title, delete_spaces(title), STRING_SIZE - 1);
            strncpy(entries[entry_count].author, delete_spaces(author), STRING_SIZE - 1);
            strncpy(entries[entry_count].genre, delete_spaces(genre), STRING_SIZE - 1);
            strncpy(entries[entry_count].description, delete_spaces(description), DESCRIPTION_SIZE - 1);
            entry_count++;
        }
    }
    fclose(file_ptr);

    return entry_count;
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------
// Функция для добавления новой записи в файл
// Аргументы: const char* file_path - путь к файлу
void add_new_entry(const char* file_path) {
    Literature new_entry;

    printf("\n=== Добавление новой записи ===\n");
    get_fixed_string("Название произведения: ", STRING_SIZE - 1, new_entry.title);
    get_fixed_string("Автор: ", STRING_SIZE - 1, new_entry.author);

    printf("Выберите жанр:\n");
    printf("1 - русская классика\n");
    printf("2 - фэнтези\n");
    int genre_choice = get_valid_int("Ваш выбор: ", 1, 2);
    if (genre_choice == 1)
        strcpy(new_entry.genre, "русская классика");
    else
        strcpy(new_entry.genre, "фэнтези");

    get_fixed_string("Описание: ", DESCRIPTION_SIZE - 1, new_entry.description);

    // Открываем файл для дозаписи
    FILE* file_ptr = fopen(file_path, "a");
    if (file_ptr == NULL) {
        printf("Ошибка: не удалось открыть файл %s для добавления.\n", file_path);
        return;
    }

    // Записываем новую строку с разделителем ';'
    fprintf(file_ptr, "%s;%s;%s;%s\n",
        new_entry.title,
        new_entry.author,
        new_entry.genre,
        new_entry.description);

    fclose(file_ptr);

    if (entry_count < MAX_ENTRIES) {
        entries[entry_count] = new_entry;
        entry_count++;
    }
    else {
        printf("Количество записей привышает максимальное\n");
        return;
    }
        
    printf("Запись успешно добавлена!\n");
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------
// Функция для получения жанра
// Аргументы: int choice - число, соответствующее пункту меню
// Возвращает: строку жанра или NULL, если выбранкт пункт "Все" или иное
const char* get_genre_filter(int choice) {
    switch (choice) {
    case 2: return "русская классика";
    case 3: return "фэнтези";
    default: return NULL; // все
    }
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------
// Функция для подсчета записей, удовлетворяющих фильтру по определенному жанру
// Аргументы: const char* genre_filter - жанр
// Возвращает: количество удовлетворяющих записей
int count_filtered_entries(const char* genre_filter) {
    int count = 0;
    for (int i = 0; i < entry_count; i++) {
        if (genre_filter == NULL || strcmp(entries[i].genre, genre_filter) == 0)
            count++;
    }
    return count;
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------
// Функция для поиска индекса случайно записи, подходящей по фильтру
// Аргументы: const char* genre_filter - жанр
// Возвращает: индекс записи
int get_random_filtered_entry_index(const char* genre_filter) {
    int total = count_filtered_entries(genre_filter); // Число подходящих записей

    if (total == 0)
        return -1;

    int target = rand() % total; // Генерируем случайное число от 0 до total-1
    int current = 0;

    // Ищем запись с порядковым номером target среди отфильтрованных
    for (int i = 0; i < entry_count; i++) {
        if (genre_filter == NULL || strcmp(entries[i].genre, genre_filter) == 0) {
            if (current == target)
                return i;
            current++;
        }
    }

    return -1;
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------
// Функция для запуска игры в режиме "Угадай автора"
// Аргументы: const char* genre_filter - жанр
void play_guess_author(const char* genre_filter) {
    int total = count_filtered_entries(genre_filter);

    if (total == 0) {
        printf("Нет доступных записей для выбранного жанра.\n");
        return;
    }

    int correct = 0, wrong = 0;
    char answer[STRING_SIZE];

    // Игровой цикл
    do {
        // Получаем случайную запись
        int index = get_random_filtered_entry_index(genre_filter);

        if (index == -1)
            return;

        Literature entry = entries[index];

        // Выводим вопрос
        printf("\n--- Угадайте автора ---\n");
        printf("Произведение: %s\n", entry.title);
        printf("Описание: %s\n", entry.description);

        get_fixed_string("Ваш ответ (в формате А.С.Пушкин) (или 'q' для выхода): ", STRING_SIZE - 1, answer);

        // Проверка выхода
        if (strcmp(answer, "q") == 0 || strcmp(answer, "Q") == 0) break;

        // Подготавливаем копии для сравнения без учёта регистра
        char correct_ans[STRING_SIZE];
        char user_ans[STRING_SIZE];

        strcpy(correct_ans, entry.author);
        strcpy(user_ans, answer);

        // Сравниваем
        if (compare_ignore_case(correct_ans, user_ans) == 0) {
            printf("Правильно!\n");
            correct++;
        }
        else {
            printf("Неверно. Правильный ответ: %s\n", correct_ans);
            wrong++;
        }

        if (!ask_continue("Продолжить? (1 - да, 0 - нет): ", "Игра прервана.\n"))
            break;
        
    } while (1);

    // Вывод итоговой статистики
    printf("\nИгра завершена. Правильных: %d, неправильных: %d\n", correct, wrong);

    // Сохраняем результаты в файл
    FILE* result_file = fopen(RESULT_FILE, "a");

    if (result_file == NULL) {
        printf("Ошибка добавления информации в файл\n");
        return;
    }

    fprintf(result_file, "Режим: Угадать автора\n");
    fprintf(result_file, "Правильных: %d, Неправильных: %d\n\n", correct, wrong);
    fclose(result_file);

}
//---------------------------------------------------------------------------------------------------------------------------------------------------------
// Функция для запуска игры в режиме "Угадай произведение"
// Аргументы: const char* genre_filter - жанр
void play_guess_title(const char* genre_filter) {
    int total = count_filtered_entries(genre_filter);
    if (total == 0) {
        printf("Нет доступных записей для выбранного жанра.\n");
        return;
    }

    int correct = 0, wrong = 0;
    char answer[STRING_SIZE];

    do {
        int index = get_random_filtered_entry_index(genre_filter);

        if (index == -1)
            return;

        Literature entry = entries[index];

        printf("\n--- Угадайте произведение ---\n");
        printf("Автор: %s\n", entry.author);
        printf("Описание: %s\n", entry.description);

        get_fixed_string("Ваш ответ (или 'q' для выхода): ", STRING_SIZE - 1, answer);

        if (strcmp(answer, "q") == 0 || strcmp(answer, "Q") == 0) break;

        char correct_ans[STRING_SIZE];
        char user_ans[STRING_SIZE];

        strcpy(correct_ans, entry.title);
        strcpy(user_ans, answer);

        if (compare_ignore_case(correct_ans, user_ans) == 0) {
            printf("Правильно!\n");
            correct++;
        }
        else {
            printf("Неверно. Правильный ответ: %s\n", correct_ans);
            wrong++;
        }

        if (!ask_continue("Продолжить? (1 - да, 0 - нет): ", "Игра прервана.\n"))
            break;

    } while (1);

    printf("\nИгра завершена. Правильных: %d, неправильных: %d\n", correct, wrong);

    FILE* result_file = fopen(RESULT_FILE, "a");

    if (result_file == NULL) {
        printf("Ошибка добавления информации в файл\n");
        return;
    }

    fprintf(result_file, "Режим: Угадать произведение\n");
    fprintf(result_file, "Правильных: %d, Неправильных: %d\n\n", correct, wrong);
    fclose(result_file);

}
//---------------------------------------------------------------------------------------------------------------------------------------------------------
// Меню выбора режима игры для заданного жанра
// Аргументы: const char* genre_filter - жанр
void game_menu(const char* genreFilter) {
    int choice;
    do {
        printf("\n=== Режим игры ===\n");
        printf("0 - Назад\n");
        printf("1 - Угадать автора по произведению\n");
        printf("2 - Угадать произведение по автору\n");
        
        choice = get_valid_int("Ваш выбор: ", 0, 2);

        switch (choice) {
            case 0: 
                break;
            case 1: 
                play_guess_author(genreFilter); 
                break;
            case 2: 
                play_guess_title(genreFilter); 
                break;
        }
    } while (choice != 0);
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------
int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    // Инициализация генератора случайных чисел
    srand((unsigned int)time(NULL));

    // Попытка загрузить файл данных
    int loaded = load_file("literature.txt");
    if (loaded < 0) {
        // Файл не найден – создаём пустой
        printf("Файл данных не найден\n");
        create_empty_file("literature.txt");
        loaded = 0;
    }

    if (entry_count == 0)
        printf("Записей пока нет\n");
    else
        printf("Загружено %d записей.\n", loaded);

    int main_choice;
    // Главный цикл меню
    do {
        printf("\n=== Главное меню ===\n");
        printf("0 - Выход\n");
        printf("1 - Начать игру\n");
        printf("2 - Добавить новую запись\n");
        main_choice = get_valid_int("Ваш выбор: ", 0, 2);

        if (main_choice == 0) break;

        if (main_choice == 2) {
            // Добавление новой записи
            add_new_entry("literature.txt");
            continue; // Возвращаемся в главное меню
        }

        // Пункт "Начать игру"
        if (entry_count == 0) {
            printf("Нет ни одной записи. Сначала добавьте записи через пункт 2.\n");
            continue;
        }

        int genre_choice;
        // Подменю выбора жанра
        do {
            printf("\nВыберите жанр:\n");
            printf("0 - Назад\n");
            printf("1 - Все\n");
            printf("2 - Русская классика\n");
            printf("3 - Фэнтези\n");

            genre_choice = get_valid_int("Ваш выбор: ", 0, 3);
            if (genre_choice == 0) break; // Назад в главное меню

            const char* genre_filter = get_genre_filter(genre_choice);
            game_menu(genre_filter); // Запуск игрового меню с фильтром
        } while (genre_choice != 0);

    } while (1);

    printf("Программа завершена.\n");
    return 0;
}

