#include "common.h"
// Файл: db_functions.cpp
// Содержит фукнции для работы с базой данных
//---------------------------------------------------------------------------------------------------------------------------------------------------------
// Функция для вывода базы данных на экран. Доступ к файлу - последовательный
void print_DB() {
    Train train = {0}; // Структура для чтения одной записи
    FILE* file_ptr;
    long long index = 0; // Порядковый номер для отображения
    long long count = 0; // Количество записей в файле

    file_ptr = fopen(get_database_file_path(), "rb");
    if (file_ptr == NULL) {
        print_error("Ошибка: Не удалось открыть файл.\n");
        return;
    }

    count = get_record_count(file_ptr); // Получаем количество записей
    
    // Проверяем, есть ли вообще записи
    if (count < 0) {
        print_error("Ошибка чтения базы данных.\n");
        return;
    }
    if (count == 0) {
        print_info("База данных пуста.\n");
        return;
    }

    // Файл не пуст – начинаем печать
    print_table_header();
    while (1) {
        train = read_train_at_current(file_ptr); // Читаем запись с текущей позиции
        if (is_empty_struct(train)) {
            if (feof(file_ptr)) break; // Достигли конца файла - все нормально
            else {
                print_error("Ошибка чтения.\n");
                break;
            }
        }
        else {
            print_train_row(index, train); // выводим строку таблицы
            index++;
        }
    }
    fclose(file_ptr);

}
//---------------------------------------------------------------------------------------------------------------------------------------------------------
// Функция для добавления новой записи в базу данных. Доступ к файлу - последовательный
void add_train() {
	Train new_train = {0};
    FILE* file_ptr;

    printf("\n--- Добавление новой записи ---\n");

    get_fixed_string("Введите ID поезда(цифры или буквы,максимальная длина - 5)\n", ID_SIZE - 1, new_train.train_ID);

    get_fixed_string("Введите пункт отправления\n", NAME_SIZE - 1, new_train.departure_point);

    get_fixed_string("Введите пункт назначения\n", NAME_SIZE - 1, new_train.destination_point);

    new_train.travel_time = get_valid_int("Введите время в пути (в часах, максимум - 99999)\n", 1, 99999);

    new_train.carriage_count = get_valid_int("Введите количество вагонов (максимум - 9999999)\n", 1, 9999999);

    new_train.min_price = get_valid_double("Введите минимальную цену билета(руб)\n", 1, MAX_PRICE - 1);

    do { // Максимульная цена строго больше минимальной
        new_train.max_price = get_valid_double("Введите максимальную цену билета(руб)\n", 1, MAX_PRICE);
        if (new_train.min_price >= new_train.max_price) printf("Минимальная цена больше, чем максимальная или равна ей. Попробуйте ввести снова\n");
    } while (new_train.min_price >= new_train.max_price);

    if (!ask_continue("Уверены, что хотите добавить новую запись?(1 - да, 0 - нет)\n", "Добавление отменено\n")) 
        return;

    // Открываем файл для добавления в конец
    file_ptr = fopen(get_database_file_path(), "ab");

    if (file_ptr == NULL) {
        print_error("Ошибка: не удалось открыть файл для добавления.\n");
        return;
    }

    // Пишем запись в текущую позицию
    if (write_train_at_current(file_ptr, new_train) == 0)
        print_success("Добавление успешно.\n");
    else
        print_error("Ошибка записи.\n");

    fclose(file_ptr);
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------
// Функция для корректировки записи в базе данных. Доступ к файлу - прямой
void edit_train() {
    Train train = {0};
    FILE* file_ptr;
    int continue_edit = 1;

    print_DB();

    file_ptr = fopen(get_database_file_path(), "rb+");

    if (file_ptr == NULL) {
        print_error("Ошибка: Не удалось открыть файл.\n");
        return;
    }

    long long count = get_record_count(file_ptr);

    // Проверяем, есть ли вообще записи
    if (count < 0) {
        print_error("Ошибка чтения базы данных.\n");
        fclose(file_ptr);
        return;
    }

    if (count == 0) {
        print_info("База пуста.\n");
        fclose(file_ptr);
        return;
    }

    long long edit_index = get_valid_int("Введите номер для редактирования\n", 1, count) - 1;

    if (!ask_continue("Начать редактирование? (1 - да, 0 - нет)\n", "Редактирование отменено\n"))
        return;

    // Читаем запись, которую будем редактировать
    train = read_train_by_index(file_ptr, edit_index);

    if (is_empty_struct(train)) {
        print_error("Ошибка: Не удалось считать запись.\n");
        fclose(file_ptr);
        return;
    }

    print_table_header();
    print_train_row(edit_index, train); // показываем текущее содержимое записи

    while (continue_edit) {
        printf("Выберите поле для редактирования\n");
        printf("0. Выход\n1. ID поезда\n2. Пункт отправления\n3. Пункт назначения\n4. Время в пути\n5. Количество вагонов\n6. Минимальная цена\n7. Максимальная цена\n");

        int choice = get_valid_int("Ваш выбор: ", 0, 7);
        switch (choice) {
        case 0:
            continue_edit = 0;
            break;
        case 1:
            get_fixed_string("Введите новый ID поезда\n", ID_SIZE - 1, train.train_ID);
            printf("ID поезда был успешно изменен\n");
            break;
        case 2:
            get_fixed_string("Введите новый пункт отправления\n", NAME_SIZE - 1, train.departure_point);
            printf("Пункт отправления был успешно изменен\n");
            break;
        case 3:
            get_fixed_string("Введите новый пункт назначения\n", NAME_SIZE - 1, train.destination_point);
            printf("Пункт назначения был успешно изменен\n");
            break;
        case 4:
            train.travel_time = get_valid_int("Введите новое время в пути (в часах, максимум - 99999)\n", 1, 99999);
            printf("Время в пути было успешно изменено\n");
            break;
        case 5:
            train.carriage_count = get_valid_int("Введите новое количество вагонов (максимум - 9999999)\n", 1, 9999999);
            printf("Количество вагонов было успешно изменено\n");
            break;
        case 6:
            // При изменении минимальной цены убеждаемся, что она меньше текущей максимальной
            train.min_price = get_valid_double("Введите новую минимальную цену билета(руб)\n", 1, train.max_price - 1);
            printf("Минимальная цена была успешно изменена\n");
            break;
        case 7:
            // При изменении максимальной цены она не может быть меньше минимальной
            train.max_price = get_valid_double("Введите новую максимальную цену билета(руб)\n", train.min_price, MAX_PRICE);
            printf("Максимальная цена была успешно изменена\n");
            break;
        default:
            break;
        }
        // Если не выход, показываем обновлённую запись
        if (choice != 0) {
            print_table_header();
            print_train_row(edit_index, train);
        }
        if (choice != 0 && ask_continue("Продолжить редактирование? (1 - да, 0 - нет)\n", "Редактирование прервано\n") == 0)
            continue_edit = 0;
    }
    // Перезаписываем изменённую запись на то же место
    if (write_train_by_index(file_ptr, edit_index, train) != 0) {
        print_error("Ошибка записи в файл. Изменения не сохранены.\n");
    }
    else {
        print_success("Запись успешно отредактирована.\n");
    }

    fclose(file_ptr);
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------
// Функция для удаления записи из базы данных. Доступ к файлу - последовательный
void delete_train() {
    Train train = {0};
    FILE* source_file_ptr;
    FILE* temp_file_ptr;

    source_file_ptr = fopen(get_database_file_path(), "rb");

    if (source_file_ptr == NULL) {
        print_error("Ошибка: Не удалось открыть файл.\n");
        return;
    }

    long long count = get_record_count(source_file_ptr);

    // Проверяем, есть ли вообще записи
    if (count < 0) {
        print_error("Ошибка чтения базы данных.\n");
        fclose(source_file_ptr);
        return;
    }

    if (count <= 0) {
        print_info("База пуста.\n");
        fclose(source_file_ptr);
        return;
    }

    print_DB();

    long long delete_index = get_valid_int("Введите номер для удаления\n", 1, count);

    int confirm = get_valid_int("Вы уверены, что хотите удалить эту запись? (1 - да, 0 - нет)\n", 0, 1);
    if (!confirm) {
        print_info("Удаление отменено.\n");
        return;
    }

    temp_file_ptr = fopen("temp.txt", "wb");
    if (temp_file_ptr == NULL) {
        print_error("Ошибка: Не удалось создать временный файл.\n");
        fclose(source_file_ptr);
        return;
    }
    rewind(source_file_ptr);

    long long current_index = 1; // Номер текущей записи (для пользователя нумерация с 1)

    // Копируем все записи, кроме удаляемой
    while (1) {
        train = read_train_at_current(source_file_ptr);
        if (is_empty_struct(train)) {
            if (feof(source_file_ptr))
                break; // Конец файла
            else {
                print_error("Ошибка записи во временный файл.\n");
                fclose(source_file_ptr);
                fclose(temp_file_ptr);
                remove("temp.txt");
                return;
            }
        }
        if (current_index != delete_index) { // Не удаляемая запись – копируем
            if (write_train_at_current(temp_file_ptr, train) != 0) {
                print_error("Ошибка записи во временный файл.\n");
                fclose(source_file_ptr);
                fclose(temp_file_ptr);
                remove("temp.txt");
                return;
            }
        }
        current_index++;

    }

    fclose(temp_file_ptr);
    fclose(source_file_ptr);

    // Замена исходного файла временным
    if (remove(get_database_file_path()) != 0) { // Удаляем исходный файл
        print_error("Ошибка: Не удалось удалить исходный файл.\n");
        remove("temp.txt");
        return;
    }
    if (rename("temp.txt", get_database_file_path()) != 0) { // Переименовываем временный в исходный
        print_error("Ошибка: Не удалось переименовать временный файл.\n");
        return;
    }
    
    print_success("Удаление записи успешно.\n");
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------
// Функция для обработки запроса пользователя. Доступ к файлу - последовательный
void process_request() {
    Train train = {0};
    FILE* file_ptr;
    long long count = 0;

    file_ptr = fopen(get_database_file_path(), "rb");
    if (file_ptr == NULL) {
        print_error("Ошибка: Не удалось открыть файл базы данных.\n");
        return;
    }
    count = get_record_count(file_ptr);
    fclose(file_ptr);

    // Проверяем, есть ли вообще записи
    if (count < 0) {
        print_error("Ошибка чтения базы данных.\n");
        return;
    }
    if (count == 0) {
        print_info("База данных пуста. Запрос невозможен.\n");
        return;
    }

    int continue_request = 1; // Выполнять ли ещё один новый запрос?
    while (continue_request) {
        // Переменные фильтров
        int id_filter_usage = 0;
        char id_filter[ID_SIZE] = "";
        int departure_filter_usage = 0;
        char departure_filter[NAME_SIZE] = "";
        int destination_filter_usage = 0;
        char destination_filter[NAME_SIZE] = "";
        int time_filter_usage = 0, min_time = 0, max_time = 0;
        int carriage_filter_usage = 0, min_carriages = 0, max_carriages = 0;
        int price_filter_usage = 0;
        double min_price = 0.0, max_price = 0.0;

        int filter_choice;
        int configuring = 1;
        int canceled = 0;

        print_DB(); // Показываем всю базу перед настройкой

        printf("\n--- Настройка фильтров запроса ---\n");

        while (configuring) {
            // Вывод текущих фильтров
            printf("\nТекущие фильтры:\n");
            if (id_filter_usage) printf("  ID: %s\n", id_filter);
            if (departure_filter_usage) printf("  Отправление: %s\n", departure_filter);
            if (destination_filter_usage) printf("  Назначение: %s\n", destination_filter);
            if (time_filter_usage) printf("  Время в пути: %d - %d ч\n", min_time, max_time);
            if (carriage_filter_usage) printf("  Вагонов: %d - %d\n", min_carriages, max_carriages);
            if (price_filter_usage) printf("  Цена: %.2f - %.2f руб.\n", min_price, max_price);
            if (!id_filter_usage && !departure_filter_usage && !destination_filter_usage &&
                !time_filter_usage && !carriage_filter_usage && !price_filter_usage)
                print_warning("  (ни один фильтр не активен)\n");

            printf("\nВыберите действие:\n");
            printf("0. Отменить запрос (выход)\n");
            printf("1. Добавить/изменить фильтр по ID поезда\n");
            printf("2. Добавить/изменить фильтр по пункту отправления\n");
            printf("3. Добавить/изменить фильтр по пункту назначения\n");
            printf("4. Добавить/изменить фильтр по времени в пути\n");
            printf("5. Добавить/изменить фильтр по количеству вагонов\n");
            printf("6. Добавить/изменить фильтр по цене\n");
            printf("7. Сбросить все фильтры\n");
            printf("8. Выполнить запрос\n");

            filter_choice = get_valid_int("Ваш выбор: ", 0, 8);

            // Обработка выбора 
            switch (filter_choice) {
            case 0:
                printf("Запрос отменён.\n");
                canceled = 1;
                configuring = 0;
                break;
            case 1:
                id_filter_usage = 1;
                get_fixed_string("Введите ID поезда (или часть): ", ID_SIZE - 1, id_filter);
                break;
            case 2:
                departure_filter_usage = 1;
                get_fixed_string("Введите пункт отправления (или часть): ", NAME_SIZE - 1, departure_filter);
                break;
            case 3:
                destination_filter_usage = 1;
                get_fixed_string("Введите пункт назначения (или часть): ", NAME_SIZE - 1, destination_filter);
                break;
            case 4:
                time_filter_usage = 1;
                min_time = get_valid_int("Введите минимальное время в пути (часы): ", 1, 99998);
                max_time = get_valid_int("Введите максимальное время в пути (часы): ", min_time, 99999);
                break;
            case 5:
                carriage_filter_usage = 1;
                min_carriages = get_valid_int("Введите минимальное количество вагонов: ", 1, 9999998);
                max_carriages = get_valid_int("Введите максимальное количество вагонов: ", min_carriages, 9999999);
                break;
            case 6:
                price_filter_usage = 1;
                min_price = get_valid_double("Введите минимальную цену билета (руб.): ", 1.0, MAX_PRICE - 1);
                max_price = get_valid_double("Введите максимальную цену билета (руб.): ", min_price, MAX_PRICE);
                break;
            case 7:
                id_filter_usage = departure_filter_usage = destination_filter_usage = 0;
                time_filter_usage = carriage_filter_usage = price_filter_usage = 0;
                print_warning("Все фильтры сброшены.\n");
                break;
            case 8:
                if (!ask_continue("Выполнить запрос с заданными фильтрами? (1 - да, 0 - отмена): ",
                    "Запрос отменён.\n")) {
                    canceled = 1;
                }
                configuring = 0;  // выход из цикла настройки
                break;
            }

            // Промежуточный вывод после изменения фильтра (кроме отмены и выполнения)
            if (!canceled && filter_choice >= 1 && filter_choice <= 7) {
                printf("\n--- Промежуточные результаты ---\n");
                file_ptr = fopen(get_database_file_path(), "rb");
                if (file_ptr == NULL) {
                    print_error("Ошибка открытия файла.\n");
                    return;
                }

                long long found = 0;
                long long index = 0;
                while (1) {
                    train = read_train_at_current(file_ptr);
                    if (is_empty_struct(train)) {
                        if (feof(file_ptr)) break;
                        else {
                            print_error("Ошибка чтения файла.\n");
                            fclose(file_ptr);
                            return;
                        }
                    }
                    index++;
                    int match = 1;
                    if (id_filter_usage && !stristr(train.train_ID, id_filter)) match = 0;
                    if (departure_filter_usage && !stristr(train.departure_point, departure_filter)) match = 0;
                    if (destination_filter_usage && !stristr(train.destination_point, destination_filter)) match = 0;
                    if (time_filter_usage && (train.travel_time < min_time || train.travel_time > max_time)) match = 0;
                    if (carriage_filter_usage && (train.carriage_count < min_carriages || train.carriage_count > max_carriages)) match = 0;
                    if (price_filter_usage && (train.max_price < min_price || train.min_price > max_price)) match = 0;

                    if (match) {
                        if (found == 0) print_table_header();
                        print_train_row(index, train);
                        found++;
                    }
                }
                fclose(file_ptr);
                if (found == 0) print_info("По вашему запросу ничего не найдено.\n");
                else print_success("Найдено поездов: %lld\n", found);

                if (!ask_continue("\nПродолжить настройку фильтров? (1 - да, 0 - выполнить запрос): ",
                    "Переход к выполнению запроса.\n")) {
                    configuring = 0;   // Выходим из цикла настройки, переходим к финальному выводу
                }
            }
        } // Конец цикла настройки

        if (canceled) {
            continue_request = ask_continue("Выполнить новый запрос? (1 - да, 0 - нет): ",
                "Возврат в главное меню.\n");
            continue;   // Следующий внешний цикл
        }

        // Финальный вывод результатов запроса
        printf("\n--- Результаты запроса ---\n");
        file_ptr = fopen(get_database_file_path(), "rb");
        if (file_ptr == NULL) {
            print_error("Ошибка открытия файла.\n");
            return;
        }

        long long found = 0;
        long long index = 0;
        while (1) {
            train = read_train_at_current(file_ptr);
            if (is_empty_struct(train)) {
                if (feof(file_ptr)) break;
                else {
                    print_error("Ошибка чтения файла.\n");
                    fclose(file_ptr);
                    return;
                }
            }
            index++;
            int match = 1;
            if (id_filter_usage && !stristr(train.train_ID, id_filter)) match = 0;
            if (departure_filter_usage && !stristr(train.departure_point, departure_filter)) match = 0;
            if (destination_filter_usage && !stristr(train.destination_point, destination_filter)) match = 0;
            if (time_filter_usage && (train.travel_time < min_time || train.travel_time > max_time)) match = 0;
            if (carriage_filter_usage && (train.carriage_count < min_carriages || train.carriage_count > max_carriages)) match = 0;
            if (price_filter_usage && (train.max_price < min_price || train.min_price > max_price)) match = 0;

            if (match) {
                if (found == 0) print_table_header();
                print_train_row(index, train);
                found++;
            }
        }
        fclose(file_ptr);
        if (found == 0) print_info("По вашему запросу ничего не найдено.\n");
        else print_success("Найдено поездов: %lld\n", found);

        continue_request = ask_continue("Выполнить новый запрос? (1 - да, 0 - нет): ",
            "Возврат в главное меню.\n");
    }
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------
