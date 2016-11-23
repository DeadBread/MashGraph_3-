#include <iostream>
#include <vector>

#include "Utility.h"
#include "SOIL.h"

using namespace std;

const uint GRASS_INSTANCES = 100000; // Количество травинок

GL::Camera camera;               // Мы предоставляем Вам реализацию камеры. В OpenGL камера - это просто 2 матрицы. Модельно-видовая матрица и матрица проекции. // ###
                                 // Задача этого класса только в том чтобы обработать ввод с клавиатуры и правильно сформировать эти матрицы.
                                 // Вы можете просто пользоваться этим классом для расчёта указанных матриц.


GLuint grassPointsCount; // Количество вершин у модели травинки
GLuint grassShader;      // Шейдер, рисующий траву
GLuint grassVAO;         // VAO для травы (что такое VAO почитайте в доках)
GLuint grassVariance;    // Буфер для смещения координат травинок
vector<VM::vec4> grassVarianceData(GRASS_INSTANCES); // Вектор со смещениями для координат травинок

GLuint groundShader; // Шейдер для земли
GLuint groundVAO; // VAO для земли

GLuint groundTexture; // Текстура для земли
GLuint grassTexture; //Текстура для травы

// Размеры экрана
uint screenWidth = 800;
uint screenHeight = 600;

// Это для захвата мышки. Вам это не потребуется (это не значит, что нужно удалять эту строку)
bool captureMouse = true;

// Функция, рисующая замлю
void DrawGround() {

    // Используем шейдер для земли
    glUseProgram(groundShader);                                                  CHECK_GL_ERRORS

    // Устанавливаем юниформ для шейдера. В данном случае передадим перспективную матрицу камеры
    // Находим локацию юниформа 'camera' в шейдере
    GLint cameraLocation = glGetUniformLocation(groundShader, "camera");         CHECK_GL_ERRORS
    // Устанавливаем юниформ (загружаем на GPU матрицу проекции?)                                                     // ###
    glUniformMatrix4fv(cameraLocation, 1, GL_TRUE, camera.getMatrix().data().data()); CHECK_GL_ERRORS

    // Подключаем VAO, который содержит буферы, необходимые для отрисовки земли
    glBindVertexArray(groundVAO);                                                CHECK_GL_ERRORS

    glActiveTexture(GL_TEXTURE0);
    //glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glUniform1i(glGetUniformLocation(groundShader, "groundTexture"), 0);

    // Рисуем землю: 2 треугольника (6 вершин)
    glDrawArrays(GL_TRIANGLES, 0, 6);                                            CHECK_GL_ERRORS

    // Отсоединяем VAO
    glBindVertexArray(0);                                                        CHECK_GL_ERRORS
    // Отключаем шейдер
    glUseProgram(0);                                                             CHECK_GL_ERRORS
}

