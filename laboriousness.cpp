#include <iostream>
#include <fstream>
#include <sstream>
#include </usr/local/include/metis.h>

using namespace std;

    // Входные данные (Если запускать не через MAIN.cpp)
    float k = 0.1; // коэффициент трудоемкости при передачи информации от одного процессора к другому

    // Начальная инициализация
    idx_t n = 0; // размерность матрицы matrix
    idx_t m = 0; // размерность матрицы matrixCircle
    
    // Функция считывает размерность матрицы из файла file.
    void readMatrixSize(const string& file, idx_t& numRows, idx_t& numCols) {
        // Проверка на открываемость файла
        ifstream inFile(file);
        if (!inFile.is_open()) {
            cerr << "Ошибка при открытии файла: " << file << endl;
            return;
        }

        string line;
        if (getline(inFile, line)) {
            stringstream ss(line);
            string value;
            numCols = 0;
            while (ss >> value) {
                ++numCols;
            }
            ++numRows;
        }

        while (getline(inFile, line)) {
            ++numRows;
        }

        inFile.close();
    }
    
    // Функция считывает матрицу из файла file в matrix
    void readMatrix(const string& file, idx_t* matrix, idx_t& numRows, idx_t& numCols){
        ifstream inFile(file);
        
        string line;
        idx_t* ptr = matrix;
        while (getline(inFile, line)) {
            stringstream ss(line);
            idx_t val;
            while (ss >> val) {
                *ptr = val;
                ++ptr;
            }
        }
    }

