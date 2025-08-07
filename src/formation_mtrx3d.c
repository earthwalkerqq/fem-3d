#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "formation_mtrx3d.h"

// Формирование матрицы деформаций для тетраэдра
double **formationDeformMtrx3d(double **deformMtrx, coord3d coord1, coord3d coord2,
                                coord3d coord3, coord3d coord4) {
  
  // Вычисляем объем тетраэдра
  double detJ = (coord2.x - coord1.x) * ((coord3.y - coord1.y) * (coord4.z - coord1.z) - 
                                          (coord3.z - coord1.z) * (coord4.y - coord1.y)) -
                 (coord2.y - coord1.y) * ((coord3.x - coord1.x) * (coord4.z - coord1.z) - 
                                          (coord3.z - coord1.z) * (coord4.x - coord1.x)) +
                 (coord2.z - coord1.z) * ((coord3.x - coord1.x) * (coord4.y - coord1.y) - 
                                          (coord3.y - coord1.y) * (coord4.x - coord1.x));
  
  double V = detJ / 6.0;
  
  // Вычисляем производные функций формы
  double b1 = ((coord3.y - coord1.y) * (coord4.z - coord1.z) - 
                (coord3.z - coord1.z) * (coord4.y - coord1.y)) / (6.0 * V);
  double b2 = ((coord1.y - coord2.y) * (coord4.z - coord1.z) - 
                (coord1.z - coord2.z) * (coord4.y - coord1.y)) / (6.0 * V);
  double b3 = ((coord2.y - coord1.y) * (coord3.z - coord1.z) - 
                (coord2.z - coord1.z) * (coord3.y - coord1.y)) / (6.0 * V);
  double b4 = ((coord1.y - coord3.y) * (coord2.z - coord1.z) - 
                (coord1.z - coord3.z) * (coord2.y - coord1.y)) / (6.0 * V);
  
  double c1 = ((coord3.z - coord1.z) * (coord4.x - coord1.x) - 
                (coord3.x - coord1.x) * (coord4.z - coord1.z)) / (6.0 * V);
  double c2 = ((coord1.z - coord2.z) * (coord4.x - coord1.x) - 
                (coord1.x - coord2.x) * (coord4.z - coord1.z)) / (6.0 * V);
  double c3 = ((coord2.z - coord1.z) * (coord3.x - coord1.x) - 
                (coord2.x - coord1.x) * (coord3.z - coord1.z)) / (6.0 * V);
  double c4 = ((coord1.z - coord3.z) * (coord2.x - coord1.x) - 
                (coord1.x - coord3.x) * (coord2.z - coord1.z)) / (6.0 * V);
  
  double d1 = ((coord3.x - coord1.x) * (coord4.y - coord1.y) - 
                (coord3.y - coord1.y) * (coord4.x - coord1.x)) / (6.0 * V);
  double d2 = ((coord1.x - coord2.x) * (coord4.y - coord1.y) - 
                (coord1.y - coord2.y) * (coord4.x - coord1.x)) / (6.0 * V);
  double d3 = ((coord2.x - coord1.x) * (coord3.y - coord1.y) - 
                (coord2.y - coord1.y) * (coord3.x - coord1.x)) / (6.0 * V);
  double d4 = ((coord1.x - coord3.x) * (coord2.y - coord1.y) - 
                (coord1.y - coord3.y) * (coord2.x - coord1.x)) / (6.0 * V);
  
  // Заполняем матрицу деформаций
  // Для каждого узла: [dN/dx, 0, 0, dN/dy, 0, dN/dz]
  //                    [0, dN/dy, 0, dN/dx, dN/dz, 0]
  //                    [0, 0, dN/dz, 0, dN/dy, dN/dx]
  
  // Узел 1
  deformMtrx[0][0] = b1; deformMtrx[0][1] = 0; deformMtrx[0][2] = 0;
  deformMtrx[1][0] = 0; deformMtrx[1][1] = c1; deformMtrx[1][2] = 0;
  deformMtrx[2][0] = 0; deformMtrx[2][1] = 0; deformMtrx[2][2] = d1;
  deformMtrx[3][0] = c1; deformMtrx[3][1] = b1; deformMtrx[3][2] = 0;
  deformMtrx[4][0] = d1; deformMtrx[4][1] = 0; deformMtrx[4][2] = b1;
  deformMtrx[5][0] = 0; deformMtrx[5][1] = d1; deformMtrx[5][2] = c1;
  
  // Узел 2
  deformMtrx[0][3] = b2; deformMtrx[0][4] = 0; deformMtrx[0][5] = 0;
  deformMtrx[1][3] = 0; deformMtrx[1][4] = c2; deformMtrx[1][5] = 0;
  deformMtrx[2][3] = 0; deformMtrx[2][4] = 0; deformMtrx[2][5] = d2;
  deformMtrx[3][3] = c2; deformMtrx[3][4] = b2; deformMtrx[3][5] = 0;
  deformMtrx[4][3] = d2; deformMtrx[4][4] = 0; deformMtrx[4][5] = b2;
  deformMtrx[5][3] = 0; deformMtrx[5][4] = d2; deformMtrx[5][5] = c2;
  
  // Узел 3
  deformMtrx[0][6] = b3; deformMtrx[0][7] = 0; deformMtrx[0][8] = 0;
  deformMtrx[1][6] = 0; deformMtrx[1][7] = c3; deformMtrx[1][8] = 0;
  deformMtrx[2][6] = 0; deformMtrx[2][7] = 0; deformMtrx[2][8] = d3;
  deformMtrx[3][6] = c3; deformMtrx[3][7] = b3; deformMtrx[3][8] = 0;
  deformMtrx[4][6] = d3; deformMtrx[4][7] = 0; deformMtrx[4][8] = b3;
  deformMtrx[5][6] = 0; deformMtrx[5][7] = d3; deformMtrx[5][8] = c3;
  
  // Узел 4
  deformMtrx[0][9] = b4; deformMtrx[0][10] = 0; deformMtrx[0][11] = 0;
  deformMtrx[1][9] = 0; deformMtrx[1][10] = c4; deformMtrx[1][11] = 0;
  deformMtrx[2][9] = 0; deformMtrx[2][10] = 0; deformMtrx[2][11] = d4;
  deformMtrx[3][9] = c4; deformMtrx[3][10] = b4; deformMtrx[3][11] = 0;
  deformMtrx[4][9] = d4; deformMtrx[4][10] = 0; deformMtrx[4][11] = b4;
  deformMtrx[5][9] = 0; deformMtrx[5][10] = d4; deformMtrx[5][11] = c4;
  
  return deformMtrx;
}

