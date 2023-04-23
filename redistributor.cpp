#include <iostream>
#include <cstdlib>
#include <stddef.h>
#include <math.h>
#include </usr/local/include/metis.h>
#include <fstream>


using namespace std;


    // Входные данные
    idx_t N = 5; // Число процессоров
    idx_t m = 7; // Количество строк матрицы 
    idx_t n = m; // Количество столбцов матрицы
    idx_t nnodes = n * m; // Общее количество узлов
    idx_t ncon = 1; // Количество весов (трудоемкость)
    
    idx_t Radius = 4; // Радиус круга, задаваемый количеством узлов (шт).
    idx_t Speed = 1; //1,4 Количество узлов, проходимых центром круга по диагонали за секунду (шт/сек).
    idx_t Time = 2; // Время, которое двигался круг по диагонали (сек).

    // Функция, связывающая файл redistributor.cpp и MAIN.cpp
    void getMAINvalues(idx_t N1, idx_t m1, idx_t n1, idx_t nnodes1, idx_t ncon1, idx_t Radius1, idx_t Speed1, idx_t Time1){
        N=N1;
        m = m1;
        n = n1;
        nnodes = nnodes1;
        ncon = ncon1;
        Radius = Radius1;
        Speed = Speed1;
        Time = Time1;
    }


    // Функция возвращает матрицу трудоемкостей, где внутри круга трудоемкость x10, а вне круга x1.
    // R - Радиус круга, задаваемый количеством узлов,  
    // V - количество узлов, проходимых центром крга по диагонали за секунду,
    // T - количество секунд - время, которое двигался круг по диагонали.
    // Круг, центр которого располагался в начальный момент времени в НИЖНЕМ ЛЕВОМ узле, двигался по диагонали, стартуя из НИЖНЕГО ЛЕВОГО узла.
    idx_t* movingCircle (idx_t R, idx_t V, idx_t T){
        idx_t R1 = R-1; 
        idx_t* matrix = new idx_t[n * m]; // То, что возвращаем, начальная инициализация
        for (idx_t i = 0; i < n * m; ++i) { 
            matrix[i] = 0;
        }

        idx_t centerX = m-1-V*T; //Начальное положение центра
        idx_t centerY = V*T;

        idx_t diagonalLength = R1;
        idx_t diagonalCells = diagonalLength * V * T;

        // Рассматриваем каждую точки и считаем расстояние до центра
        for (idx_t i = 0; i < m; ++i) {
            for (idx_t j = 0; j < n; ++j) {
                // Считаем расстрояние от узла до центра
                idx_t distance = sqrt(pow(i - centerX, 2) + pow(j - centerY, 2));
                // Если оно меньше радиуса, отмечаем его 
                if (distance <= R1) {
                    matrix[i * n + j] = 1;
                }
                
            }
        }
        // выделяем последний элемент для наглядности
        matrix[centerX*m + centerY]=2; 
        return matrix;
    }


    // Функция выводит массив, содержащий список всех соседних вершин для каждой вершины графа.
    void printAdjncy(idx_t* adjncy, idx_t* xadj){
        idx_t i2=0;
        idx_t max = xadj[n*m-1]+n;

        //Форматированный вывод.
        for (idx_t i=0; i < n*m; i++){
            cout << "xadj["<< i <<"]:" << xadj[i]<< "   ";
            if (i==n*m-1){
                for (idx_t j=xadj[i]; j<xadj[i]+2; j++){
                    cout << adjncy[j]<<" ";
                }
                cout << endl;
                break;
            }
            for (idx_t j=xadj[i]; j<xadj[i+1]; j++){
                cout << adjncy[j]<<" ";
            }
            cout << endl;
        }
        cout << endl;

        // Вывод xadj.
        cout << "xadj[]= {";
        for (idx_t i = 0; i<n*m;i++){
            cout<< xadj[i]<<", ";
        }
        cout<< xadj[n*m] << "}"<<endl;

        // Вывод adjncy.
        cout << "adjncy[]= {";
        for (idx_t i = 0; i<xadj[n*m-1]+1;i++){
            cout<< adjncy[i]<<", ";
        }
        cout<< adjncy[xadj[n*m-1]+1] << "}"<<endl;
    }


