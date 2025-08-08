#ifndef DEFINES_H
#define DEFINES_H

#include <math.h>

#define GL_SILENCE_DEPRECATION

#include "../dependencies/glew/2.2.01/include/GL/glew.h"
#include "../dependencies/glfw/3.4/include/GLFW/glfw3.h"
#include "../dependencies/glut/include/glut.h"
#include "fem3d.h"
#include "formation_mtrx3d.h"

#define LOAD 25000.  // Приложенная нагрузка
#define FLT_MAX 100.
#define KOEF_X 25.
#define KOEF_Y 14.
#define KOEF_Z 10.
#define WINDTH_WINDOW 1200          // Ширина окна
#define HEIGHT_WINDOW 800           // Высота окна
#define STRESS_SCALE 1000.0         // Масштаб для визуализации напряжений
#define DEFORMATION_SCALE 10.0      // Масштаб для визуализации деформаций
#define GLUT_WINDOW_POSITION_X 100  // Позиция окна (по оси x)
#define GLUT_WINDOW_POSITION_Y 100  // Позиция окна (по оси y)

extern int nelem;        // кол-во тетраэдров
extern int nys;          // число узлов К.Э модели
extern double** car;     // массив координат узлов элемента
extern int** jt03;       // массив номеров узлов элемента
extern double* u;        // массив перемещений узлов
extern double** stress;  // массив напряжений

#endif