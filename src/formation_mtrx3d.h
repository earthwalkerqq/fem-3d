#ifndef FORMATION_MTRX3D
#define FORMATION_MTRX3D

#include "fem3d.h"

// Формирование матрицы деформаций для тетраэдра
double** formationDeformMtrx3d(double** deformMtrx, coord3d coord1,
                               coord3d coord2, coord3d coord3, coord3d coord4);

// Формирование матрицы упругости для 3D
double** formationElastMtrx3d(double** elastMtrx, double e, double puas);

// Расчет матрицы жесткости тетраэдра
void tetrahedronElement(coord3d coord1, coord3d coord2, coord3d coord3,
                        coord3d coord4, double e, double puas, double** gest);

// Сборка локальной матрицы в глобальную
void assemblyGlobMatr3d(nodeNumber3d node, double** gest, double** kglb);

// Расчет напряжений в тетраэдре
void stressTetrahedronElem(coord3d coord1, coord3d coord2, coord3d coord3,
                           coord3d coord4, double e, double puas,
                           double** deformMtrx, double** strsMatr);

// Заполнение закрепленных и нагруженных узлов
void FillConstrainedLoadedNodes3d(int** nodePres, int* lenNodePres,
                                  int** nodeZakrU, int* lenNodeZakrU,
                                  int** nodeZakrV, int* lenNodeZakrV,
                                  int** nodeZakrW, int* lenNodeZakrW,
                                  double** car, int nys);

// Применение закреплений
void MakeConstrained3d(int* nodeZakr, int lenNodeZakr, double** kglb,
                       int ndofysla);

// Задание вектора нагрузок
void SetLoadVector3d(double* r, int lenNodePres, int* nodePres, int ndofysla,
                     int ndof, float load);

// Создание матриц
char makeDoubleMtrx(double* dataMtrx, double*** mtrx, int row, int col);
char makeIntegerMtrx(int* dataMtrx, int*** mtrx, int row, int col);
char makeDoubleArr(double** arr, int size);
char mem_for_arrays(int size, int count_arrs, ...);

// Чтение и запись файлов
char read_from_file(char* filename, int* nys, double** dataCar, double*** car,
                    int* nelem, int** data_jt03, int*** jt03);
void writeResult3d(char* filename, double** strain, double** stress, double* u,
                   int nelem, int nys, int ndof);

#endif