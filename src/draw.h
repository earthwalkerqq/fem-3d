#ifndef DRAW_H
#define DRAW_H

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

void drawMashForSolve3d(int argc, char** argv);
void drawModel3d(void);
void display3d(void);
void init3d(void);
void reshape3d(int w, int h);
void updateAnimation3d(int __attribute__((unused)) value);
void keyboard3d(unsigned char key, int __attribute__((unused)) x,
                int __attribute__((unused)) y);
void renderText3d(float x, float y, float z, const char* text);

#endif