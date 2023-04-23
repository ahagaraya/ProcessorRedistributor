#include <cstdlib>
#include <iostream>
#include <cstdio>
#include <stddef.h>
#include <math.h>
//#include <algorithm>
#include </usr/local/include/metis.h>
#include <fstream>

using namespace std;

int main() {
    
    // Входные данные:
    idx_t N = 10; // Число процессоров
    idx_t m = 100; // Количество строк матрицы 
    idx_t n = m; // Количество столбцов матрицы
    idx_t nnodes = n * m; // Общее количество узлов
    idx_t ncon = 1; // Количество весов (трудоемкость)
    
    idx_t Radius = 30; // Радиус круга, задаваемый количеством узлов (шт).
    idx_t Speed = 40; //1,4 Количество узлов, проходимых центром круга по диагонали за секунду (шт/сек).
    idx_t Time = 1; // Время, которое двигался круг по диагонали (сек).



    // Запускаем redistributor.cpp
    system("g++ redistributor.cpp -lmetis -o redistributor");
    string command = "./redistributor " + to_string(N) + " " + to_string(m) + " " + to_string(n) + " " + to_string(nnodes) + " " + to_string(ncon) + " " + to_string(Radius) + " " + to_string(Speed) + " " + to_string(Time);
    system(command.c_str());
    //system( "./redistributor " + to_string(N) + " " + to_string(m) + " " + to_string(n) + " " + to_string(nnodes) + " " + to_string(ncon) + " " + to_string(Radius) + " " + to_string(Speed) + " " + to_string(Time));

    // Запускаем graph.py
    system("python graph.py");

    return 0;
}