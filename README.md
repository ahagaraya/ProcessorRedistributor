# ProcessorRedistributor
Distributes NxN square grid nodes between processors.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

-> Задача:
Реализовать программу, которая разбивает квадратную матрицу вычислительных узлов размерности NxN между P процессорами так, чтобы трудоемкости приблизительно были равны.


-> Дополнительное условие: 
Изначально трудоемкость каждого узла равна 1.
Из левого нижнего узла матрицы начинает двигаться круг, внутри которого трудоемкость x10.

Программа должна эффективно разделить узлы между процессорами. 

Для тестирования введите в терминал:
$  g++ MAIN.cpp
$  ./a.out

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

-> Task:
Implement a program that partitions an NxN square matrix of computational nodes among P processors so that the computational loads are approximately equal.

-> Additional condition:
Initially, the computational load of each node is 1. A circle starts moving from the bottom-left node of the matrix, with a radius of R and a computational load of x10 inside the circle.

The program should efficiently distribute the nodes among processors.

To test, enter into the terminal:
$  g++ MAIN.cpp
$  ./a.out

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
