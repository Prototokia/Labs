// Файл common.h
// Общий заголовочный файл. Содержит подключения библиотек, константы, прототипы всех функций, структуру Train
#ifndef COMMON_H
#define COMMON_H

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <conio.h>
#include <locale.h>
#include <windows.h>

// ------------------------------- Константы ---------------------------------
#define ID_SIZE 6 
#define NAME_SIZE 31
#define RECORD_SIZE 179
#define FILE_PATH_SIZE 200
#define MAX_PRICE 1e+47 - 1
#define MAX_INPUT_BUFFER 256
#define DB_TMP_FILENAME "temp.txt"
#define WHITE_CODE 7
#define GREEN_CODE 10
#define CYAN_CODE 11
#define RED_CODE 12
#define YELLOW_CODE 14

// ------------------------------- Структура Train ---------------------------------
typedef struct Train {
    char train_ID[ID_SIZE];
    char departure_point[NAME_SIZE];
    char destination_point[NAME_SIZE];
    int travel_time;
    int carriage_count;
    double min_price;
    double max_price;
} Train;

// ------------------------------- validation.cpp ---------------------------------
int get_valid_int(const char* prompt, int min, int max);
void get_fixed_string(const char* prompt, int max_length, char* destination);
void get_fixed_string_or_empty(const char* prompt, int max_length, char* destination);
double get_valid_double(const char* prompt, double min, double max);
int ask_continue(const char* prompt_yes_no, const char* prompt_cancel);

// ------------------------------- utils.cpp ---------------------------------
char* stristr(const char* haystack, const char* needle);
void clean_input_buffer();
unsigned char tolower_1251(unsigned char c);
int is_empty_struct(Train train);
void delete_right_spaces(char* str);

// ------------------------------- colorful_output.cpp ---------------------------------
void set_console_color(int color);
void reset_console_color();
void print_error(const char* format, ...);
void print_warning(const char* format, ...);
void print_success(const char* format, ...);
void print_info(const char* format, ...);

// ------------------------------- ui_output.cpp ---------------------------------
void print_table_header();
void print_train_row(long long index, Train train);

// ------------------------------- file_io.cpp ---------------------------------
const char* get_database_file_path();
void set_database_file_path(const char* new_path);
int init_DB_file();
int change_DB_file();
int copy_all_records(const char* source_path, const char* destination_path);
long long get_record_count(FILE* file_ptr);
Train read_train_by_index(FILE* file_ptr, long long index);
int write_train_by_index(FILE* file_ptr, long long index, Train train);
Train read_train_at_current(FILE* file_ptr);
int write_train_at_current(FILE* file_ptr, Train train);

// ------------------------------- db_ops.cpp ---------------------------------
void print_DB();
void add_train();
void edit_train();
void delete_train();
void process_request();

#endif /* COMMON_H */

