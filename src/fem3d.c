#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "fem3d.h"
#include "formation_mtrx3d.h"

// Функция освобождения памяти
void free_memory(int count, ...) {
  va_list args;
  va_start(args, count);
  for (int i = 0; i < count; i++) {
    void *ptr = va_arg(args, void *);
    if (ptr != NULL) {
      free(ptr);
    }
  }
  va_end(args);
}

// LDLT разложение матрицы
bool matrLDLT3d(int ndof, double **kglb) {
  for (int i = 0; i < ndof; i++) {
    for (int j = 0; j < i; j++) {
      double sum = kglb[i][j];
      for (int k = 0; k < j; k++) {
        sum -= kglb[i][k] * kglb[j][k] * kglb[k][k];
      }
      kglb[i][j] = sum / kglb[j][j];
    }
    
    double sum = kglb[i][i];
    for (int k = 0; k < i; k++) {
      sum -= kglb[i][k] * kglb[i][k] * kglb[k][k];
    }
    kglb[i][i] = sum;
    
    if (fabs(kglb[i][i]) < 1e-12) {
      return false;  // Матрица вырождена
    }
  }
  return true;
}

// Прямой ход LDLT
void direktLDLT3d(int ndof, double **kglb, double *x, double *r) {
  for (int i = 0; i < ndof; i++) {
    double sum = r[i];
    for (int j = 0; j < i; j++) {
      sum -= kglb[i][j] * x[j];
    }
    x[i] = sum;
  }
}

// Диагональный ход LDLT
bool diagLDLT3d(int ndof, double **kglb, double *x) {
  for (int i = 0; i < ndof; i++) {
    if (fabs(kglb[i][i]) < 1e-12) {
      return false;
    }
    x[i] /= kglb[i][i];
  }
  return true;
}

// Обратный ход LDLT
void rechLDLT3d(int ndof, double **kglb, double *u, double *x) {
  for (int i = ndof - 1; i >= 0; i--) {
    double sum = x[i];
    for (int j = i + 1; j < ndof; j++) {
      sum -= kglb[j][i] * u[j];
    }
    u[i] = sum;
  }
}

// Решение системы уравнений LDLT
bool solveLinearSystemLDLT3d(double **kglb, double *u, double *r, double *x, int ndof) {
  if (!matrLDLT3d(ndof, kglb)) {
    return false;
  }
  
  direktLDLT3d(ndof, kglb, x, r);
  
  if (!diagLDLT3d(ndof, kglb, x)) {
    return false;
  }
  
  rechLDLT3d(ndof, kglb, u, x);
  return true;
}

// Расчет напряжений в 3D модели
void stressModel3d(int nelem, int **jt03, double **car, double e,
                   double puas, double *u, double **strain, double **stress) {
  
  for (int elem = 0; elem < nelem; elem++) {
    // Получаем координаты узлов элемента
    int node1 = jt03[elem][0] - 1;
    int node2 = jt03[elem][1] - 1;
    int node3 = jt03[elem][2] - 1;
    int node4 = jt03[elem][3] - 1;
    
    coord3d coord1 = {car[0][node1], car[1][node1], car[2][node1]};
    coord3d coord2 = {car[0][node2], car[1][node2], car[2][node2]};
    coord3d coord3 = {car[0][node3], car[1][node3], car[2][node3]};
    coord3d coord4 = {car[0][node4], car[1][node4], car[2][node4]};
    
    // Формируем матрицу деформаций
    double **deformMtrx = (double **)calloc(6, sizeof(double *));
    for (int i = 0; i < 6; i++) {
      deformMtrx[i] = (double *)calloc(12, sizeof(double));
    }
    
    formationDeformMtrx3d(deformMtrx, coord1, coord2, coord3, coord4);
    
    // Формируем матрицу упругости
    double **elastMtrx = (double **)calloc(6, sizeof(double *));
    for (int i = 0; i < 6; i++) {
      elastMtrx[i] = (double *)calloc(6, sizeof(double));
    }
    
    formationElastMtrx3d(elastMtrx, e, puas);
    
    // Вычисляем деформации
    for (int i = 0; i < 6; i++) {
      strain[elem][i] = 0.0;
      for (int j = 0; j < 12; j++) {
        strain[elem][i] += deformMtrx[i][j] * u[j];
      }
    }
    
    // Вычисляем напряжения
    for (int i = 0; i < 6; i++) {
      stress[elem][i] = 0.0;
      for (int j = 0; j < 6; j++) {
        stress[elem][i] += elastMtrx[i][j] * strain[elem][j];
      }
    }
    
    // Освобождаем память
    for (int i = 0; i < 6; i++) {
      free(deformMtrx[i]);
      free(elastMtrx[i]);
    }
    free(deformMtrx);
    free(elastMtrx);
  }
}