float windVar1 = 0;
float windVar2 = 0;
// Обновление смещения травинок
void UpdateGrassVariance() {
    // Генерация случайных смещений

    //VM::vec4 wind(0.1 ,0 ,0.1, 0);



    VM::vec4 wind(abs(cos(windVar1)) / 30, 0, (abs(cos(windVar2))) / 30, 0);
    windVar1 += 0.02;
    windVar2 += 0.04;

    if (windVar1 > 2 * M_PI)
        windVar1 -= 2 * M_PI;//

    if (windVar2 > 2 * M_PI)
        windVar2 -= 2 * M_PI;
    //cout << wind[0] << ' ' << wind[2] << endl;


    //VM::vec4 wind(0,0,0,0);

    static vector<VM::vec4> acceleratons(GRASS_INSTANCES, VM::vec4(0,0,0,0));
    static vector<VM::vec4> speeds(GRASS_INSTANCES, VM::vec4(0,0,0,0));

    float g = 0.005;

    for (uint i = 0; i < GRASS_INSTANCES; ++i) {

        double k = 6 + i * 1.001 / GRASS_INSTANCES;

        VM::vec4 hook;
        hook.x = - k * grassVarianceData[i].x;
        hook.y = - k * grassVarianceData[i].y;
        hook.z = - k * grassVarianceData[i].z;

        acceleratons[i].x = (wind.x + hook.x);
        acceleratons[i].y = (wind.y + hook.y + g);
        acceleratons[i].z = (wind.z + hook.z);


        speeds[i].x += acceleratons[i].x;
        speeds[i].y += acceleratons[i].y;
        speeds[i].z += acceleratons[i].z;

        grassVarianceData[i].x += speeds[i].x / 3000 ; // = (float)rand() / RAND_MAX / 100;
        grassVarianceData[i].y += speeds[i].y / 3000 ;
        grassVarianceData[i].z += speeds[i].z / 3000 ;  //= (float)rand() / RAND_MAX / 100;
    }

    // Привязываем буфер, содержащий смещения
    glBindBuffer(GL_ARRAY_BUFFER, grassVariance);                                CHECK_GL_ERRORS
    // Загружаем данные в видеопамять
    glBufferData(GL_ARRAY_BUFFER, sizeof(VM::vec4) * GRASS_INSTANCES, grassVarianceData.data(), GL_STATIC_DRAW); CHECK_GL_ERRORS
    // Отвязываем буфер
    glBindBuffer(GL_ARRAY_BUFFER, 0);                                            CHECK_GL_ERRORS
}


// Рисование травы
void DrawGrass() {
    // Тут то же самое, что и в рисовании земли
    glUseProgram(grassShader);                                                   CHECK_GL_ERRORS
    GLint cameraLocation = glGetUniformLocation(grassShader, "camera");          CHECK_GL_ERRORS
    glUniformMatrix4fv(cameraLocation, 1, GL_TRUE, camera.getMatrix().data().data()); CHECK_GL_ERRORS
    glBindVertexArray(grassVAO);                                                 CHECK_GL_ERRORS
    // Обновляем смещения для травы
    UpdateGrassVariance();
    // Отрисовка травинок в количестве GRASS_INSTANCES

    glActiveTexture(GL_TEXTURE1);
    glUniform1i(glGetUniformLocation(grassShader, "grassTexture"), 1);

    glDrawArraysInstanced(GL_TRIANGLES, 0, grassPointsCount, GRASS_INSTANCES);   CHECK_GL_ERRORS
    glBindVertexArray(0);                                                        CHECK_GL_ERRORS
    glUseProgram(0);                                                             CHECK_GL_ERRORS
}

// Эта функция вызывается для обновления экрана
void RenderLayouts() {
    // Включение буфера глубины
    glEnable(GL_DEPTH_TEST);
    // Очистка буфера глубины и цветового буфера
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Рисуем меши
    DrawGround();
    DrawGrass();  
    glutSwapBuffers();
}

// Завершение программы
void FinishProgram() {
    glutDestroyWindow(glutGetWindow());
}

// Обработка события нажатия клавиши (специальные клавиши обрабатываются в функции SpecialButtons)
void KeyboardEvents(unsigned char key, int x, int y) {
    if (key == 27) {
        FinishProgram();
    } else if (key == 'w') {
        camera.goForward();
    } else if (key == 's') {
        camera.goBack();
    } else if (key == 'm') {
        captureMouse = !captureMouse;
        if (captureMouse) {
            glutWarpPointer(screenWidth / 2, screenHeight / 2);
            glutSetCursor(GLUT_CURSOR_NONE);
        } else {
            glutSetCursor(GLUT_CURSOR_RIGHT_ARROW);
        }
    }
}

// Обработка события нажатия специальных клавиш
void SpecialButtons(int key, int x, int y) {
    if (key == GLUT_KEY_RIGHT) {
        camera.rotateY(0.02);
    } else if (key == GLUT_KEY_LEFT) {
        camera.rotateY(-0.02);
    } else if (key == GLUT_KEY_UP) {
        camera.rotateTop(-0.02);
    } else if (key == GLUT_KEY_DOWN) {
        camera.rotateTop(0.02);
    }
}

