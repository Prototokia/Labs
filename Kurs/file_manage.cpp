#include "common.h"
// Файл : file_manage.cpp
// Содержит функции для работы с файлом базы данных
// Все Операции с файлами выполняются в бинарном режиме ("rb", "rb+", "wb", "ab") (так как используем запись строго фиксированной длины)
//---------------------------------------------------------------------------------------------------------------------------------------------------------
static char database_file_path[FILE_PATH_SIZE] = "train_db.txt"; 
//---------------------------------------------------------------------------------------------------------------------------------------------------------
// Функция для получения текущего пути к файлу базы данных
// Возвращает: указатель на строку, содержащую путь
const char* get_database_file_path() {
	return database_file_path;
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------
// Функция для установки нового пути к файлу базы данных
// Аргументы: const char* new_file_path - новый путь к файлу
void set_database_file_path(const char* new_file_path) {
	strncpy(database_file_path, new_file_path, FILE_PATH_SIZE - 1);
	database_file_path[FILE_PATH_SIZE - 1] = '\0';
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------
// Функция для копирования всех данных из одного файла в другой
// Аргументы: const char* source_path - путь к исходному файлу, const char* destination_path - путь к целевому файлу
// Возвращает: 0 - при успехе, 1 - при ошибке
int copy_all_records(const char* source_path, const char* destination_path) {
	FILE* source_file_ptr;
	FILE* destination_file_ptr;

	source_file_ptr = fopen(source_path, "rb");

	if (source_file_ptr == NULL) {
		return 1; // Не удалось открыть исходник
	}

	destination_file_ptr = fopen(destination_path, "wb");

	if (destination_file_ptr == NULL) {
		fclose(source_file_ptr);
		return 1; //  Не удалось открыть целевой файл
	}

	Train train = {0}; // Временная структура
	long long copied_records = 0; 

	while (1) {
		train = read_train_at_current(source_file_ptr); // Читаем одну запись из исходника
		// Проверка: достигли конца файла или случилась ошибка
		if (is_empty_struct(train)) {
			if (feof(source_file_ptr)) 
				break; // Конец файла (все нормально)
			else { // Структура пуста, но конца файла не достигли - ошибка
				fclose(source_file_ptr);
				fclose(destination_file_ptr);
				return 1;
			}
		}
		// Записываем прочитанную запись в целевой файл
		if (write_train_at_current(destination_file_ptr, train) != 0) {
			fclose(source_file_ptr);
			fclose(destination_file_ptr);
			return 1; // Ошибка записи
		}
		copied_records++;
	}
	fclose(source_file_ptr);
	fclose(destination_file_ptr);

	print_info("Перенесено записей %lld", copied_records);

	return 0;
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------
// Функция для сохранения пути к базе данных в файле конфигурации в текущей папке программы
void save_config_file() {
	FILE* config = fopen("dbpath.cfg", "w");
	if (config != NULL) {
		fprintf(config, "%s", database_file_path);
		fclose(config);
	}
	// Если файл не удалось создать, игнорируем – конфигурация не критична.
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------
// Функция для загрузки пути к базе данных из файла конфигурации
void load_config_file() {
	FILE* config = fopen("dbpath.cfg", "rb");
	if (config == NULL) return; // Конфига нет - оставляем путь по умолчанию

	char buffer[FILE_PATH_SIZE];
	if (fgets(buffer, sizeof(buffer), config) != NULL) {
		buffer[strcspn(buffer, "\n")] = '\0';
		// Проверяем, что файл по пути, считанному из файла существует
		FILE* file_ptr = fopen(buffer, "rb");
		if (file_ptr != NULL) {
			fclose(file_ptr);
			set_database_file_path(buffer); // обновляем путь
		}
		// Если файл недоступен, оставляем предыдущий путь
	}
	fclose(config);
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------
// Функция инициализации файла базы данных
// Возвращает: 0 - при успехе, 1 - при ошибке
int init_DB_file() {
	// Пытаемся загрузить путь из файла конфигурации
	load_config_file();

	FILE* file_ptr;

	file_ptr = fopen(database_file_path, "rb+");

	if (file_ptr != NULL) { 
		fclose(file_ptr);
		return 0; // Файл существует и готов к работе
	}

	file_ptr = fopen(database_file_path, "wb");

	if (file_ptr == NULL) 
		return 1; // Не удалось создать файл

	fclose(file_ptr);

	file_ptr = fopen(database_file_path, "rb+"); // Проверяем, что файл можно открыть в режиме чтения-записи
	if (file_ptr == NULL)
		return 1; 

	fclose(file_ptr);

	// Если путь отличается от стандартного, то сохраняем его в конфиг
	if (strcmp(database_file_path, "train_db.txt") != 0) {
		save_config_file();
	}

	return 0;

}
//---------------------------------------------------------------------------------------------------------------------------------------------------------
// Функция для изменения пути к файлу базы данных
// Возвращает: 0 - при успехе, 1 - при ошибке
int change_DB_file() {
	char new_path[FILE_PATH_SIZE];
	FILE* file_ptr;
	char directory_path_name[FILE_PATH_SIZE];
	char file_name[FILE_PATH_SIZE];

	print_info("Текущий путь к базе данных: %s\n", database_file_path);

	get_fixed_string_or_empty("Введите новый путь к файлу (директория) (Enter - отменить): ", FILE_PATH_SIZE - 1, directory_path_name);
	if (strlen(directory_path_name) == 0) {
		print_info("Изменение отменено\n");
		return 0;
	}
	get_fixed_string_or_empty("Введите имя файла (например my_file) (Enter - отменить): ", FILE_PATH_SIZE - 1, file_name);
	if (strlen(file_name) == 0) {
		print_info("Изменение отменено\n");
		return 0;
	}

	// Определяем, нужен ли разделитель (слеш/обратный слеш)
	const char* separator = (directory_path_name[strlen(directory_path_name) - 1] == '\\' || directory_path_name[strlen(directory_path_name) - 1] == '/') ? "" : "\\";

	// Формируем полный путь к файлу
	snprintf(new_path, sizeof(new_path), "%s%s%s.txt", directory_path_name, separator, file_name);

	printf("Будет создан файл: %s\n", new_path);

	if (strcmp(new_path, database_file_path) == 0) {
		print_error("Ошибка: Новый путь совпадает с текущим.\n");
		return 1;
	}

	// Проверяем, не существует ли уже файл с таким именем
	file_ptr = fopen(new_path, "rb");
	if (file_ptr != NULL) {
		print_error("Ошибка: Файл с таким именем уже существует. Перезапись данных отменена.\n");
		fclose(file_ptr);
		return 1;
	}

	if (copy_all_records(database_file_path, new_path) != 0) {
		print_error("При копировании записей произошла ошибка. Изменение отменено.\n");
		remove(new_path);
		return 1;
	}

	// Сохраняем старый путь перед переключением (чтобы удалить правильный файл)
	char old_path[FILE_PATH_SIZE];
	strcpy(old_path, database_file_path);

	// Устанавливаем новый путь и сохраняем его в конфиг
	set_database_file_path(new_path);
	save_config_file();

	// Удаляем старый файл
	if (remove(old_path) != 0) {
		print_warning("\nПредупреждение: не удалось удалить старый файл '%s'.\n", old_path);
	}

	print_success("\nПуть к файлу базы данных успешно изменен на '%s'.\n", database_file_path);

	return 0;
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------
// Функция для записи структуры train, начиная с текущей позиции
// Аргументы: FILE* file_ptr - открытый файл в режиме допускающем запись, Train train - данные для записи
// Возвращает: 0 - при успехе, 1 - при ошибке
int write_train_at_current(FILE* file_ptr, Train train) {
	char line[RECORD_SIZE + 1] = ""; // Буфер для форматированной строки
	int valid = 0; // Флаг для проверки длинны строки
	int written = 0; // Количество записанных байт

	// Формируем запись
	valid = snprintf(line, sizeof(line), "%-5.5s%-30.30s%-30.30s%5d%7d%50.2f%50.2f\r\n",
		train.train_ID,
		train.departure_point,
		train.destination_point,
		train.travel_time,
		train.carriage_count,
		train.min_price,
		train.max_price);

	// Проверяем, что длина строки (без завершающего нуля) точно равна RECORD_SIZE
	if (valid != RECORD_SIZE)
		return 1;

	line[RECORD_SIZE] = '\0';

	// Записываем ровно RECORD_SIZE байт в файл
	written = fwrite(line, 1, RECORD_SIZE, file_ptr);

	if (written != RECORD_SIZE)
		return 1; // Ошибка записи

	// Сбрасываем буфер, заставляя систему сделать запись прямо сейчас, не дожидаясь закрытия файла
	fflush(file_ptr);

	return 0;
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------
// Функция для записи структуры train, начиная с текущей позиции
// Аргументы: FILE* file_ptr - открытый файл в режиме чтения,
// Возвращает: заполненую структуру при успехе, пустую структуру при ошибке или конце файла
Train read_train_at_current(FILE* file_ptr) {
	Train train = {0};
	char line[RECORD_SIZE + 1] = "";
	int valid = 0;
	int parsed = 0;

	// Читаем ровно RECORD_SIZE байт
	valid = fread(line, 1, RECORD_SIZE, file_ptr);

	if (valid != RECORD_SIZE)
		return train; // неполная запись или конец файла

	line[RECORD_SIZE] = '\0';
	char temp[51];  // Временный буфер для считывания полей (достаточно для поля 50 символов + '\0')

	// Последовательный ручной разбор полей записи фиксированной длины (179 байт).
	// Точно знаем смещение и ширину каждого поля, поэтому извлекаем их побайтово,
	// Убираем пробелы справа у строк и преобразуем числа с помощью atoi/atof.
	// --- ID (позиции 0-4, ширина 5) ---
	strncpy(temp, line, 5);
	temp[5] = '\0';
	delete_right_spaces(temp);
	strcpy(train.train_ID, temp);

	// --- Пункт отправления (позиции 5-34, ширина 30) ---
	strncpy(temp, line + 5, 30);
	temp[30] = '\0';
	delete_right_spaces(temp);
	strcpy(train.departure_point, temp);

	// --- Пункт назначения (позиции 35-64, ширина 30) ---
	strncpy(temp, line + 35, 30);
	temp[30] = '\0';
	delete_right_spaces(temp);
	strcpy(train.destination_point, temp);

	// --- Время в пути (позиции 65-69, ширина 5) ---
	strncpy(temp, line + 65, 5);
	temp[5] = '\0';
	train.travel_time = atoi(temp); // atoi автоматически игнорирует ведущие пробелы

	// --- Количество вагонов (позиции 70-76, ширина 7) ---
	strncpy(temp, line + 70, 7);
	temp[7] = '\0';
	train.carriage_count = atoi(temp);

	// --- Минимальная цена (позиции 77-126, ширина 50) ---
	strncpy(temp, line + 77, 50);
	temp[50] = '\0';
	train.min_price = atof(temp); // atof также пропускает пробелы

	// --- Максимальная цена (позиции 127-176, ширина 50) ---
	strncpy(temp, line + 127, 50);
	temp[50] = '\0';
	train.max_price = atof(temp);

	return train;
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------
// Функция для перезаписи записи в файле по указанному индексу
// Аргументы: FILE* file_ptr - указатель на открытый файл в режиме rb+, long long index - номер записи, Train train - новые данные для записи
// Возвращает: 0 - при успехе, 1 - при ошибке
int write_train_by_index(FILE* file_ptr, long long index, Train train) {
	long long offset = index * RECORD_SIZE; // Вычисляем смещение относительно начала файла 

	// Перемещаем файловый указатель на начало нужной записи
	if (fseek(file_ptr, offset, SEEK_SET) != 0)
		return 1; // возникла ошибка при позиционировании

	return write_train_at_current(file_ptr, train); // Записываем в текущую позицию
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------
// Функция для чтения записи в файле по указанному индексу
// Аргументы: FILE* file_ptr - указатель на открытый файл в режиме rb+ или rb, long long index - номер записи
// Возвращает: пустую структуру при ошибке, структуру train с данными при успехе
Train read_train_by_index(FILE* file_ptr, long long index) {
	Train train = {0};
	long long offset = index * RECORD_SIZE;

	if (fseek(file_ptr, offset, SEEK_SET) != 0)
		return train;

	return read_train_at_current(file_ptr);
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------
// Функция для подсчета количества записей в открытом файле
// Аргументы: FILE* file_ptr - файл, открытый в режиме чтения
// Возвращает: Количество записей или -1 при ошибке
long long get_record_count(FILE* file_ptr) {
	Train train = {0};
	long long current_pos = ftell(file_ptr); // Запоминаем текущую позицию, чтобы восстановить её после подсчёта

	rewind(file_ptr);

	long long count = 0;

	while (1) {
		train = read_train_at_current(file_ptr);
		if (is_empty_struct(train)) {
			if (feof(file_ptr))
				break; // Достигнут конец файла – нормальное завершение
			else {
				fseek(file_ptr, current_pos, SEEK_SET); // Восстанавливаем исходную позицию
				return -1; // Ошибка чтения
			}
		}
		count++;
	}
	fseek(file_ptr, current_pos, SEEK_SET);

	return count;
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------