// Формирование матрицы упругости для 3D
double **formationElastMtrx3d(double **elastMtrx, double e, double puas) {
  double factor = e / ((1.0 + puas) * (1.0 - 2.0 * puas));
  
  // Заполняем матрицу упругости для изотропного материала
  elastMtrx[0][0] = factor * (1.0 - puas);
  elastMtrx[0][1] = factor * puas;
  elastMtrx[0][2] = factor * puas;
  elastMtrx[0][3] = 0.0;
  elastMtrx[0][4] = 0.0;
  elastMtrx[0][5] = 0.0;
  
  elastMtrx[1][0] = factor * puas;
  elastMtrx[1][1] = factor * (1.0 - puas);
  elastMtrx[1][2] = factor * puas;
  elastMtrx[1][3] = 0.0;
  elastMtrx[1][4] = 0.0;
  elastMtrx[1][5] = 0.0;
  
  elastMtrx[2][0] = factor * puas;
  elastMtrx[2][1] = factor * puas;
  elastMtrx[2][2] = factor * (1.0 - puas);
  elastMtrx[2][3] = 0.0;
  elastMtrx[2][4] = 0.0;
  elastMtrx[2][5] = 0.0;
  
  elastMtrx[3][0] = 0.0;
  elastMtrx[3][1] = 0.0;
  elastMtrx[3][2] = 0.0;
  elastMtrx[3][3] = factor * (1.0 - 2.0 * puas) / 2.0;
  elastMtrx[3][4] = 0.0;
  elastMtrx[3][5] = 0.0;
  
  elastMtrx[4][0] = 0.0;
  elastMtrx[4][1] = 0.0;
  elastMtrx[4][2] = 0.0;
  elastMtrx[4][3] = 0.0;
  elastMtrx[4][4] = factor * (1.0 - 2.0 * puas) / 2.0;
  elastMtrx[4][5] = 0.0;
  
  elastMtrx[5][0] = 0.0;
  elastMtrx[5][1] = 0.0;
  elastMtrx[5][2] = 0.0;
  elastMtrx[5][3] = 0.0;
  elastMtrx[5][4] = 0.0;
  elastMtrx[5][5] = factor * (1.0 - 2.0 * puas) / 2.0;
  
  return elastMtrx;
}

