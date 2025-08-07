#ifndef FEM3D_H
#define FEM3D_H
#include <stdbool.h>

// 3D координаты узла
typedef struct {
  double x;
  double y;
  double z;
} coord3d;

// Номера узлов тетраэдра (4 узла)
typedef struct {
  int iys1;
  int iys2;
  int iys3;
  int iys4;
} nodeNumber3d;

// Компоненты деформации в 3D
typedef struct {
  double ex;
  double ey;
  double ez;
  double exy;
  double exz;
  double eyz;
} deformComp3d;

// Компоненты напряжений в 3D
typedef struct {
  double sx;
  double sy;
  double sz;
  double sxy;
  double sxz;
  double syz;
} stressComp3d;

// Функции для решения системы уравнений
bool matrLDLT3d(int ndof, double **kglb);
void direktLDLT3d(int ndof, double **kglb, double *x, double *r);
bool diagLDLT3d(int ndof, double **kglb, double *x);
void rechLDLT3d(int ndof, double **kglb, double *u, double *x);
bool solveLinearSystemLDLT3d(double **kglb, double *u, double *r, double *x, int ndof);

// Функции для расчета напряжений и деформаций
void stressModel3d(int nelem, int **jt03, double **car, double e,
                   double puas, double *u, double **strain, double **stress);

// Функции для сборки глобальной матрицы жесткости
void AssembleLocalStiffnessToGlobal3d(double **kglb, int **jt03, double **car,
                                      int nelem, double e, double puas);

// Функции для работы с памятью
void free_memory(int, ...);

#endif 