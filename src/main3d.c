#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

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

// Глобальные переменные для управления визуализацией
static int showDeformed = 1;           // Показывать деформированную модель
static int showStress = 1;             // Показывать напряжения
static int showValues = 0;             // Показывать числовые значения
static double zoom = 1.0;              // Масштаб отображения
static float animationProgress = 0.f;  // Шаг анимации
static int isAnimating = 0;            // Флаг анимации
static int animationDirection = 1;     // Направление анимации
static float rotationX = 0.0f;         // Поворот вокруг оси X
static float rotationY = 0.0f;         // Поворот вокруг оси Y
static float rotationZ = 0.0f;         // Поворот вокруг оси Z
static float deformationScale = 1.0f;  // Масштаб деформации
static int showLoad = 1;               // Показывать нагрузку
static float loadAnimation = 0.0f;     // Анимация нагрузки

void drawMashForSolve3d(int argc, char **argv);
void drawModel3d(void);
void display3d(void);
void init3d(void);
void reshape3d(int w, int h);
void updateAnimation3d(int __attribute__((unused)) value);
void keyboard3d(unsigned char key, int __attribute__((unused)) x,
                int __attribute__((unused)) y);
void renderText3d(float x, float y, float z, const char *text);
void drawTetrahedron(double **vertices, double *color);
void drawCube(double **vertices, double *color);

static int nelem;               // кол-во тетраэдров
static int nys;                 // число узлов К.Э модели
static double **car = NULL;     // массив координат узлов элемента
static int **jt03 = NULL;       // массив номеров узлов элемента
static double *u = NULL;        // массив перемещений узлов
static double **stress = NULL;  // массив напряжений

int main(int argc, char **argv) {
  printf("=== 3D FEM Analysis Starting ===\n");
  
  // Объявления переменных
  double *dataCar = NULL;
  int *data_jt03 = NULL;
  double *dataKGLB = NULL;
  double **kglb = NULL;
  double *r = NULL;
  double *x = NULL;
  double **strain = NULL;
  
  // Инициализация переменных
  nys = 0;
  nelem = 0;
  car = NULL;
  jt03 = NULL;
  stress = NULL;
  strain = NULL;
  
  // Чтение данных из файла
  printf("Reading file...\n");
  short fileErr = readFromFile3d("../nodes/beam3d.txt", &nys, &dataCar, &car,
                                 &nelem, &data_jt03, &jt03);
  
  if (fileErr != 0) {
    printf("Error: Failed to read file\n");
    return 1;
  }
  
  printf("File read successfully\n");
  printf("Nodes: %d, Elements: %d\n", nys, nelem);
  
  // Проверка данных
  if (nys <= 0 || nelem <= 0) {
    printf("Error: Invalid data - nys=%d, nelem=%d\n", nys, nelem);
    return 1;
  }
  
  if (car == NULL || jt03 == NULL) {
    printf("Error: NULL pointers after file read\n");
    return 1;
  }
  
  // Инициализация FEM расчетов
  int ndofysla = 3;  // кол-во степеней свободы одного узла (x, y, z)
  int ndof = nys * ndofysla;  // общее кол-во степеней свободы
  
  // Выделение памяти для результатов
  u = (double *)calloc(ndof, sizeof(double));
  if (u == NULL) {
    printf("Error: Failed to allocate memory for u\n");
    return 1;
  }
  
  r = (double *)calloc(ndof, sizeof(double));
  if (r == NULL) {
    printf("Error: Failed to allocate memory for r\n");
    free(u);
    return 1;
  }
  
  x = (double *)calloc(ndof, sizeof(double));
  if (x == NULL) {
    printf("Error: Failed to allocate memory for x\n");
    free(u);
    free(r);
    return 1;
  }
  
  // Выделение памяти для глобальной матрицы жесткости
  dataKGLB = (double *)calloc(ndof * ndof, sizeof(double));
  if (dataKGLB == NULL) {
    printf("Error: Failed to allocate memory for dataKGLB\n");
    free(u);
    free(r);
    free(x);
    return 1;
  }
  
  kglb = (double **)calloc(ndof, sizeof(double *));
  if (kglb == NULL) {
    printf("Error: Failed to allocate memory for kglb\n");
    free(dataKGLB);
    free(u);
    free(r);
    free(x);
    return 1;
  }
  
  for (int i = 0; i < ndof; i++) {
    kglb[i] = dataKGLB + i * ndof;
  }
  
  // Выделение памяти для напряжений и деформаций
  stress = (double **)calloc(nelem, sizeof(double *));
  if (stress == NULL) {
    printf("Error: Failed to allocate memory for stress\n");
    free(dataKGLB);
    free(kglb);
    free(u);
    free(r);
    free(x);
    return 1;
  }
  
  strain = (double **)calloc(nelem, sizeof(double *));
  if (strain == NULL) {
    printf("Error: Failed to allocate memory for strain\n");
    free(stress);
    free(dataKGLB);
    free(kglb);
    free(u);
    free(r);
    free(x);
    return 1;
  }
  
  for (int i = 0; i < nelem; i++) {
    stress[i] = (double *)calloc(6, sizeof(double));
    strain[i] = (double *)calloc(6, sizeof(double));
    if (stress[i] == NULL || strain[i] == NULL) {
      printf("Error: Failed to allocate memory for stress/strain[%d]\n", i);
      return 1;
    }
  }
  
  printf("Starting visualization...\n");
  drawMashForSolve3d(argc, argv);
  
  return 0;
}