void IdleFunc() {
    glutPostRedisplay();
}

// Обработка события движения мыши
void MouseMove(int x, int y) {
    if (captureMouse) {
        int centerX = screenWidth / 2,
            centerY = screenHeight / 2;
        if (x != centerX || y != centerY) {
            camera.rotateY((x - centerX) / 1000.0f);
            camera.rotateTop((y - centerY) / 1000.0f);
            glutWarpPointer(centerX, centerY);
        }
    }
}

// Обработка нажатия кнопки мыши
void MouseClick(int button, int state, int x, int y) {
}

// Событие изменение размера окна
void windowReshapeFunc(GLint newWidth, GLint newHeight) {
    glViewport(0, 0, newWidth, newHeight);
    screenWidth = newWidth;
    screenHeight = newHeight;

    camera.screenRatio = (float)screenWidth / screenHeight;
}

// Инициализация окна
void InitializeGLUT(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutInitContextVersion(3, 0);
    glutInitWindowPosition(-1, -1);
    glutInitWindowSize(screenWidth, screenHeight);
    glutCreateWindow("Computer Graphics 3");
    glutWarpPointer(400, 300);
    glutSetCursor(GLUT_CURSOR_NONE);

    glutDisplayFunc(RenderLayouts);
    glutKeyboardFunc(KeyboardEvents);
    glutSpecialFunc(SpecialButtons);
    glutIdleFunc(IdleFunc);
    glutPassiveMotionFunc(MouseMove);
    glutMouseFunc(MouseClick);
    glutReshapeFunc(windowReshapeFunc);
}

// Генерация позиций травинок (эту функцию вам придётся переписать)
vector<VM::vec2> GenerateGrassPositions() {
    srand(time(0));
    vector<VM::vec2> grassPositions(GRASS_INSTANCES);
    for (uint i = 0; i < GRASS_INSTANCES; ++i) {
        grassPositions[i] = VM::vec2((float)rand() * 10 / RAND_MAX , (float)rand() * 10 / RAND_MAX);
    }
    return grassPositions;
}

// Здесь вам нужно будет генерировать меш
vector<VM::vec4> GenMesh(uint n) {

    double rx = (double)rand();
    double ry = (double)rand();
    double scmul = sqrt(pow(rx, 2) + pow(ry, 2));

    rx /= scmul;
    ry /= (scmul * 1000);

    return {
        VM::vec4(0, 0, 0, 1),
        VM::vec4(0.25, 0, 0, 1),
        VM::vec4(0, 0.5, 0, 1),

        VM::vec4(0, 0.5, 0, 1),
        VM::vec4(0.25, 0.5, 0, 1),
        VM::vec4(0.25, 0, 0, 1),

        VM::vec4(0, 0.5, 0, 1),
        VM::vec4(0.25, 0.5, 0, 1),
        VM::vec4(0, 1, 0, 1),

        VM::vec4(0, 1, 0, 1),
        VM::vec4(0.25, 1, 0, 1),
        VM::vec4(0.25, 0.5, 0, 1),

        VM::vec4(0, 1, 0, 1),
        VM::vec4(0.25, 1, 0, 1),
        VM::vec4(0, 1.5, 0, 1),

        VM::vec4(0, 1.5, 0, 1),
        VM::vec4(0.25, 1.5, 0, 1),
        VM::vec4(0.25, 1, 0, 1),

        VM::vec4(0, 1.5, 0, 1),
        VM::vec4(0.25, 1.5, 0, 1),
        VM::vec4(0, 2, 0, 1),

        VM::vec4(0, 2, 0, 1),
        VM::vec4(0.25, 2, 0, 1),
        VM::vec4(0.25, 1.5, 0, 1),

        VM::vec4(0, 2, 0, 1),
        VM::vec4(0.25, 2, 0, 1),
        VM::vec4(0.25, 2.25, 0, 1),

    };
}