int main(int argc, char* argv[]) {
    // Связь с MAIN.cpp
    if (argc > 1){
        k = atof(argv[1]);
    }

    // Считываем n и m
    readMatrixSize("matrix.csv", m, n);

    // Выделяем память для матрицы matrix и matrixCircle
    idx_t* matrix = new idx_t[n * n];
    idx_t* matrixCircle = new idx_t[n * n];

    // Считываем матрицуы matrix и matrixCircle из соответствующих файлов
    readMatrix("matrix.csv",matrix, m, n);
    readMatrix("matrixCircle.csv",matrixCircle, m, n);

        // Выводим матрицы 
    // Вывод матрицы matrix
    // for (idx_t i = 0; i < m; ++i) {
    //     for (int j = 0; j < n; ++j) {
    //         cout << matrix[i * n + j] << " ";
    //     }
    //     cout << endl;
    // }
    // cout << endl;

    // Вывод матрицы matrixCircle
    // for (idx_t i = 0; i < m; ++i) {
    //     for (int j = 0; j < m; ++j) {
    //         cout << matrixCircle[i * m + j] << " ";
    //     }
    //     cout << endl;
    // }


    // Считаем количество процессоров
    idx_t N = 0; // Число процессоров
    for (idx_t i = 0; i < m; i++){
        for (idx_t j = 0; j < m; j++){
            if (matrix[i*n + j] > N){
                N=matrix[i*n + j];
            }
        }
    }
    
    // Создаем массивы трудоемкостей у каждого процессора
    idx_t* laborsGeneral = new idx_t[N+1]; // Основная трудоемкость узла
    idx_t* laborsTrans = new idx_t[N+1]; // Количество ребер графа одного процессора для вычисления трудоемкости узла, вызванной передачей информации другому процессору

    // Идентификация нулями
    for (idx_t i = 0; i < N+1; i++){
        laborsGeneral[i] = 0;
        laborsTrans[i] = 0;
    }

    // Заполняем массив трудоемкостей
    for (idx_t row = 0; row < m; ++row) {
        for (idx_t elem = 0; elem < n; ++elem) {
            if (row == 0){ // Первая строка
                if (elem == 0){ // ЛВ - левый верхний элемент
                    // Заполняем основную трудоемкость
                    if (matrixCircle[row*n + elem]==0){ // Вне круга
                        laborsGeneral[matrix[row*n + elem]] += 1; 
                    }
                    else{ // Внутри круга
                        laborsGeneral[matrix[row*n + elem]] += 10; 
                    }

                    // Заполняем дополнительную трудоемкость
                    if (matrix[row*n + elem] != matrix[row*n + elem + 1]){ // Правая грань
                        laborsTrans[matrix[row*n + elem]] += 1; 
                    }
                    if (matrix[row*n + elem] != matrix[row*n + elem + n]){ // Нижняя грань
                        laborsTrans[matrix[row*n + elem]] += 1; 
                    }

                    //cout << "Row: "<< row << ", Col: "<< elem << "       NumProc: "<< matrix[row*n + elem] <<"   laborsGeneral: " <<laborsGeneral[matrix[row*n + elem]]<< "   laborsTrans: " <<laborsTrans[matrix[row*n + elem]] <<endl;
                }
                else if (elem == n-1){ // ПВ - правый верхний элемент
                    // Заполняем основную трудоемкость
                    if (matrixCircle[row*n + elem]==0){ // Вне круга
                        laborsGeneral[matrix[row*n + elem]] += 1; 
                    }
                    else{ // Внутри круга
                        laborsGeneral[matrix[row*n + elem]] += 10; 
                    }

                    // Заполняем дополнительную трудоемкость
                    if (matrix[row*n + elem] != matrix[row*n + elem - 1]){ // Левая грань
                        laborsTrans[matrix[row*n + elem]] += 1; 
                    }
                    if (matrix[row*n + elem] != matrix[row*n + elem + n]){ // Нижняя грань
                        laborsTrans[matrix[row*n + elem]] += 1; 
                    }

                    //cout << "Row: "<< row << ", Col: "<< elem << "       NumProc: "<< matrix[row*n + elem] <<"   laborsGeneral: " <<laborsGeneral[matrix[row*n + elem]]<< "   laborsTrans: " <<laborsTrans[matrix[row*n + elem]] <<endl;
                }
                else{ // 2 - (n-1) элементы
                    // Заполняем основную трудоемкость
                    if (matrixCircle[row*n + elem]==0){ // Вне круга
                        laborsGeneral[matrix[row*n + elem]] += 1; 
                    }
                    else{ // Внутри круга
                        laborsGeneral[matrix[row*n + elem]] += 10; 
                    }

                    // Заполняем дополнительную трудоемкость
                    if (matrix[row*n + elem] != matrix[row*n + elem - 1]){ // Левая грань
                        laborsTrans[matrix[row*n + elem]] += 1; 
                    }
                    if (matrix[row*n + elem] != matrix[row*n + elem + 1]){ // Правая грань
                        laborsTrans[matrix[row*n + elem]] += 1; 
                    }
                    if (matrix[row*n + elem] != matrix[row*n + elem + n]){ // Нижняя грань
                        laborsTrans[matrix[row*n + elem]] += 1; 
                    }

                    //cout << "Row: "<< row << ", Col: "<< elem << "       NumProc: "<< matrix[row*n + elem] <<"   laborsGeneral: " <<laborsGeneral[matrix[row*n + elem]]<< "   laborsTrans: " <<laborsTrans[matrix[row*n + elem]] <<endl;
                    
                }
            }
// / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / 
            else if (row == (m-1)){ // Последняя строка
                if (elem == 0){ // ЛН - левый нижний элемент
                    // Заполняем основную трудоемкость
                    if (matrixCircle[row*n + elem]==0){ // Вне круга
                        laborsGeneral[matrix[row*n + elem]] += 1; 
                    }
                    else{ // Внутри круга
                        laborsGeneral[matrix[row*n + elem]] += 10; 
                    }

                    // Заполняем дополнительную трудоемкость
                    if (matrix[row*n + elem] != matrix[row*n + elem - n]){ // Верхняя грань
                        laborsTrans[matrix[row*n + elem]] += 1; 
                    }
                    if (matrix[row*n + elem] != matrix[row*n + elem + 1]){ // Правая грань
                        laborsTrans[matrix[row*n + elem]] += 1; 
                    }

                    //cout << "Row: "<< row << ", Col: "<< elem << "       NumProc: "<< matrix[row*n + elem] <<"   laborsGeneral: " <<laborsGeneral[matrix[row*n + elem]]<< "   laborsTrans: " <<laborsTrans[matrix[row*n + elem]] <<endl;
                }
                else if (elem == n-1){ // ПН - правый нижний элемент
                    // Заполняем основную трудоемкость
                    if (matrixCircle[row*n + elem]==0){ // Вне круга
                        laborsGeneral[matrix[row*n + elem]] += 1; 
                    }
                    else{ // Внутри круга
                        laborsGeneral[matrix[row*n + elem]] += 10; 
                    }

                    // Заполняем дополнительную трудоемкость
                    if (matrix[row*n + elem] != matrix[row*n + elem - n]){ // Верхняя грань
                        laborsTrans[matrix[row*n + elem]] += 1; 
                    }
                    if (matrix[row*n + elem] != matrix[row*n + elem - 1]){ // Правая грань
                        laborsTrans[matrix[row*n + elem]] += 1; 
                    }

                    //cout << "Row: "<< row << ", Col: "<< elem << "       NumProc: "<< matrix[row*n + elem] <<"   laborsGeneral: " <<laborsGeneral[matrix[row*n + elem]]<< "   laborsTrans: " <<laborsTrans[matrix[row*n + elem]] <<endl;
                }
                else{ // 2 - (n-1) элементы
                    // Заполняем основную трудоемкость
                    if (matrixCircle[row*n + elem]==0){ // Вне круга
                        laborsGeneral[matrix[row*n + elem]] += 1; 
                    }
                    else{ // Внутри круга
                        laborsGeneral[matrix[row*n + elem]] += 10; 
                    }

                    // Заполняем дополнительную трудоемкость
                    if (matrix[row*n + elem] != matrix[row*n + elem - n]){ // Верхняя грань
                        laborsTrans[matrix[row*n + elem]] += 1; 
                    }
                    if (matrix[row*n + elem] != matrix[row*n + elem - 1]){ // Правая грань
                        laborsTrans[matrix[row*n + elem]] += 1; 
                    }
                    if (matrix[row*n + elem] != matrix[row*n + elem + 1]){ // Правая грань
                        laborsTrans[matrix[row*n + elem]] += 1; 
                    }

                    //cout << "Row: "<< row << ", Col: "<< elem << "       NumProc: "<< matrix[row*n + elem] <<"   laborsGeneral: " <<laborsGeneral[matrix[row*n + elem]]<< "   laborsTrans: " <<laborsTrans[matrix[row*n + elem]] <<endl;
                }
            }
// / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / 
            else{ // 2 - (m-1) строки
                if (elem == 0){ // Первые элементы строк
                    // Заполняем основную трудоемкость
                    if (matrixCircle[row*n + elem]==0){ // Вне круга
                        laborsGeneral[matrix[row*n + elem]] += 1; 
                    }
                    else{ // Внутри круга
                        laborsGeneral[matrix[row*n + elem]] += 10; 
                    }

                    // Заполняем дополнительную трудоемкость
                    if (matrix[row*n + elem] != matrix[row*n + elem - n]){ // Верхняя грань
                        laborsTrans[matrix[row*n + elem]] += 1; 
                    }
                    if (matrix[row*n + elem] != matrix[row*n + elem + 1]){ // Правая грань
                        laborsTrans[matrix[row*n + elem]] += 1; 
                    }
                    if (matrix[row*n + elem] != matrix[row*n + elem + n]){ // Правая грань
                        laborsTrans[matrix[row*n + elem]] += 1; 
                    }

                    //cout << "Row: "<< row << ", Col: "<< elem << "       NumProc: "<< matrix[row*n + elem] <<"   laborsGeneral: " <<laborsGeneral[matrix[row*n + elem]]<< "   laborsTrans: " <<laborsTrans[matrix[row*n + elem]] <<endl;
                }
                else if (elem == n-1){ // Последние элементы строк
                    // Заполняем основную трудоемкость
                    if (matrixCircle[row*n + elem]==0){ // Вне круга
                        laborsGeneral[matrix[row*n + elem]] += 1; 
                    }
                    else{ // Внутри круга
                        laborsGeneral[matrix[row*n + elem]] += 10; 
                    }

                    // Заполняем дополнительную трудоемкость
                    if (matrix[row*n + elem] != matrix[row*n + elem - n]){ // Верхняя грань
                        laborsTrans[matrix[row*n + elem]] += 1; 
                    }
                    if (matrix[row*n + elem] != matrix[row*n + elem - 1]){ // Правая грань
                        laborsTrans[matrix[row*n + elem]] += 1; 
                    }
                    if (matrix[row*n + elem] != matrix[row*n + elem + n]){ // Правая грань
                        laborsTrans[matrix[row*n + elem]] += 1; 
                    }

                    //cout << "Row: "<< row << ", Col: "<< elem << "       NumProc: "<< matrix[row*n + elem] <<"   laborsGeneral: " <<laborsGeneral[matrix[row*n + elem]]<< "   laborsTrans: " <<laborsTrans[matrix[row*n + elem]] <<endl;
                }
                else{ // 2 - (n-1) элементы строк
                    // Заполняем основную трудоемкость
                    if (matrixCircle[row*n + elem]==0){ // Вне круга
                        laborsGeneral[matrix[row*n + elem]] += 1; 
                    }
                    else{ // Внутри круга
                        laborsGeneral[matrix[row*n + elem]] += 10; 
                    }

                    // Заполняем дополнительную трудоемкость
                    if (matrix[row*n + elem] != matrix[row*n + elem - n]){ // Верхняя грань
                        laborsTrans[matrix[row*n + elem]] += 1; 
                    }
                    if (matrix[row*n + elem] != matrix[row*n + elem - 1]){ // Правая грань
                        laborsTrans[matrix[row*n + elem]] += 1; 
                    }
                    if (matrix[row*n + elem] != matrix[row*n + elem + 1]){ // Правая грань
                        laborsTrans[matrix[row*n + elem]] += 1; 
                    }
                    if (matrix[row*n + elem] != matrix[row*n + elem + n]){ // Правая грань
                        laborsTrans[matrix[row*n + elem]] += 1; 
                    }

                    //cout << "Row: "<< row << ", Col: "<< elem << "       NumProc: "<< matrix[row*n + elem] <<"   laborsGeneral: " <<laborsGeneral[matrix[row*n + elem]]<< "   laborsTrans: " <<laborsTrans[matrix[row*n + elem]] <<endl;
                }
            }
        }
    }
    

        // Вывод общей и дополнительной трудоемкостей каждого процессора
    // cout << endl;
    // for(idx_t i = 1; i < N+1; i++){
    //     cout << "NumProc: "<< i << "    laborsGeneral: " <<laborsGeneral[i]<< "   laborsTrans: " <<laborsTrans[i] <<endl; 
    // } 

    // Создаем массив с итоговыми трудоемкостями
    float* laborsMain = new float[N+1];

    // Заполняем его
    for (idx_t i = 0; i < N+1; i++){
        laborsMain[i] = laborsGeneral[i] + k * laborsTrans[i];
    }

    // Считаем среднее значение трудоемкости для процессора
    float averageLabor = 0;
    for(idx_t i = 0; i < N+1; i++){
        averageLabor += laborsGeneral[i]; 
        //cout << "averageLabor: "<< averageLabor<< "  laborsGenera["<<i<<"] "<< laborsGeneral[i]<<endl;
    }
    averageLabor = averageLabor / N;
    //cout << averageLabor << endl;


    // Вывод итоговых трудоемкостей каждого процессора
    cout << endl;
    cout << "Average labor intensity:  " << averageLabor << endl;
    for(idx_t i = 1; i < N+1; i++){
        cout << "NumProc: "<< i << "    laborsGeneral: " <<laborsGeneral[i]<< "   laborsTrans: " <<laborsTrans[i] <<"   k: "<< k <<"   laborsMain: " <<laborsMain[i] << "   deviation: "<<laborsMain[i]-averageLabor <<endl; 
    } 

    // Поиск максимального отклонения
    float maxDeviation = 0;
    for (idx_t i = 0; i < N + 1; i++){
        if (maxDeviation < laborsMain[i]-averageLabor){
            maxDeviation = laborsMain[i]-averageLabor;
        }
    }

    //Вывод максимального отклонения
    cout << "Max deviation: "<< maxDeviation << endl;

    // Освобождаем память
    delete[] matrix;
    delete[] matrixCircle;
    delete[] laborsGeneral;
    delete[] laborsTrans;
    delete[] laborsMain;

}