void drawMashForSolve3d(int argc, char **argv) {
  printf("Initializing GLUT window...\n");
  
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(WINDTH_WINDOW, HEIGHT_WINDOW);
  glutInitWindowPosition(GLUT_WINDOW_POSITION_X, GLUT_WINDOW_POSITION_Y);
  glutCreateWindow("3D FEM Analysis");
  
  printf("Setting up OpenGL...\n");
  init3d();
  
  printf("Setting up callbacks...\n");
  glutDisplayFunc(display3d);
  glutKeyboardFunc(keyboard3d);
  glutReshapeFunc(reshape3d);
  glutTimerFunc(50, updateAnimation3d, 0);
  
  printf("Starting GLUT main loop...\n");
  glutMainLoop();
}

void init3d(void) {
  printf("Initializing OpenGL...\n");
  
  glClearColor(0.2, 0.2, 0.2, 0.0);  // Темно-серый фон
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_COLOR_MATERIAL);
  
  // Настройка освещения
  GLfloat light_position[] = {5.0, 5.0, 5.0, 1.0};
  GLfloat white_light[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat lmodel_ambient[] = {0.3, 0.3, 0.3, 1.0};
  
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
  
  printf("OpenGL initialization completed\n");
}

void display3d(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  
  // Настройка камеры для обзора балки
  gluLookAt(8.0, 4.0, 6.0,  // позиция камеры
            2.0, 0.25, 0.25,   // точка, на которую смотрит камера (центр балки)
            0.0, 1.0, 0.0);  // вектор "вверх"
  
  // Применение поворотов
  glRotatef(rotationX, 1.0, 0.0, 0.0);
  glRotatef(rotationY, 0.0, 1.0, 0.0);
  glRotatef(rotationZ, 0.0, 0.0, 1.0);
  
  // Масштабирование
  glScalef(zoom, zoom, zoom);
  
  drawModel3d();
  
  glutSwapBuffers();
}

void reshape3d(int w, int h) {
  printf("Reshape: %dx%d\n", w, h);
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0, (float)w / (float)h, 0.1, 100.0);
  glMatrixMode(GL_MODELVIEW);
}