int main(int argc, char* argv[]) {

    //Получение данных из файла MAIN.cpp
    if (argc>1){
        getMAINvalues(stoi(argv[1]),stoi(argv[2]), stoi(argv[3]),
                      stoi(argv[4]),stoi(argv[5]),stoi(argv[6]),
                      stoi(argv[7]),stoi(argv[8]));
    }
    

        // Создание и заполнение матрицы узлов T
    idx_t* xadj = new idx_t[nnodes + 1];   //массив, содержащий индексы начала каждой строки в массиве adjncy.
    idx_t* adjncy = new idx_t[8 + (n-2)*3*2 + (m-2)*3*2 + (n-2)*(m-2)*4];  //массив, содержащий список всех соседних вершин для каждой вершины графа.
    idx_t* vwgt = new idx_t[nnodes * ncon];   //массив, содержащий веса для каждой вершины.
    idx_t* adjwgt = NULL;   //массив, содержащий веса для каждого ребра. В данном случае этот аргумент не используется и передается значение NULL.
    idx_t options[METIS_NOPTIONS];   //массив, содержащий настройки для алгоритма разбиения. В данном случае мы используем значения по умолчанию, которые задаются функцией METIS_SetDefaultOptions().
    METIS_SetDefaultOptions(options);   //устанавливает опции разбиения.
    idx_t* part = new idx_t[nnodes];   //массив, в который будут записаны номера партиций, в которые будут помещены вершины.


        // Заполнение xadj и adjncy
    idx_t strnumInd = 0; // Инкрементируется при заполнении xadj.
    idx_t adjncyInd = 0; // Инкрементируется при заполнении adjncy.
    idx_t strnum = 0; // Счетчик элементов, показывающий на каком элементе начинается строка в adjncy.

    // Для каждой строки матрицы
    for (idx_t row = 0; row < m; ++row) {
        for (idx_t elem = 0; elem < n; ++elem) {
            if (row == 0){ // Первая строка
                if (elem == 0){ // ЛВ - левый верхний элемент
                    xadj[strnumInd] = strnum;
                    strnum += 2;
                    strnumInd++;
                    adjncy[adjncyInd]= row*n + elem+1;
                    adjncyInd++;
                    adjncy[adjncyInd]= row*n + elem+n;
                    adjncyInd++;
                    //cout << "xadj["<<row*n+elem<<"]: "<< xadj[strnumInd-1] <<"   "<<adjncy[adjncyInd-2]<< " "<< adjncy[adjncyInd-1] << endl;
                    //return 0;
                }
                else if (elem == n-1){ // ПВ - правый верхний элемент
                    xadj[strnumInd] = strnum;
                    strnum += 2;
                    strnumInd++;
                    adjncy[adjncyInd]= row*n + elem-1;
                    adjncyInd++;
                    adjncy[adjncyInd]= row*n + elem+n;
                    adjncyInd++;
                    //cout << "xadj["<<row*n+elem<<"]: "<< xadj[strnumInd-1] <<"   "<< adjncy[adjncyInd-2]<< " "<< adjncy[adjncyInd-1] << endl;
                    //return 0;
                }
                else{ // 2 - (n-1) элементы
                    xadj[strnumInd] = strnum;
                    strnum += 3;
                    strnumInd++;
                    adjncy[adjncyInd]= row*n + elem-1;
                    adjncyInd++;
                    adjncy[adjncyInd]= row*n + elem+1;
                    adjncyInd++;
                    adjncy[adjncyInd]= row*n + elem+n;
                    adjncyInd++;
                    //cout << "xadj["<<row*n+elem<<"]: "<< xadj[strnumInd-1] <<"   "<< adjncy[adjncyInd-3]<<" "<< adjncy[adjncyInd-2]<< " "<< adjncy[adjncyInd-1] << endl;
                    //return 0;
                }
            }
// / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / 
            else if (row == (m-1)){ // Последняя строка
                if (elem == 0){ // ЛН - левый нижний элемент
                    xadj[strnumInd] = strnum;
                    strnum += 2;
                    strnumInd++;
                    adjncy[adjncyInd]= row*n + elem-n;
                    adjncyInd++;
                    adjncy[adjncyInd]= row*n + elem+1;
                    adjncyInd++;
                    //cout << "xadj["<<row*n+elem<<"]: "<< xadj[strnumInd-1] <<"   "<< adjncy[adjncyInd-2]<< " "<< adjncy[adjncyInd-1] << endl;
                    //return 0;
                }
                else if (elem == n-1){ // ПН - правый нижний элемент
                    xadj[strnumInd] = strnum;
                    strnum += 2;
                    strnumInd++;
                    adjncy[adjncyInd]= row*n + elem-n;
                    adjncyInd++;
                    adjncy[adjncyInd]= row*n + elem-1;
                    adjncyInd++;
                    //cout << "xadj["<<row*n+elem<<"]: "<< xadj[strnumInd-1] <<"   "<< adjncy[adjncyInd-2]<< " "<< adjncy[adjncyInd-1] << endl;
                    //return 0;
                    // Последний элемент в xadj является количеством элементов в adjncy
                    xadj[strnumInd] = strnum;

                }
                else{ // 2 - (n-1) элементы
                    xadj[strnumInd] = strnum;
                    strnum += 3;
                    strnumInd++;
                    adjncy[adjncyInd]= row*n + elem-n;
                    adjncyInd++;
                    adjncy[adjncyInd]= row*n + elem-1;
                    adjncyInd++;
                    adjncy[adjncyInd]= row*n + elem+1;
                    adjncyInd++;
                    //cout << "xadj["<<row*n+elem<<"]: "<< xadj[strnumInd-1] <<"   "<< adjncy[adjncyInd-3]<<" "<< adjncy[adjncyInd-2]<< " "<< adjncy[adjncyInd-1] << endl;
                    //return 0;
                }
            }
// / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / 
            else{ // 2 - (m-1) строки
                if (elem == 0){ // Первые элементы строк
                    xadj[strnumInd] = strnum;
                    strnum += 3;
                    strnumInd++;
                    adjncy[adjncyInd]= row*n + elem-n;
                    adjncyInd++;
                    adjncy[adjncyInd]= row*n + elem+1;
                    adjncyInd++;
                    adjncy[adjncyInd]= row*n + elem+n;
                    adjncyInd++;
                    //cout << "xadj["<<row*n+elem<<"]: "<< xadj[strnumInd-1] <<"   "<< adjncy[adjncyInd-3]<<" "<< adjncy[adjncyInd-2]<< " "<< adjncy[adjncyInd-1] << endl;
                    //return 0;
                }
                else if (elem == n-1){ // Последние элементы строк
                    xadj[strnumInd] = strnum;
                    strnum += 3;
                    strnumInd++;
                    adjncy[adjncyInd]= row*n + elem-n;
                    adjncyInd++;
                    adjncy[adjncyInd]= row*n + elem-1;
                    adjncyInd++;
                    adjncy[adjncyInd]= row*n + elem+n;
                    adjncyInd++;
                    //cout << "xadj["<<row*n+elem<<"]: "<< xadj[strnumInd-1] <<"   "<< adjncy[adjncyInd-3]<<" "<< adjncy[adjncyInd-2]<< " "<< adjncy[adjncyInd-1] << endl;
                    //return 0;
                }
                else{ // 2 - (n-1) элементы строк
                    xadj[strnumInd] = strnum;
                    strnum += 4;
                    strnumInd++;
                    adjncy[adjncyInd]= row*n + elem-n;
                    adjncyInd++;
                    adjncy[adjncyInd]= row*n + elem-1;
                    adjncyInd++;
                    adjncy[adjncyInd]= row*n + elem+1;
                    adjncyInd++;
                    adjncy[adjncyInd]= row*n + elem+n;
                    adjncyInd++;
                    //cout << "xadj["<<row*n+elem<<"]: "<< xadj[strnumInd-1] <<"   "<< adjncy[adjncyInd-4]<<" "<< adjncy[adjncyInd-3]<<" "<< adjncy[adjncyInd-2]<< " "<< adjncy[adjncyInd-1] << endl;
                    //return 0;
                }
            }
        }
    }
    
    
    //Выводим к каждому узлу смежные узлы
    //printAdjncy(adjncy,xadj);    


    // Получаем маску трудоемкостей
    idx_t* circle = movingCircle(Radius,Speed,Time);


    // Вывод маски
    for (idx_t i = 0; i < m; i++){
        for(idx_t j = 0; j < n; j++){
            cout.width(3);
            cout << circle[i*n + j] <<" ";
        }
        cout << endl;
    }
    cout << endl;



        // Заполнение vwgt (трудоемкость и номер процессора)
    for (idx_t i = 0; i < nnodes; ++i) {
        // Трудоемкость
        if (circle[i]==0){
            vwgt[i * ncon] = 1;
        }
        else {
            vwgt[i * ncon] = 10;
        }
        // Номер процессора
        if (ncon==2){
            vwgt[i * ncon + 1] = 1; 
        }
    }


    // Вывод vwgt.
    // for (idx_t i = 0; i < m; i++){
    //     for(idx_t j = 0; j < n; j++){
    //         cout.width(3);
    //         cout << vwgt[i*n + j] <<" ";
    //     }
    //     cout << endl;
    // }


        // Вызов функции METIS_PartGraphKway() для разбиения графа на части
    idx_t objval; // указатель на переменную, в которую будет записано значение целевой функции, определяющей качество разбиения
    METIS_PartGraphRecursive(&nnodes, &ncon, xadj, adjncy, vwgt, adjwgt, NULL, &N, NULL, NULL, options, &objval, part);
    //METIS_PartGraphKway(&nnodes, &ncon, xadj, adjncy, vwgt, adjwgt, NULL, &N, NULL, NULL, options, &objval, part);


    //return 0;
    cout<< endl;
    idx_t* T = new idx_t(n*m) ;
    //idx_t T[n][m] = {} ;



    // Изменение значения элемента в матрице T
    for (idx_t i = 0; i < m; ++i) { // строки
        for (idx_t j = 0; j < n; ++j) { // столбцы
            idx_t partId = part[i*n + j]; // Номер процессора, на который была разбита текущая вершина
            //T[i][j] = partId + 1; // Изменение значения элемента в матрице T в соответствии с разбиением
            T[i*n + j] = partId + 1; // Изменение значения элемента в матрице T в соответствии с разбиением
        
        }
    }


    // Вывод полученной матрицы T
    cout << "[";
    for (idx_t i = 0; i < m; ++i) {
        cout << "[";
        for (idx_t j = 0; j < n; ++j) {
            cout.width(2);
            //cout << T[i][j];
            cout << T[i*n + j];
            if (j != n - 1) {
                cout << ", ";
            }
        }
        cout << "]";
        if (i != m - 1) {
            cout << ", "<<endl;
        }
    }
    cout << "]" << endl;



    // Вывод маски Circle в файл matrix.csv
    ofstream outfile;
    outfile.open("matrixCircle.csv");
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            //outfile << T[i][j] << " ";
            outfile << circle[i*n+j] << " ";
        }
        outfile << endl;
    }
    // Закрытие файла
    outfile.close();




    // Вывод матрицы T в файл matrix.csv
    outfile.open("matrix.csv");
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            //outfile << T[i][j] << " ";
            outfile << T[i*n+j] << " ";
        }
        outfile << endl;
    }
    // Закрытие файла
    outfile.close();




    // Очистка памяти
    free(xadj);
    free(adjncy);
    free(vwgt);
    free(part);
    free(T);

    return 0;
}