// Сборка локальной матрицы жесткости в глобальную
void AssembleLocalStiffnessToGlobal3d(double **kglb, int **jt03, double **car,
                                      int nelem, double e, double puas) {
  
  printf("Starting assembly for %d elements...\n", nelem);
  
  for (int elem = 0; elem < nelem; elem++) {
    printf("Processing element %d...\n", elem);
    
    // Получаем координаты узлов элемента
    int node1 = jt03[elem][0] - 1;
    int node2 = jt03[elem][1] - 1;
    int node3 = jt03[elem][2] - 1;
    int node4 = jt03[elem][3] - 1;
    
    printf("Element %d nodes: %d, %d, %d, %d\n", elem, node1+1, node2+1, node3+1, node4+1);
    
    // Проверяем границы массивов
    if (node1 < 0 || node2 < 0 || node3 < 0 || node4 < 0) {
      printf("Error: Invalid node indices in element %d\n", elem);
      continue;
    }
    
    coord3d coord1 = {car[0][node1], car[1][node1], car[2][node1]};
    coord3d coord2 = {car[0][node2], car[1][node2], car[2][node2]};
    coord3d coord3 = {car[0][node3], car[1][node3], car[2][node3]};
    coord3d coord4 = {car[0][node4], car[1][node4], car[2][node4]};
    
    printf("Element %d coordinates: (%.2f,%.2f,%.2f), (%.2f,%.2f,%.2f), (%.2f,%.2f,%.2f), (%.2f,%.2f,%.2f)\n",
           elem, coord1.x, coord1.y, coord1.z, coord2.x, coord2.y, coord2.z,
           coord3.x, coord3.y, coord3.z, coord4.x, coord4.y, coord4.z);
    
    // Вычисляем матрицу жесткости элемента
    double **gest = (double **)calloc(12, sizeof(double *));
    if (gest == NULL) {
      printf("Error: Cannot allocate local stiffness matrix for element %d\n", elem);
      continue;
    }
    
    for (int i = 0; i < 12; i++) {
      gest[i] = (double *)calloc(12, sizeof(double));
      if (gest[i] == NULL) {
        printf("Error: Cannot allocate row %d of local stiffness matrix for element %d\n", i, elem);
        // Освобождаем уже выделенную память
        for (int j = 0; j < i; j++) {
          free(gest[j]);
        }
        free(gest);
        continue;
      }
    }
    
    printf("Calculating local stiffness matrix for element %d...\n", elem);
    tetrahedronElement(coord1, coord2, coord3, coord4, e, puas, gest);
    
    // Собираем в глобальную матрицу
    nodeNumber3d node = {jt03[elem][0], jt03[elem][1], jt03[elem][2], jt03[elem][3]};
    printf("Assembling element %d into global matrix...\n", elem);
    assemblyGlobMatr3d(node, gest, kglb);
    
    // Освобождаем память
    for (int i = 0; i < 12; i++) {
      free(gest[i]);
    }
    free(gest);
    
    printf("Element %d completed.\n", elem);
  }
  
  printf("Assembly completed.\n");
} 