void drawModel3d(void) {
  // Отрисовка осей координат для отладки
  glDisable(GL_LIGHTING);
  glBegin(GL_LINES);
  // Ось X - красная
  glColor3f(1.0, 0.0, 0.0);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(6.0, 0.0, 0.0);
  // Ось Y - зеленая
  glColor3f(0.0, 1.0, 0.0);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(0.0, 2.0, 0.0);
  // Ось Z - синяя
  glColor3f(0.0, 0.0, 1.0);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(0.0, 0.0, 2.0);
  glEnd();
  glEnable(GL_LIGHTING);
  
  // Отрисовка нагрузки (сила в конце балки)
  if (showLoad) {
    glDisable(GL_LIGHTING);
    glColor3f(1.0, 0.0, 0.0);  // Красный цвет для нагрузки
    
    float loadX = 4.0f;  // Позиция нагрузки (конец балки)
    float loadY = 0.25f;
    float loadZ = 0.25f;
    float loadSize = 0.3f * (0.5f + 0.5f * sin(loadAnimation));  // Пульсирующая нагрузка
    
    // Отрисовка стрелки нагрузки
    glBegin(GL_LINES);
    glVertex3f(loadX, loadY, loadZ);
    glVertex3f(loadX, loadY - loadSize, loadZ);
    glEnd();
    
    // Острие стрелки
    glBegin(GL_TRIANGLES);
    glVertex3f(loadX, loadY - loadSize, loadZ);
    glVertex3f(loadX - 0.1f, loadY - loadSize + 0.1f, loadZ);
    glVertex3f(loadX + 0.1f, loadY - loadSize + 0.1f, loadZ);
    glEnd();
    
    glEnable(GL_LIGHTING);
  }
  
  // Отрисовка тетраэдров из файла данных
  if (jt03 != NULL && car != NULL && nelem > 0) {
    int drawnElements = 0;
    
    for (int elem = 0; elem < nelem; elem++) {
      // Проверяем, что индексы узлов корректны
      if (jt03[elem][0] <= 0 || jt03[elem][1] <= 0 || 
          jt03[elem][2] <= 0 || jt03[elem][3] <= 0) {
        continue; // Пропускаем некорректные элементы
      }
      
      // Получаем координаты узлов тетраэдра
      int node1 = jt03[elem][0] - 1;
      int node2 = jt03[elem][1] - 1;
      int node3 = jt03[elem][2] - 1;
      int node4 = jt03[elem][3] - 1;
      
      // Проверяем границы массива
      if (node1 < 0 || node2 < 0 || node3 < 0 || node4 < 0 ||
          node1 >= nys || node2 >= nys || node3 >= nys || node4 >= nys) {
        continue; // Пропускаем элементы с некорректными индексами
      }
      
      double vertices[4][3] = {
        {car[node1][0], car[node1][1], car[node1][2]},
        {car[node2][0], car[node2][1], car[node2][2]},
        {car[node3][0], car[node3][1], car[node3][2]},
        {car[node4][0], car[node4][1], car[node4][2]}
      };
      
      // Применяем деформацию к вершинам
      if (showDeformed && isAnimating) {
        for (int i = 0; i < 4; i++) {
          // Простая модель деформации балки под нагрузкой
          float x = vertices[i][0];
          
          // Деформация зависит от расстояния от закрепленного конца
          float deflection = 0.0f;
          if (x > 0.0f) {
            // Максимальный прогиб в конце балки
            float maxDeflection = 0.5f * animationProgress * deformationScale;
            // Параболическая форма прогиба
            deflection = maxDeflection * (x * x) / 16.0f;  // 4^2 = 16
          }
          
          // Применяем прогиб в направлении Y
          vertices[i][1] += deflection;
          
          // Добавляем небольшое скручивание
          float twist = 0.1f * animationProgress * sin(x * M_PI / 4.0f);
          vertices[i][2] += twist;
        }
      }
      
      // Проверяем, что тетраэдр имеет ненулевой объем (используем абсолютное значение)
      double detJ = (vertices[1][0] - vertices[0][0]) * 
                    ((vertices[2][1] - vertices[0][1]) * (vertices[3][2] - vertices[0][2]) - 
                     (vertices[2][2] - vertices[0][2]) * (vertices[3][1] - vertices[0][1])) -
                    (vertices[1][1] - vertices[0][1]) * 
                    ((vertices[2][0] - vertices[0][0]) * (vertices[3][2] - vertices[0][2]) - 
                     (vertices[2][2] - vertices[0][2]) * (vertices[3][0] - vertices[0][0])) +
                    (vertices[1][2] - vertices[0][2]) * 
                    ((vertices[2][0] - vertices[0][0]) * (vertices[3][1] - vertices[0][1]) - 
                     (vertices[2][1] - vertices[0][1]) * (vertices[3][0] - vertices[0][0]));
      
      double V = fabs(detJ) / 6.0;  // Используем абсолютное значение
      if (V < 1e-12) {
        continue;
      }
      
      // Цвет в зависимости от положения элемента и деформации
      double color[3] = {0.7, 0.8, 0.9};  // Голубоватый цвет
      
      if (showStress && isAnimating) {
        // Цвет зависит от деформации
        float x = (vertices[0][0] + vertices[1][0] + vertices[2][0] + vertices[3][0]) / 4.0f;
        float stressLevel = animationProgress * (x / 4.0f);  // Больше деформации дальше от закрепления
        
        color[0] = 0.5 + 0.5 * stressLevel; // Красный
        color[1] = 0.5 - 0.3 * stressLevel; // Зеленый
        color[2] = 0.5 - 0.5 * stressLevel; // Синий
      } else {
        // Разные цвета для разных элементов
        if (elem % 4 == 0) {
          color[0] = 0.8; color[1] = 0.6; color[2] = 0.6; // Красноватый
        } else if (elem % 4 == 1) {
          color[0] = 0.6; color[1] = 0.8; color[2] = 0.6; // Зеленоватый
        } else if (elem % 4 == 2) {
          color[0] = 0.6; color[1] = 0.6; color[2] = 0.8; // Синеватый
        } else {
          color[0] = 0.8; color[1] = 0.8; color[2] = 0.6; // Желтоватый
        }
      }
      
      glColor3f(color[0], color[1], color[2]);
      
      // Отрисовка тетраэдра
      glBegin(GL_TRIANGLES);
      
      // Грань 1: узлы 1, 2, 3
      glVertex3f(vertices[0][0], vertices[0][1], vertices[0][2]);
      glVertex3f(vertices[1][0], vertices[1][1], vertices[1][2]);
      glVertex3f(vertices[2][0], vertices[2][1], vertices[2][2]);
      
      // Грань 2: узлы 1, 3, 4
      glVertex3f(vertices[0][0], vertices[0][1], vertices[0][2]);
      glVertex3f(vertices[2][0], vertices[2][1], vertices[2][2]);
      glVertex3f(vertices[3][0], vertices[3][1], vertices[3][2]);
      
      // Грань 3: узлы 1, 4, 2
      glVertex3f(vertices[0][0], vertices[0][1], vertices[0][2]);
      glVertex3f(vertices[3][0], vertices[3][1], vertices[3][2]);
      glVertex3f(vertices[1][0], vertices[1][1], vertices[1][2]);
      
      // Грань 4: узлы 2, 4, 3
      glVertex3f(vertices[1][0], vertices[1][1], vertices[1][2]);
      glVertex3f(vertices[3][0], vertices[3][1], vertices[3][2]);
      glVertex3f(vertices[2][0], vertices[2][1], vertices[2][2]);
      
      glEnd();
      
      // Отрисовка ребер тетраэдра для лучшей видимости
      glDisable(GL_LIGHTING);
      glColor3f(0.0, 0.0, 0.0);
      glBegin(GL_LINES);
      
      // Ребра тетраэдра
      glVertex3f(vertices[0][0], vertices[0][1], vertices[0][2]);
      glVertex3f(vertices[1][0], vertices[1][1], vertices[1][2]);
      
      glVertex3f(vertices[0][0], vertices[0][1], vertices[0][2]);
      glVertex3f(vertices[2][0], vertices[2][1], vertices[2][2]);
      
      glVertex3f(vertices[0][0], vertices[0][1], vertices[0][2]);
      glVertex3f(vertices[3][0], vertices[3][1], vertices[3][2]);
      
      glVertex3f(vertices[1][0], vertices[1][1], vertices[1][2]);
      glVertex3f(vertices[2][0], vertices[2][1], vertices[2][2]);
      
      glVertex3f(vertices[1][0], vertices[1][1], vertices[1][2]);
      glVertex3f(vertices[3][0], vertices[3][1], vertices[3][2]);
      
      glVertex3f(vertices[2][0], vertices[2][1], vertices[2][2]);
      glVertex3f(vertices[3][0], vertices[3][1], vertices[3][2]);
      
      glEnd();
      glEnable(GL_LIGHTING);
      
      drawnElements++;
    }
  } else {
    // Fallback: отрисовка простого куба
    double vertices[8][3] = {
      {-1.0, -1.0, -1.0},
      {1.0, -1.0, -1.0},
      {1.0, 1.0, -1.0},
      {-1.0, 1.0, -1.0},
      {-1.0, -1.0, 1.0},
      {1.0, -1.0, 1.0},
      {1.0, 1.0, 1.0},
      {-1.0, 1.0, 1.0}
    };
    
    double color[3] = {0.8, 0.8, 0.8};
    
    // Отрисовка куба
    glColor3f(color[0], color[1], color[2]);
    glBegin(GL_QUADS);
    
    // Передняя грань
    glVertex3f(vertices[0][0], vertices[0][1], vertices[0][2]);
    glVertex3f(vertices[1][0], vertices[1][1], vertices[1][2]);
    glVertex3f(vertices[2][0], vertices[2][1], vertices[2][2]);
    glVertex3f(vertices[3][0], vertices[3][1], vertices[3][2]);
    
    // Задняя грань
    glVertex3f(vertices[4][0], vertices[4][1], vertices[4][2]);
    glVertex3f(vertices[5][0], vertices[5][1], vertices[5][2]);
    glVertex3f(vertices[6][0], vertices[6][1], vertices[6][2]);
    glVertex3f(vertices[7][0], vertices[7][1], vertices[7][2]);
    
    // Левая грань
    glVertex3f(vertices[0][0], vertices[0][1], vertices[0][2]);
    glVertex3f(vertices[3][0], vertices[3][1], vertices[3][2]);
    glVertex3f(vertices[7][0], vertices[7][1], vertices[7][2]);
    glVertex3f(vertices[4][0], vertices[4][1], vertices[4][2]);
    
    // Правая грань
    glVertex3f(vertices[1][0], vertices[1][1], vertices[1][2]);
    glVertex3f(vertices[2][0], vertices[2][1], vertices[2][2]);
    glVertex3f(vertices[6][0], vertices[6][1], vertices[6][2]);
    glVertex3f(vertices[5][0], vertices[5][1], vertices[5][2]);
    
    // Верхняя грань
    glVertex3f(vertices[3][0], vertices[3][1], vertices[3][2]);
    glVertex3f(vertices[2][0], vertices[2][1], vertices[2][2]);
    glVertex3f(vertices[6][0], vertices[6][1], vertices[6][2]);
    glVertex3f(vertices[7][0], vertices[7][1], vertices[7][2]);
    
    // Нижняя грань
    glVertex3f(vertices[0][0], vertices[0][1], vertices[0][2]);
    glVertex3f(vertices[1][0], vertices[1][1], vertices[1][2]);
    glVertex3f(vertices[5][0], vertices[5][1], vertices[5][2]);
    glVertex3f(vertices[4][0], vertices[4][1], vertices[4][2]);
    
    glEnd();
  }
}