// Расчет матрицы жесткости тетраэдра
void tetrahedronElement(coord3d coord1, coord3d coord2, coord3d coord3, coord3d coord4,
                        double e, double puas, double **gest) {
  
  printf("  Entering tetrahedronElement...\n");
  
  // Формируем матрицу деформаций
  double **deformMtrx = (double **)calloc(6, sizeof(double *));
  if (deformMtrx == NULL) {
    printf("  Error: Cannot allocate deformation matrix\n");
    return;
  }
  
  for (int i = 0; i < 6; i++) {
    deformMtrx[i] = (double *)calloc(12, sizeof(double));
    if (deformMtrx[i] == NULL) {
      printf("  Error: Cannot allocate deformation matrix row %d\n", i);
      // Освобождаем уже выделенную память
      for (int j = 0; j < i; j++) {
        free(deformMtrx[j]);
      }
      free(deformMtrx);
      return;
    }
  }
  
  printf("  Forming deformation matrix...\n");
  formationDeformMtrx3d(deformMtrx, coord1, coord2, coord3, coord4);
  
  // Формируем матрицу упругости
  double **elastMtrx = (double **)calloc(6, sizeof(double *));
  if (elastMtrx == NULL) {
    printf("  Error: Cannot allocate elasticity matrix\n");
    // Освобождаем память
    for (int i = 0; i < 6; i++) {
      free(deformMtrx[i]);
    }
    free(deformMtrx);
    return;
  }
  
  for (int i = 0; i < 6; i++) {
    elastMtrx[i] = (double *)calloc(6, sizeof(double));
    if (elastMtrx[i] == NULL) {
      printf("  Error: Cannot allocate elasticity matrix row %d\n", i);
      // Освобождаем память
      for (int j = 0; j < 6; j++) {
        free(elastMtrx[j]);
      }
      free(elastMtrx);
      for (int j = 0; j < 6; j++) {
        free(deformMtrx[j]);
      }
      free(deformMtrx);
      return;
    }
  }
  
  printf("  Forming elasticity matrix...\n");
  formationElastMtrx3d(elastMtrx, e, puas);
  
  // Вычисляем объем тетраэдра
  double detJ = (coord2.x - coord1.x) * ((coord3.y - coord1.y) * (coord4.z - coord1.z) - 
                                          (coord3.z - coord1.z) * (coord4.y - coord1.y)) -
                 (coord2.y - coord1.y) * ((coord3.x - coord1.x) * (coord4.z - coord1.z) - 
                                          (coord3.z - coord1.z) * (coord4.x - coord1.x)) +
                 (coord2.z - coord1.z) * ((coord3.x - coord1.x) * (coord4.y - coord1.y) - 
                                          (coord3.y - coord1.y) * (coord4.x - coord1.x));
  
  double V = detJ / 6.0;
  printf("  Tetrahedron volume: %.6f\n", V);
  
  if (fabs(V) < 1e-12) {
    printf("  Warning: Very small or negative volume detected\n");
  }
  
  // Вычисляем матрицу жесткости: K = B^T * D * B * V
  double **temp1 = (double **)calloc(6, sizeof(double *));
  if (temp1 == NULL) {
    printf("  Error: Cannot allocate temporary matrix\n");
    // Освобождаем память
    for (int i = 0; i < 6; i++) {
      free(elastMtrx[i]);
      free(deformMtrx[i]);
    }
    free(elastMtrx);
    free(deformMtrx);
    return;
  }
  
  for (int i = 0; i < 6; i++) {
    temp1[i] = (double *)calloc(12, sizeof(double));
    if (temp1[i] == NULL) {
      printf("  Error: Cannot allocate temporary matrix row %d\n", i);
      // Освобождаем память
      for (int j = 0; j < 6; j++) {
        free(temp1[j]);
      }
      free(temp1);
      for (int j = 0; j < 6; j++) {
        free(elastMtrx[j]);
        free(deformMtrx[j]);
      }
      free(elastMtrx);
      free(deformMtrx);
      return;
    }
  }
  
  printf("  Computing D * B...\n");
  // temp1 = D * B
  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 12; j++) {
      temp1[i][j] = 0.0;
      for (int k = 0; k < 6; k++) {
        temp1[i][j] += elastMtrx[i][k] * deformMtrx[k][j];
      }
    }
  }
  
  printf("  Computing B^T * temp1 * V...\n");
  // gest = B^T * temp1 * V
  for (int i = 0; i < 12; i++) {
    for (int j = 0; j < 12; j++) {
      gest[i][j] = 0.0;
      for (int k = 0; k < 6; k++) {
        gest[i][j] += deformMtrx[k][i] * temp1[k][j] * V;
      }
    }
  }
  
  printf("  Cleaning up memory...\n");
  // Освобождаем память
  for (int i = 0; i < 6; i++) {
    free(deformMtrx[i]);
    free(elastMtrx[i]);
    free(temp1[i]);
  }
  free(deformMtrx);
  free(elastMtrx);
  free(temp1);
  
  printf("  tetrahedronElement completed.\n");
}

