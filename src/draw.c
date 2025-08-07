#include "draw.h"
#include "defines.h"

void drawMashForSolve3d(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(WINDTH_WINDOW, HEIGHT_WINDOW);
  glutInitWindowPosition(GLUT_WINDOW_POSITION_X, GLUT_WINDOW_POSITION_Y);
  glutCreateWindow("3D FEM Analysis");

  init3d();

  glutDisplayFunc(display3d);
  glutKeyboardFunc(keyboard3d);
  glutReshapeFunc(reshape3d);
  glutTimerFunc(50, updateAnimation3d, 0);

  glutMainLoop();
}

void init3d(void) {
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
}

void display3d(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();

  // Настройка камеры для обзора балки
  gluLookAt(8.0, 4.0, 6.0,    // позиция камеры
            2.0, 0.25, 0.25,  // точка, на которую смотрит камера (центр балки)
            0.0, 1.0, 0.0);   // вектор "вверх"

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
    float loadSize =
        0.3f * (0.5f + 0.5f * sin(loadAnimation));  // Пульсирующая нагрузка

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
    for (int elem = 0; elem < nelem; elem++) {
      // Проверяем, что индексы узлов корректны
      if (jt03[elem][0] <= 0 || jt03[elem][1] <= 0 || jt03[elem][2] <= 0 ||
          jt03[elem][3] <= 0) {
        continue;  // Пропускаем некорректные элементы
      }

      // Получаем координаты узлов тетраэдра
      int node1 = jt03[elem][0] - 1;
      int node2 = jt03[elem][1] - 1;
      int node3 = jt03[elem][2] - 1;
      int node4 = jt03[elem][3] - 1;

      // Проверяем границы массива
      if (node1 < 0 || node2 < 0 || node3 < 0 || node4 < 0 || node1 >= nys ||
          node2 >= nys || node3 >= nys || node4 >= nys) {
        continue;  // Пропускаем элементы с некорректными индексами
      }

      double vertices[4][3] = {{car[node1][0], car[node1][1], car[node1][2]},
                               {car[node2][0], car[node2][1], car[node2][2]},
                               {car[node3][0], car[node3][1], car[node3][2]},
                               {car[node4][0], car[node4][1], car[node4][2]}};

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
                        ((vertices[2][1] - vertices[0][1]) *
                             (vertices[3][2] - vertices[0][2]) -
                         (vertices[2][2] - vertices[0][2]) *
                             (vertices[3][1] - vertices[0][1])) -
                    (vertices[1][1] - vertices[0][1]) *
                        ((vertices[2][0] - vertices[0][0]) *
                             (vertices[3][2] - vertices[0][2]) -
                         (vertices[2][2] - vertices[0][2]) *
                             (vertices[3][0] - vertices[0][0])) +
                    (vertices[1][2] - vertices[0][2]) *
                        ((vertices[2][0] - vertices[0][0]) *
                             (vertices[3][1] - vertices[0][1]) -
                         (vertices[2][1] - vertices[0][1]) *
                             (vertices[3][0] - vertices[0][0]));

      double V = fabs(detJ) / 6.0;  // Используем абсолютное значение
      if (V < 1e-12) {
        continue;
      }

      // Цвет в зависимости от положения элемента и деформации
      double color[3] = {0.7, 0.8, 0.9};  // Голубоватый цвет

      if (showStress && isAnimating) {
        // Цвет зависит от деформации
        float x = (vertices[0][0] + vertices[1][0] + vertices[2][0] +
                   vertices[3][0]) /
                  4.0f;
        float stressLevel =
            animationProgress *
            (x / 4.0f);  // Больше деформации дальше от закрепления

        color[0] = 0.5 + 0.5 * stressLevel;  // Красный
        color[1] = 0.5 - 0.3 * stressLevel;  // Зеленый
        color[2] = 0.5 - 0.5 * stressLevel;  // Синий
      } else {
        // Разные цвета для разных элементов
        if (elem % 4 == 0) {
          color[0] = 0.8;
          color[1] = 0.6;
          color[2] = 0.6;  // Красноватый
        } else if (elem % 4 == 1) {
          color[0] = 0.6;
          color[1] = 0.8;
          color[2] = 0.6;  // Зеленоватый
        } else if (elem % 4 == 2) {
          color[0] = 0.6;
          color[1] = 0.6;
          color[2] = 0.8;  // Синеватый
        } else {
          color[0] = 0.8;
          color[1] = 0.8;
          color[2] = 0.6;  // Желтоватый
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
    }
  } else {
    // Fallback: отрисовка простого куба
    double vertices[8][3] = {{-1.0, -1.0, -1.0}, {1.0, -1.0, -1.0},
                             {1.0, 1.0, -1.0},   {-1.0, 1.0, -1.0},
                             {-1.0, -1.0, 1.0},  {1.0, -1.0, 1.0},
                             {1.0, 1.0, 1.0},    {-1.0, 1.0, 1.0}};

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
  char flag_exit = 0;
  switch (key) {
    case 'q':
    case 'Q':
    case 27:  // ESC
      flag_exit = 1;
      break;
    case 'r':
    case 'R':
      rotationX = 0.0f;
      rotationY = 0.0f;
      rotationZ = 0.0f;
      zoom = 1.0f;
      break;
    case 'x':
      rotationX += 10.0f;
      break;
    case 'X':
      rotationX -= 10.0f;
      break;
    case 'y':
      rotationY += 10.0f;
      break;
    case 'Y':
      rotationY -= 10.0f;
      break;
    case 'z':
      rotationZ += 10.0f;
      break;
    case 'Z':
      rotationZ -= 10.0f;
      break;
    case '+':
    case '=':
      zoom *= 1.1f;
      break;
    case '-':
    case '_':
      zoom /= 1.1f;
      break;
    case 's':
    case 'S':
      showStress = !showStress;
      break;
    case 'd':
    case 'D':
      showDeformed = !showDeformed;
      break;
    case 'v':
    case 'V':
      showValues = !showValues;
      break;
    case 'a':
    case 'A':
      isAnimating = !isAnimating;
      break;
    case 'l':
    case 'L':
      showLoad = !showLoad;
      break;
    case '1':
      deformationScale = 0.5f;
      break;
    case '2':
      deformationScale = 1.0f;
      break;
    case '3':
      deformationScale = 2.0f;
      break;
  }
  if (flag_exit)
    return;
  glutPostRedisplay();
}

void renderText3d(float x, float y, float z, const char* text) {
  glRasterPos3f(x, y, z);
  for (const char* c = text; *c != '\0'; c++) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, *c);
  }
}