void updateAnimation3d(int __attribute__((unused)) value) {
  if (isAnimating) {
    // Анимация деформации
    animationProgress += 0.02f * animationDirection;
    if (animationProgress >= 1.0f) {
      animationProgress = 1.0f;
      animationDirection = -1;
    } else if (animationProgress <= 0.0f) {
      animationProgress = 0.0f;
      animationDirection = 1;
    }
    
    // Анимация нагрузки
    loadAnimation += 0.05f;
    if (loadAnimation > 2.0f * M_PI) {
      loadAnimation = 0.0f;
    }
    
    glutPostRedisplay();
  }
  
  glutTimerFunc(50, updateAnimation3d, 0);
}

void keyboard3d(unsigned char key, int __attribute__((unused)) x,
                int __attribute__((unused)) y) {
  switch (key) {
    case 'q':
    case 'Q':
    case 27:  // ESC
      printf("Exiting...\n");
      exit(0);
      break;
    case 'r':
    case 'R':
      rotationX = 0.0f;
      rotationY = 0.0f;
      rotationZ = 0.0f;
      zoom = 1.0f;
      printf("Reset view\n");
      break;
    case 'x':
      rotationX += 10.0f;
      printf("Rotate X: %.1f\n", rotationX);
      break;
    case 'X':
      rotationX -= 10.0f;
      printf("Rotate X: %.1f\n", rotationX);
      break;
    case 'y':
      rotationY += 10.0f;
      printf("Rotate Y: %.1f\n", rotationY);
      break;
    case 'Y':
      rotationY -= 10.0f;
      printf("Rotate Y: %.1f\n", rotationY);
      break;
    case 'z':
      rotationZ += 10.0f;
      printf("Rotate Z: %.1f\n", rotationZ);
      break;
    case 'Z':
      rotationZ -= 10.0f;
      printf("Rotate Z: %.1f\n", rotationZ);
      break;
    case '+':
    case '=':
      zoom *= 1.1f;
      printf("Zoom: %.2f\n", zoom);
      break;
    case '-':
    case '_':
      zoom /= 1.1f;
      printf("Zoom: %.2f\n", zoom);
      break;
    case 's':
    case 'S':
      showStress = !showStress;
      printf("Show stress: %s\n", showStress ? "ON" : "OFF");
      break;
    case 'd':
    case 'D':
      showDeformed = !showDeformed;
      printf("Show deformed: %s\n", showDeformed ? "ON" : "OFF");
      break;
    case 'v':
    case 'V':
      showValues = !showValues;
      printf("Show values: %s\n", showValues ? "ON" : "OFF");
      break;
    case 'a':
    case 'A':
      isAnimating = !isAnimating;
      printf("Animation: %s\n", isAnimating ? "ON" : "OFF");
      break;
    case 'l':
    case 'L':
      showLoad = !showLoad;
      printf("Show load: %s\n", showLoad ? "ON" : "OFF");
      break;
    case '1':
      deformationScale = 0.5f;
      printf("Deformation scale: %.1f\n", deformationScale);
      break;
    case '2':
      deformationScale = 1.0f;
      printf("Deformation scale: %.1f\n", deformationScale);
      break;
    case '3':
      deformationScale = 2.0f;
      printf("Deformation scale: %.1f\n", deformationScale);
      break;
    default:
      printf("Controls:\n");
      printf("  R - Reset view\n");
      printf("  X/Y/Z - Rotate around X/Y/Z axis\n");
      printf("  +/- - Zoom in/out\n");
      printf("  S - Toggle stress visualization\n");
      printf("  D - Toggle deformed view\n");
      printf("  V - Toggle value display\n");
      printf("  A - Toggle animation\n");
      printf("  L - Toggle load display\n");
      printf("  1/2/3 - Deformation scale (0.5/1.0/2.0)\n");
      printf("  Q/ESC - Quit\n");
      break;
  }
  
  glutPostRedisplay();
}

void renderText3d(float x, float y, float z, const char *text) {
  glRasterPos3f(x, y, z);
  for (const char *c = text; *c != '\0'; c++) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, *c);
  }
} 