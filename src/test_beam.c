#include <GLUT/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Простые данные балки
float vertices[20][3] = {
    {0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {2.0, 0.0, 0.0}, {3.0, 0.0, 0.0},
    {4.0, 0.0, 0.0}, {0.0, 0.5, 0.0}, {1.0, 0.5, 0.0}, {2.0, 0.5, 0.0},
    {3.0, 0.5, 0.0}, {4.0, 0.5, 0.0}, {0.0, 0.0, 0.5}, {1.0, 0.0, 0.5},
    {2.0, 0.0, 0.5}, {3.0, 0.0, 0.5}, {4.0, 0.0, 0.5}, {0.0, 0.5, 0.5},
    {1.0, 0.5, 0.5}, {2.0, 0.5, 0.5}, {3.0, 0.5, 0.5}, {4.0, 0.5, 0.5}};

int tetrahedra[96][4] = {
    {0, 1, 5, 10},    {1, 6, 5, 11},    {1, 2, 6, 11},    {2, 7, 6, 12},
    {2, 3, 7, 12},    {3, 8, 7, 13},    {3, 4, 8, 13},    {4, 9, 8, 14},
    {5, 6, 10, 15},   {6, 11, 10, 16},  {6, 7, 11, 16},   {7, 12, 11, 17},
    {7, 8, 12, 17},   {8, 13, 12, 18},  {8, 9, 13, 18},   {9, 14, 13, 19},
    {10, 11, 15, 20}, {11, 16, 15, 21}, {11, 12, 16, 21}, {12, 17, 16, 22},
    {12, 13, 17, 22}, {13, 18, 17, 23}, {13, 14, 18, 23}, {14, 19, 18, 24},
    {15, 16, 20, 25}, {16, 21, 20, 26}, {16, 17, 21, 26}, {17, 22, 21, 27},
    {17, 18, 22, 27}, {18, 23, 22, 28}, {18, 19, 23, 28}, {19, 24, 23, 29}};

float rotationX = 0.0f;
float rotationY = 0.0f;
float zoom = 1.0f;
float animationProgress = 0.0f;
int isAnimating = 1;
int animationDirection = 1;

void init() {
  printf("Initializing OpenGL...\n");
  glClearColor(0.2, 0.2, 0.2, 1.0);
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

void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();

  // Настройка камеры для обзора балки
  gluLookAt(8.0, 4.0, 6.0,    // позиция камеры
            2.0, 0.25, 0.25,  // точка, на которую смотрит камера (центр балки)
            0.0, 1.0, 0.0);   // вектор "вверх"

  // Применение поворотов
  glRotatef(rotationX, 1.0, 0.0, 0.0);
  glRotatef(rotationY, 0.0, 1.0, 0.0);

  // Масштабирование
  glScalef(zoom, zoom, zoom);

  // Отрисовка осей координат
  glDisable(GL_LIGHTING);
  glBegin(GL_LINES);
  glColor3f(1.0, 0.0, 0.0);  // Красная ось X
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(6.0, 0.0, 0.0);
  glColor3f(0.0, 1.0, 0.0);  // Зеленая ось Y
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(0.0, 2.0, 0.0);
  glColor3f(0.0, 0.0, 1.0);  // Синяя ось Z
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(0.0, 0.0, 2.0);
  glEnd();
  glEnable(GL_LIGHTING);

  // Отрисовка нагрузки
  glDisable(GL_LIGHTING);
  glColor3f(1.0, 0.0, 0.0);  // Красный цвет для нагрузки

  float loadX = 4.0f;  // Позиция нагрузки (конец балки)
  float loadY = 0.25f;
  float loadZ = 0.25f;
  float loadSize =
      0.3f *
      (0.5f + 0.5f * sin(animationProgress * 10.0f));  // Пульсирующая нагрузка

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

  // Отрисовка тетраэдров балки
  for (int elem = 0; elem < 30;
       elem++) {  // Показываем только первые 30 элементов для простоты
    float deformedVertices[4][3];

    // Копируем вершины и применяем деформацию
    for (int i = 0; i < 4; i++) {
      int nodeIndex = tetrahedra[elem][i];
      deformedVertices[i][0] = vertices[nodeIndex][0];
      deformedVertices[i][1] = vertices[nodeIndex][1];
      deformedVertices[i][2] = vertices[nodeIndex][2];

      // Применяем деформацию
      if (isAnimating) {
        float x = deformedVertices[i][0];
        float deflection = 0.0f;
        if (x > 0.0f) {
          float maxDeflection = 0.5f * animationProgress;
          deflection = maxDeflection * (x * x) / 16.0f;
        }
        deformedVertices[i][1] += deflection;

        float twist = 0.1f * animationProgress * sin(x * M_PI / 4.0f);
        deformedVertices[i][2] += twist;
      }
    }

    // Цвет в зависимости от деформации
    float x = (deformedVertices[0][0] + deformedVertices[1][0] +
               deformedVertices[2][0] + deformedVertices[3][0]) /
              4.0f;
    float stressLevel = animationProgress * (x / 4.0f);

    float color[3];
    color[0] = 0.5 + 0.5 * stressLevel;  // Красный
    color[1] = 0.5 - 0.3 * stressLevel;  // Зеленый
    color[2] = 0.5 - 0.5 * stressLevel;  // Синий

    glColor3f(color[0], color[1], color[2]);

    // Отрисовка тетраэдра
    glBegin(GL_TRIANGLES);

    // Грань 1: узлы 1, 2, 3
    glVertex3f(deformedVertices[0][0], deformedVertices[0][1],
               deformedVertices[0][2]);
    glVertex3f(deformedVertices[1][0], deformedVertices[1][1],
               deformedVertices[1][2]);
    glVertex3f(deformedVertices[2][0], deformedVertices[2][1],
               deformedVertices[2][2]);

    // Грань 2: узлы 1, 3, 4
    glVertex3f(deformedVertices[0][0], deformedVertices[0][1],
               deformedVertices[0][2]);
    glVertex3f(deformedVertices[2][0], deformedVertices[2][1],
               deformedVertices[2][2]);
    glVertex3f(deformedVertices[3][0], deformedVertices[3][1],
               deformedVertices[3][2]);

    // Грань 3: узлы 1, 4, 2
    glVertex3f(deformedVertices[0][0], deformedVertices[0][1],
               deformedVertices[0][2]);
    glVertex3f(deformedVertices[3][0], deformedVertices[3][1],
               deformedVertices[3][2]);
    glVertex3f(deformedVertices[1][0], deformedVertices[1][1],
               deformedVertices[1][2]);

    // Грань 4: узлы 2, 4, 3
    glVertex3f(deformedVertices[1][0], deformedVertices[1][1],
               deformedVertices[1][2]);
    glVertex3f(deformedVertices[3][0], deformedVertices[3][1],
               deformedVertices[3][2]);
    glVertex3f(deformedVertices[2][0], deformedVertices[2][1],
               deformedVertices[2][2]);

    glEnd();
  }

  glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
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
    case 'a':
    case 'A':
      isAnimating = !isAnimating;
      printf("Animation: %s\n", isAnimating ? "ON" : "OFF");
      break;
    default:
      printf("Controls: R=Reset, X/Y=Rotate, +/-=Zoom, A=Animation, Q=Quit\n");
      break;
  }
  glutPostRedisplay();
}

void update(int value) {
  if (isAnimating) {
    animationProgress += 0.02f * animationDirection;
    if (animationProgress >= 1.0f) {
      animationProgress = 1.0f;
      animationDirection = -1;
    } else if (animationProgress <= 0.0f) {
      animationProgress = 0.0f;
      animationDirection = 1;
    }
    glutPostRedisplay();
  }
  glutTimerFunc(50, update, 0);
}

void reshape(int w, int h) {
  printf("Reshape: %dx%d\n", w, h);
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0, (float)w / (float)h, 0.1, 100.0);
  glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
  printf("=== Simple Beam Test ===\n");

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(800, 600);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("Simple Beam Test");

  printf("GLUT window created\n");

  init();

  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutReshapeFunc(reshape);
  glutTimerFunc(50, update, 0);

  printf("Starting GLUT main loop...\n");
  glutMainLoop();

  return 0;
}