// Создание травы
void CreateGrass() {

    glActiveTexture(GL_TEXTURE1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, grassTexture);
    grassTexture = SOIL_load_OGL_texture("../Texture/grass.jpg", 3, 0 ,0);
    cout << grassTexture << endl;
    glGenerateMipmap(GL_TEXTURE_2D);


    uint LOD = 1;
    // Создаём меш
    vector<VM::vec4> grassPoints = GenMesh(LOD);
    // Сохраняем количество вершин в меше травы
    grassPointsCount = grassPoints.size();
    // Создаём позиции для травинок
    vector<VM::vec2> grassPositions = GenerateGrassPositions();
    // Инициализация смещений для травинок
    for (uint i = 0; i < GRASS_INSTANCES; ++i) {
        grassVarianceData[i] = VM::vec4(0, 0, 0, 0);
    }

    /* Компилируем шейдеры
    Эта функция принимает на вход название шейдера 'shaderName',
    читает файлы shaders/{shaderName}.vert - вершинный шейдер
    и shaders/{shaderName}.frag - фрагментный шейдер,
    компилирует их и линкует.
    */
    grassShader = GL::CompileShaderProgram("grass");

    // Здесь создаём буфер
    GLuint pointsBuffer;
    // Это генерация одного буфера (в pointsBuffer хранится идентификатор буфера)
    glGenBuffers(1, &pointsBuffer);                                              CHECK_GL_ERRORS
    // Привязываем сгенерированный буфер
    glBindBuffer(GL_ARRAY_BUFFER, pointsBuffer);                                 CHECK_GL_ERRORS
    // Заполняем буфер данными из вектора
    glBufferData(GL_ARRAY_BUFFER, sizeof(VM::vec4) * grassPoints.size(), grassPoints.data(), GL_STATIC_DRAW); CHECK_GL_ERRORS

    // Создание VAO
    // Генерация VAO
    glGenVertexArrays(1, &grassVAO);                                             CHECK_GL_ERRORS
    // Привязка VAO
    glBindVertexArray(grassVAO);                                                 CHECK_GL_ERRORS

    // Получение локации параметра 'point' в шейдере
    GLuint pointsLocation = glGetAttribLocation(grassShader, "point");           CHECK_GL_ERRORS
    // Подключаем массив атрибутов к данной локации
    glEnableVertexAttribArray(pointsLocation);                                   CHECK_GL_ERRORS
    // Устанавливаем параметры для получения данных из массива (по 4 значение типа float на одну вершину)
    glVertexAttribPointer(pointsLocation, 4, GL_FLOAT, GL_FALSE, 0, 0);          CHECK_GL_ERRORS

    // Создаём буфер для позиций травинок
    GLuint positionBuffer;
    glGenBuffers(1, &positionBuffer);                                            CHECK_GL_ERRORS
    // Здесь мы привязываем новый буфер, так что дальше вся работа будет с ним до следующего вызова glBindBuffer
    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);                               CHECK_GL_ERRORS
    glBufferData(GL_ARRAY_BUFFER, sizeof(VM::vec2) * grassPositions.size(), grassPositions.data(), GL_STATIC_DRAW); CHECK_GL_ERRORS

    GLuint positionLocation = glGetAttribLocation(grassShader, "position");      CHECK_GL_ERRORS
    glEnableVertexAttribArray(positionLocation);                                 CHECK_GL_ERRORS
    glVertexAttribPointer(positionLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);        CHECK_GL_ERRORS
    // Здесь мы указываем, что нужно брать новое значение из этого буфера для каждого инстанса (для каждой травинки)
    glVertexAttribDivisor(positionLocation, 1);                                  CHECK_GL_ERRORS

    // Создаём буфер для смещения травинок
    glGenBuffers(1, &grassVariance);                                            CHECK_GL_ERRORS
    glBindBuffer(GL_ARRAY_BUFFER, grassVariance);                               CHECK_GL_ERRORS
    glBufferData(GL_ARRAY_BUFFER, sizeof(VM::vec4) * GRASS_INSTANCES, grassVarianceData.data(), GL_STATIC_DRAW); CHECK_GL_ERRORS

    GLuint varianceLocation = glGetAttribLocation(grassShader, "variance");      CHECK_GL_ERRORS
    glEnableVertexAttribArray(varianceLocation);                                 CHECK_GL_ERRORS
    glVertexAttribPointer(varianceLocation, 4, GL_FLOAT, GL_FALSE, 0, 0);        CHECK_GL_ERRORS
    glVertexAttribDivisor(varianceLocation, 1);                                  CHECK_GL_ERRORS

    // Отвязываем VAO
    glBindVertexArray(0);                                                        CHECK_GL_ERRORS
    // Отвязываем буфер
    glBindBuffer(GL_ARRAY_BUFFER, 0);                                            CHECK_GL_ERRORS
}