// Сборка локальной матрицы в глобальную
void assemblyGlobMatr3d(nodeNumber3d node, double **gest, double **kglb) {
  int dofs[12];
  
  // Определяем степени свободы для каждого узла
  for (int i = 0; i < 4; i++) {
    int nodeIndex = 0;
    switch (i) {
      case 0: nodeIndex = node.iys1; break;
      case 1: nodeIndex = node.iys2; break;
      case 2: nodeIndex = node.iys3; break;
      case 3: nodeIndex = node.iys4; break;
    }
    dofs[i * 3] = (nodeIndex - 1) * 3;     // x
    dofs[i * 3 + 1] = (nodeIndex - 1) * 3 + 1; // y
    dofs[i * 3 + 2] = (nodeIndex - 1) * 3 + 2; // z
  }
  
  // Собираем в глобальную матрицу
  for (int i = 0; i < 12; i++) {
    for (int j = 0; j < 12; j++) {
      kglb[dofs[i]][dofs[j]] += gest[i][j];
    }
  }
}

// Заполнение закрепленных и нагруженных узлов
void FillConstrainedLoadedNodes3d(int **nodePres, int *lenNodePres,
                                   int **nodeZakrU, int *lenNodeZakrU,
                                   int **nodeZakrV, int *lenNodeZakrV,
                                   int **nodeZakrW, int *lenNodeZakrW,
                                   double **car, int nys) {
  *lenNodePres = 0;
  *lenNodeZakrU = 0;
  *lenNodeZakrV = 0;
  *lenNodeZakrW = 0;
  
  for (int i = 0; i < nys; i++) {
    // Нагруженные узлы (например, верхняя грань)
    if (car[1][i] > 0.9) {  // y > 0.9
      (*lenNodePres)++;
      int *temp = (int *)realloc(*nodePres, (*lenNodePres) * sizeof(int));
      if (temp == NULL) {
        if (*nodePres) free(*nodePres);
        if (*nodeZakrU) free(*nodeZakrU);
        if (*nodeZakrV) free(*nodeZakrV);
        if (*nodeZakrW) free(*nodeZakrW);
        exit(1);
      }
      *nodePres = temp;
      (*nodePres)[*lenNodePres - 1] = i;
    }
    
    // Закрепленные узлы (например, нижняя грань)
    if (car[1][i] < 0.1) {  // y < 0.1
      (*lenNodeZakrU)++;
      int *temp = (int *)realloc(*nodeZakrU, (*lenNodeZakrU) * sizeof(int));
      if (temp == NULL) {
        if (*nodePres) free(*nodePres);
        if (*nodeZakrU) free(*nodeZakrU);
        if (*nodeZakrV) free(*nodeZakrV);
        if (*nodeZakrW) free(*nodeZakrW);
        exit(1);
      }
      *nodeZakrU = temp;
      (*nodeZakrU)[*lenNodeZakrU - 1] = i;
      
      (*lenNodeZakrV)++;
      temp = (int *)realloc(*nodeZakrV, (*lenNodeZakrV) * sizeof(int));
      if (temp == NULL) {
        if (*nodePres) free(*nodePres);
        if (*nodeZakrU) free(*nodeZakrU);
        if (*nodeZakrV) free(*nodeZakrV);
        if (*nodeZakrW) free(*nodeZakrW);
        exit(1);
      }
      *nodeZakrV = temp;
      (*nodeZakrV)[*lenNodeZakrV - 1] = i;
      
      (*lenNodeZakrW)++;
      temp = (int *)realloc(*nodeZakrW, (*lenNodeZakrW) * sizeof(int));
      if (temp == NULL) {
        if (*nodePres) free(*nodePres);
        if (*nodeZakrU) free(*nodeZakrU);
        if (*nodeZakrV) free(*nodeZakrV);
        if (*nodeZakrW) free(*nodeZakrW);
        exit(1);
      }
      *nodeZakrW = temp;
      (*nodeZakrW)[*lenNodeZakrW - 1] = i;
    }
  }
}

