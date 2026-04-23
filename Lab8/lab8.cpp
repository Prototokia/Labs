#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <windows.h>
using namespace std;

class Pyramid {
    private:
        double pyramid_base_length = 0;
        double pyramid_base_height = 0;
        double pyramid_height = 0;
        char color[100] = "";
    public:
        // Конструкторы
        Pyramid();
        Pyramid(double base_length, double height, const char* color);
        //Деструктор
        ~Pyramid();
        // Сеттеры
        void set_base_length(double base_length);
        void set_base_height(double base_length);
        void set_height(double height);
        void set_color(const char* color);
        //Геттеры
        double get_base_length();
        double get_base_height();
        double get_height();
        char *get_color();
        // Методы для работы с объектом
        void print();
        double surface_area();
        double volume();
        int compare_pyramids(Pyramid *other);
        void change_color(const char* user_color);
};

Pyramid::Pyramid() {
    pyramid_base_length = 0;
    pyramid_base_height = 0;
    pyramid_height = 0;
    strcpy(color, "Белый");
}

Pyramid::Pyramid(double base_length, double height,const char* user_color) {
    pyramid_base_length = base_length;
    pyramid_base_height = base_length * (sqrt(3.0) / 2.0);
    pyramid_height = height;
    strcpy(color, user_color);
}

Pyramid::~Pyramid() {
    printf("Пирамидки больше нет...\n");
}

void Pyramid::set_base_length(double base_length) {
    pyramid_base_length = base_length;
}

void Pyramid::set_base_height(double base_length) {
    pyramid_base_height = base_length * (sqrt(3.0) / 2.0);
}

void Pyramid::set_height(double height) {
    pyramid_height = height;
}

void Pyramid::set_color(const char* user_color) {
    strcpy(color, user_color);
}

double Pyramid::get_base_length() {
    return pyramid_base_length;
}

double Pyramid::get_base_height() {
    return pyramid_base_height;
}

double Pyramid::get_height() {
    return pyramid_height;
}

char* Pyramid::get_color() {
    return color;
}

void Pyramid::change_color(const char* user_color) {
    strcpy(color, user_color);
}

void Pyramid::print() {
    printf("Длина основания: %lf\n", pyramid_base_length);
    printf("Высота основания: %lf\n", pyramid_base_height);
    printf("Высота пирамиды: %lf\n", pyramid_height);
    printf("Цвет пирамиды %s\n", color);
    printf("Площадь поверхности пирамиды %lf\n", surface_area());
    printf("Объем пирамиды %lf\n", volume());
}

double Pyramid::surface_area() {
    double base_square = 0.5 * pyramid_base_length * pyramid_base_height;
    double base_perimeter = pyramid_base_length * 3;
    double inradius = pyramid_base_length / (2.0 * sqrt(3.0));
    double apothem = sqrt(pyramid_height * pyramid_height + inradius * inradius);
    return base_square + (0.5 * base_perimeter) * apothem;
}

double Pyramid::volume() {
    double base_square = 0.5 * pyramid_base_length * pyramid_base_height;
    return (1.0 / 3.0) * base_square * pyramid_height;
}

int Pyramid::compare_pyramids(Pyramid* other) {
    double volume1 = this->volume();
    double volume2 = other->volume();
    if (volume1 > volume2) return 1;
    if (volume1 < volume2) return -1;
    return 0;
}

int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    Pyramid pyramid1;
    pyramid1.print();
    printf("\n");

    pyramid1.set_base_length(4.95);
    pyramid1.set_base_height(pyramid1.get_base_length());
    pyramid1.set_height(5.9);
    pyramid1.change_color("Красный");

    printf("Высота основания пирамиды: %lf\n", pyramid1.get_base_height());
    printf("Объем и площадь поверхности пирамиды: %lf и %lf\n", pyramid1.volume(), pyramid1.surface_area());
    printf("Цвет пирамиды %s\n", pyramid1.get_color());
    printf("\n");

    Pyramid pyramid2(5.12, 6.69, "Фиолетовый");
    pyramid2.print();

    int compare_result = pyramid1.compare_pyramids(&pyramid2);

    switch (compare_result) {
        case 0:
            printf("Пирамиды равны\n");
            break;
        case 1:
            printf("Первая пирамида больше второй\n");
            break;
        case -1:
            printf("Вторая пирамида больше первой\n");
            break;
    }
    return 0;
}


