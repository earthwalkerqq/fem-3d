#include <GLUT/glut.h>
#include <stdio.h>
#include <stdlib.h>

// Простые данные куба
float vertices[8][3] = {
    {0.0, 0.0, 0.0},  // 0
    {1.0, 0.0, 0.0},  // 1
    {1.0, 1.0, 0.0},  // 2
    {0.0, 1.0, 0.0},  // 3
    {0.0, 0.0, 1.0},  // 4
    {1.0, 0.0, 1.0},  // 5
    {1.0, 1.0, 1.0},  // 6
    {0.0, 1.0, 1.0}   // 7
};

int faces[6][4] = {
    {0, 1, 2, 3},  // передняя грань
    {5, 4, 7, 6},  // задняя грань
    {4, 0, 3, 7},  // левая грань
    {1, 5, 6, 2},  // правая грань
    {3, 2, 6, 7},  // верхняя грань
    {4, 5, 1, 0}   // нижняя грань
};

float rotationX = 0.0f;
float rotationY = 0.0f;
float zoom = 1.0f;

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
  printf("Display function called\n");

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();

  // Настройка камеры
  gluLookAt(3.0, 3.0, 3.0,   // позиция камеры
            0.5, 0.5, 0.5,   // точка, на которую смотрит камера
            0.0, 1.0, 0.0);  // вектор "вверх"

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
  glVertex3f(2.0, 0.0, 0.0);
  glColor3f(0.0, 1.0, 0.0);  // Зеленая ось Y
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(0.0, 2.0, 0.0);
  glColor3f(0.0, 0.0, 1.0);  // Синяя ось Z
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(0.0, 0.0, 2.0);
  glEnd();
  glEnable(GL_LIGHTING);

  // Отрисовка куба
  glColor3f(0.8, 0.6, 0.6);  // Красноватый цвет

  glBegin(GL_QUADS);
  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 4; j++) {
      int vertex = faces[i][j];
      glVertex3f(vertices[vertex][0], vertices[vertex][1], vertices[vertex][2]);
    }
  }
  glEnd();

  // Отрисовка ребер куба
  glDisable(GL_LIGHTING);
  glColor3f(0.0, 0.0, 0.0);
  glBegin(GL_LINES);

  // Ребра куба
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(1.0, 0.0, 0.0);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(0.0, 1.0, 0.0);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(0.0, 0.0, 1.0);
  glVertex3f(1.0, 0.0, 0.0);
  glVertex3f(1.0, 1.0, 0.0);
  glVertex3f(1.0, 0.0, 0.0);
  glVertex3f(1.0, 0.0, 1.0);
  glVertex3f(0.0, 1.0, 0.0);
  glVertex3f(1.0, 1.0, 0.0);
  glVertex3f(0.0, 1.0, 0.0);
  glVertex3f(0.0, 1.0, 1.0);
  glVertex3f(0.0, 0.0, 1.0);
  glVertex3f(1.0, 0.0, 1.0);
  glVertex3f(0.0, 0.0, 1.0);
  glVertex3f(0.0, 1.0, 1.0);
  glVertex3f(1.0, 1.0, 0.0);
  glVertex3f(1.0, 1.0, 1.0);
  glVertex3f(1.0, 0.0, 1.0);
  glVertex3f(1.0, 1.0, 1.0);
  glVertex3f(0.0, 1.0, 1.0);
  glVertex3f(1.0, 1.0, 1.0);

  glEnd();
  glEnable(GL_LIGHTING);

  glutSwapBuffers();
  printf("Display function completed\n");
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
    default:
      printf("Controls: R=Reset, X/Y=Rotate, +/-=Zoom, Q=Quit\n");
      break;
  }
  glutPostRedisplay();
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
  printf("=== Simple 3D Test Starting ===\n");

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(800, 600);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("Simple 3D Test");

  printf("GLUT window created\n");

  init();

  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutReshapeFunc(reshape);

  printf("Starting GLUT main loop...\n");
  glutMainLoop();

  return 0;
}