// Применение закреплений
void MakeConstrained3d(int *nodeZakr, int lenNodeZakr, double **kglb, int ndofysla) {
  printf("  Applying constraints for %d nodes...\n", lenNodeZakr);
  
  if (nodeZakr == NULL || kglb == NULL) {
    printf("  Error: NULL pointers in MakeConstrained3d\n");
    return;
  }
  
  for (int i = 0; i < lenNodeZakr; i++) {
    int nodeIndex = nodeZakr[i];
    int dofIndex = nodeIndex * ndofysla;
    
    printf("  Constraining node %d (DOF index: %d)\n", nodeIndex, dofIndex);
    
    // Проверяем границы массива
    if (dofIndex < 0 || dofIndex >= 1000) {
      printf("  Warning: DOF index %d out of bounds, skipping\n", dofIndex);
      continue;
    }
    
    // Проверяем, что указатели не NULL
    if (kglb[dofIndex] == NULL) {
      printf("  Warning: kglb[%d] is NULL, skipping\n", dofIndex);
      continue;
    }
    
    // Обнуляем строку и столбец для закрепленной степени свободы
    // Используем более безопасный подход
    for (int j = 0; j < 1000; j++) {  // Предполагаем максимальный размер
      if (dofIndex < 1000 && j < 1000 && kglb[j] != NULL) {
        kglb[dofIndex][j] = 0.0;
        kglb[j][dofIndex] = 0.0;
      }
    }
    if (dofIndex < 1000) {
      kglb[dofIndex][dofIndex] = 1.0;  // Единица на диагонали
    }
  }
  
  printf("  Constraints applied successfully\n");
}

// Задание вектора нагрузок
void SetLoadVector3d(double *r, int lenNodePres, int *nodePres, int ndofysla,
                     int ndof, float load) {
  for (int i = 0; i < ndof; i++) {
    r[i] = 0.0;
  }
  
  for (int i = 0; i < lenNodePres; i++) {
    int nodeIndex = nodePres[i];
    int dofIndex = nodeIndex * ndofysla + 1;  // Нагрузка по Y
    if (dofIndex < ndof) {
      r[dofIndex] = load;
    }
  }
}

// Создание матриц
void makeDoubleMtrx3d(double *dataMtrx, double ***mtrx, int row, int col) {
  *mtrx = (double **)calloc(row, sizeof(double *));
  for (int i = 0; i < row; i++) {
    (*mtrx)[i] = dataMtrx + i * col;
  }
}

void makeIntegerMtrx3d(int *dataMtrx, int ***mtrx, int row, int col) {
  *mtrx = (int **)calloc(row, sizeof(int *));
  for (int i = 0; i < row; i++) {
    (*mtrx)[i] = dataMtrx + i * col;
  }
}

