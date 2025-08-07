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
  char error = EXIT_SUCCESS;

  double* data_stress = NULL;
  double* data_strain = NULL;
  double* dataCar = NULL;
  int* data_jt03 = NULL;
  double* dataKGLB = NULL;
  double** kglb = NULL;
  double* r = NULL;
  double* x = NULL;
  double** strain = NULL;

  error += read_from_file("../nodes/beam3d.txt", &nys, &dataCar, &car,
                                 &nelem, &data_jt03, &jt03);

  if (error) {
    perror("Error: Failed to read file\n");
    return error;
  }

  int ndofysla = 3;           // кол-во степеней свободы одного узла
  int ndof = nys * ndofysla;  // общее кол-во степеней свободы

  error += mem_for_arrays(ndof, 4, &u, &r, &x, &dataKGLB);

  error += makeDoubleMtrx(dataKGLB, &kglb, ndof, ndof);
  if (error) {
    free_memory(4, u, r, x, dataKGLB);
    return error;
  }

  error += makeDoubleArr(&data_stress, nelem * 6);
  if (error) {
    free_memory(4, u, r, x, dataKGLB);
    perror("CAN'T MEMORY ALLOCATE");
    return EXIT_FAILURE;
  }
  error += makeDoubleArr(&data_strain, nelem * 6);
  if (error) {
    free_memory(5, u, r, x, dataKGLB, data_stress);
    perror("CAN'T MEMORY ALLOCATE");
    return EXIT_FAILURE;
  }
  error += makeDoubleMtrx(data_stress, &stress, nelem, 6);
  if (error) {
    free_memory(6, u, r, x, dataKGLB, data_stress, data_strain);
    perror("CAN'T MEMORY ALLOCATE");
    return EXIT_FAILURE;
  }
  error += makeDoubleMtrx(data_strain, &strain, nelem, 6);
  if (error) {
    free_memory(7, u, r, x, dataKGLB, data_stress, data_strain, stress);
    perror("CAN'T MEMORY ALLOCATE");
    return EXIT_FAILURE;
  }
  stress = (double**)calloc(nelem, sizeof(double*));
  if (stress == NULL) {
    perror("Error: Failed to allocate memory for stress\n");
    free_memory(5, dataKGLB, kglb, u, r, x);
    return EXIT_FAILURE;
  }

  drawMashForSolve3d(argc, argv);

  return EXIT_SUCCESS;
}