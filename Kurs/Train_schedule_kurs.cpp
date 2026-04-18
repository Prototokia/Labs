#include "common.h"
// Основная программа
int main()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
    
    if (init_DB_file() != 0) {
        print_error("Ошибка инициализации базы данных. Программа завершена.\n");
        return 1;
    }

    int choice = 0, subchoice = 0, continue_programm = 1;

    while (continue_programm) { // Главный цикл программы
        printf("\n==================== ГЛАВНОЕ МЕНЮ ====================\n");
        printf("0. Выход из системы\n1. Ввод и корректировка списка\n2. Вывод списка\n3. Обработка запроса по списку\n4. Изменить путь к файлу базы данных\n");
        printf("======================================================\n");
        choice = get_valid_int("Выберите пункт меню: ", 0, 4);

        switch (choice) {
        case 0:
            print_info("Работа программы завершена\n");
            continue_programm = 0;
            return 0;
        case 1: { // Подменю ввода и корректировки
            int sub_continue = 1;
            while (sub_continue) {
                printf("\n--- Ввод и корректировка ---\n");
                printf("0. Вернуться в главное меню\n1. Ввести новую запись\n2. Удалить запись\n3. Редактировать поля записи\n");
                subchoice = get_valid_int("Выберите пункт меню: ", 0, 3);
                switch (subchoice) {
                case 0:
                    sub_continue = 0;
                    system("cls");
                    break;
                case 1:
                    system("cls");
                    add_train();
                    break;
                case 2:
                    system("cls");
                    delete_train();
                    break;
                case 3:
                    system("cls");
                    edit_train();
                    break;
                default:
                    break;
                }
            }
            // После выхода из подменю спрашиваем, продолжать ли работу в подменю
            if (sub_continue)
                sub_continue = ask_continue("Продолжить ввод и корректировку? (1 - да, 0 - нет)\n", "Возврат в главное меню\n");
        }
              break;
        case 2:
            system("cls");
            print_DB();
            break;
        case 3:
            system("cls");
            process_request();
            break;
        case 4:
            system("cls");
            change_DB_file();
            break;
        default:
            break;
        }
    } // Конец главного цикла
    return 0;
}