// Чтение файла с данными
short readFromFile3d(char *filename, int *nys, double **dataCar, double ***car,
                     int *nelem, int **data_jt03, int ***jt03) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    perror("Error: Cannot open file\n");
    return EXIT_FAILURE;
  }
  
  if (fscanf(file, "%d", nys) != 1) {
    perror("Error: Cannot read number of nodes\n");
    fclose(file);
    return EXIT_FAILURE;  // Ошибка чтения количества узлов
  }
  
  *dataCar = (double *)calloc(*nys * 3, sizeof(double));
  if (*dataCar == NULL) {
    perror("Error: Cannot allocate memory for node coordinates\n");
    fclose(file);
    return EXIT_FAILURE;
  }
  
  // Читаем координаты узлов
  for (int i = 0; i < *nys; i++) {
    if (fscanf(file, "%lf %lf %lf", 
               &(*dataCar)[i * 3], 
               &(*dataCar)[i * 3 + 1], 
               &(*dataCar)[i * 3 + 2]) != 3) {
      printf("Error: Cannot read coordinates for node %d\n", i + 1);
      free(*dataCar);
      fclose(file);
      return 2;
    }
    printf("Node %d: (%.2f, %.2f, %.2f)\n", i + 1, 
           (*dataCar)[i * 3], (*dataCar)[i * 3 + 1], (*dataCar)[i * 3 + 2]);
  }
  
  // Читаем количество элементов
  if (fscanf(file, "%d", nelem) != 1) {
    printf("Error: Cannot read number of elements\n");
    free(*dataCar);
    fclose(file);
    return 2;
  }
  printf("Number of elements: %d\n", *nelem);
  
  // Выделяем память для номеров узлов элементов
  *data_jt03 = (int *)calloc(*nelem * 4, sizeof(int));
  if (*data_jt03 == NULL) {
    printf("Error: Cannot allocate memory for element nodes\n");
    free(*dataCar);
    fclose(file);
    return 3;
  }
  
  // Читаем номера узлов элементов
  for (int i = 0; i < *nelem; i++) {
    if (fscanf(file, "%d %d %d %d", 
               &(*data_jt03)[i * 4], 
               &(*data_jt03)[i * 4 + 1], 
               &(*data_jt03)[i * 4 + 2], 
               &(*data_jt03)[i * 4 + 3]) != 4) {
      printf("Error: Cannot read element %d\n", i + 1);
      free(*dataCar);
      free(*data_jt03);
      fclose(file);
      return 2;
    }
    printf("Element %d: nodes %d, %d, %d, %d\n", i + 1,
           (*data_jt03)[i * 4], (*data_jt03)[i * 4 + 1], 
           (*data_jt03)[i * 4 + 2], (*data_jt03)[i * 4 + 3]);
    
    // Проверяем корректность индексов
    for (int j = 0; j < 4; j++) {
      int nodeIndex = (*data_jt03)[i * 4 + j];
      if (nodeIndex < 1 || nodeIndex > *nys) {
        printf("Error: Invalid node index %d in element %d\n", nodeIndex, i + 1);
        free(*dataCar);
        free(*data_jt03);
        fclose(file);
        return 2;
      }
    }
  }
  
  // Создаем матрицы
  makeDoubleMtrx3d(*dataCar, car, *nys, 3);
  makeIntegerMtrx3d(*data_jt03, jt03, *nelem, 4);
  
  fclose(file);
  printf("File read successfully\n");
  return 0;  // Успешно
}

// Запись результатов
void writeResult3d(char *filename, double **strain, double **stress,
                   double *u, int nelem, int nys, int ndof) {
  FILE *file = fopen(filename, "w");
  if (!file) return;
  
  fprintf(file, "3D FEM Analysis Results\n");
  fprintf(file, "========================\n\n");
  
  fprintf(file, "Number of nodes: %d\n", nys);
  fprintf(file, "Number of elements: %d\n", nelem);
  fprintf(file, "Total degrees of freedom: %d\n\n", ndof);
  
  fprintf(file, "Displacements:\n");
  for (int i = 0; i < nys; i++) {
    fprintf(file, "Node %d: ux=%.6e, uy=%.6e, uz=%.6e\n", 
            i + 1, u[i * 3], u[i * 3 + 1], u[i * 3 + 2]);
  }
  
  fprintf(file, "\nStrains:\n");
  for (int i = 0; i < nelem; i++) {
    fprintf(file, "Element %d: exx=%.6e, eyy=%.6e, ezz=%.6e, exy=%.6e, exz=%.6e, eyz=%.6e\n",
            i + 1, strain[i][0], strain[i][1], strain[i][2], 
            strain[i][3], strain[i][4], strain[i][5]);
  }
  
  fprintf(file, "\nStresses:\n");
  for (int i = 0; i < nelem; i++) {
    fprintf(file, "Element %d: sxx=%.6e, syy=%.6e, szz=%.6e, sxy=%.6e, sxz=%.6e, syz=%.6e\n",
            i + 1, stress[i][0], stress[i][1], stress[i][2], 
            stress[i][3], stress[i][4], stress[i][5]);
  }
  
  fclose(file);
} 