// Создаём камеру (Если шаблонная камера вам не нравится, то можете переделать, но я бы не стал)
void CreateCamera() {
    camera.angle = 45.0f / 180.0f * M_PI;
    camera.direction = VM::vec3(0, 0.3, -1);
    camera.position = VM::vec3(0.5, 0.2, 0);
    camera.screenRatio = (float)screenWidth / screenHeight;
    camera.up = VM::vec3(0, 1, 0);
    camera.zfar = 50.0f;
    camera.znear = 0.05f;
}

// Создаём зeмлю
void CreateGround() {
    // Земля состоит из двух треугольников
    vector<VM::vec4> meshPoints = {
        VM::vec4(0, 0, 0, 1),
        VM::vec4(10, 0, 0, 1),
        VM::vec4(10, 0, 10, 1),
        VM::vec4(0, 0, 0, 1),
        VM::vec4(10, 0, 10, 1),
        VM::vec4(0, 0, 10, 1),
    };

    // Подробнее о том, как это работает читайте в функции CreateGrass

    groundShader = GL::CompileShaderProgram("ground");

    GLuint pointsBuffer;

    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, groundTexture);
    //TODO: заменить на относительный путь!
    groundTexture = SOIL_load_OGL_texture("../Texture/ground.bmp", 3, 0 ,0);
    cout << groundTexture << endl;
    glGenerateMipmap(GL_TEXTURE_2D);

    glGenBuffers(1, &pointsBuffer);                                              CHECK_GL_ERRORS
    glBindBuffer(GL_ARRAY_BUFFER, pointsBuffer);                                 CHECK_GL_ERRORS
    glBufferData(GL_ARRAY_BUFFER, sizeof(VM::vec4) * meshPoints.size(), meshPoints.data(), GL_STATIC_DRAW); CHECK_GL_ERRORS

    glGenVertexArrays(1, &groundVAO);                                            CHECK_GL_ERRORS
    glBindVertexArray(groundVAO);                                                CHECK_GL_ERRORS

    //glBindTexture(GL_TEXTURE_2D, text);                                          CHECK_GL_ERRORS

    GLuint index = glGetAttribLocation(groundShader, "point");                   CHECK_GL_ERRORS
    glEnableVertexAttribArray(index);                                            CHECK_GL_ERRORS
    glVertexAttribPointer(index, 4, GL_FLOAT, GL_FALSE, 0, 0);                   CHECK_GL_ERRORS

    glBindVertexArray(0);                                                        CHECK_GL_ERRORS
    glBindBuffer(GL_ARRAY_BUFFER, 0);                                            CHECK_GL_ERRORS
}

int main(int argc, char **argv)
{
    putenv("MESA_GL_VERSION_OVERRIDE=3.3COMPAT");
    try {
        cout << "Start" << endl;
        InitializeGLUT(argc, argv);
        cout << "GLUT inited" << endl;
        glewInit();
        cout << "glew inited" << endl;
        CreateCamera();
        cout << "Camera created" << endl;
        CreateGrass();
        cout << "Grass created" << endl;
        CreateGround();
        cout << "Ground created" << endl;
        glutMainLoop();
    } catch (string s) {
        cout << s << endl;
    }
}
