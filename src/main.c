#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "defines.h"
#include "draw.h"

int nelem = 0;
int nys = 0;
double** car = NULL;
int** jt03 = NULL;
double* u = NULL;
double** stress = NULL;

int main(int argc, char** argv) {
  double* dataCar = NULL;
  int* data_jt03 = NULL;
  double* dataKGLB = NULL;
  double** kglb = NULL;
  double* r = NULL;
  double* x = NULL;
  double** strain = NULL;

  short fileErr = readFromFile3d("../nodes/beam3d.txt", &nys, &dataCar, &car,
                                 &nelem, &data_jt03, &jt03);

  if (fileErr != 0) {
    perror("Error: Failed to read file\n");
    return EXIT_FAILURE;
  } else if (nys <= 0 || nelem <= 0) {
    perror("Error: Invalid data\n");
    return EXIT_FAILURE;
  } else if (car == NULL || jt03 == NULL) {
    perror("Error: NULL pointers after file read\n");
    return EXIT_FAILURE;
  }

  int ndofysla = 3;           // кол-во степеней свободы одного узла (x, y, z)
  int ndof = nys * ndofysla;  // общее кол-во степеней свободы

  u = (double*)calloc(ndof, sizeof(double));
  if (u == NULL) {
    perror("Error: Failed to allocate memory for u\n");
    return EXIT_FAILURE;
  }

  r = (double*)calloc(ndof, sizeof(double));
  if (r == NULL) {
    perror("Error: Failed to allocate memory for r\n");
    free(u);
    return EXIT_FAILURE;
  }

  x = (double*)calloc(ndof, sizeof(double));
  if (x == NULL) {
    perror("Error: Failed to allocate memory for x\n");
    free_memory(2, u, r);
    return EXIT_FAILURE;
  }

  dataKGLB = (double*)calloc(ndof * ndof, sizeof(double));
  if (dataKGLB == NULL) {
    perror("Error: Failed to allocate memory for dataKGLB\n");
    free_memory(3, u, r, x);
    return EXIT_FAILURE;
  }

  kglb = (double**)calloc(ndof, sizeof(double*));
  if (kglb == NULL) {
    perror("Error: Failed to allocate memory for kglb\n");
    free_memory(4, dataKGLB, u, r, x);
    return EXIT_FAILURE;
  }

  for (int i = 0; i < ndof; i++) {
    kglb[i] = dataKGLB + i * ndof;
  }

  stress = (double**)calloc(nelem, sizeof(double*));
  if (stress == NULL) {
    perror("Error: Failed to allocate memory for stress\n");
    free_memory(5, dataKGLB, kglb, u, r, x);
    return EXIT_FAILURE;
  }

  strain = (double**)calloc(nelem, sizeof(double*));
  if (strain == NULL) {
    perror("Error: Failed to allocate memory for strain\n");
    free_memory(6, stress, dataKGLB, kglb, u, r, x);
    return EXIT_FAILURE;
  }

  for (int i = 0; i < nelem; i++) {
    stress[i] = (double*)calloc(6, sizeof(double));
    strain[i] = (double*)calloc(6, sizeof(double));
    if (stress[i] == NULL || strain[i] == NULL) {
      perror("Error: Failed to allocate memory for stress/strain\n");
      return EXIT_FAILURE;
    }
  }

  drawMashForSolve3d(argc, argv);

  return EXIT_SUCCESS;
}