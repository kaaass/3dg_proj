#include <iostream>
#include "game.h"
#include "lighting.h"

Game *Game::INSTANCE = nullptr;

int Game::start() {
    glfwInit();
    // OpenGL 版本配置
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    // 创建窗口
    window = glfwCreateWindow(screen.width, screen.height, "3DG Project", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    // 初始化 GLAD
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // 视口设置
    glViewport(0, 0, screen.width, screen.height);
    // 注册回调
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    // 捕获鼠标
    if (INSTANCE->control.mouseCap)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // 绘制准备
    stage = new Stage;
    stage->prepareDraw();
    // 主循环
    mainLoop();
    // 结束回收
    glfwTerminate();
    return 0;
}

void Game::mainLoop() {
    float &deltaTime = INSTANCE->control.deltaTime;
    float &lastFrame = INSTANCE->control.lastFrame;

    // 绘制循环
    while (!glfwWindowShouldClose(window)) {
        // 处理输入
        processInput(window);

        // 清屏
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 绘制
        stage->drawStaff();

        // 计算帧率
        double currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        stage->idle(deltaTime);

        // 交换帧缓冲
        glfwSwapBuffers(window);
        // 处理相关事件
        glfwPollEvents();
    }
}

Game *Game::getInstance() {
    if (INSTANCE == nullptr)
        INSTANCE = new Game;
    return INSTANCE;
}

Game::~Game() {
    delete stage;
}

/*
 * 回调
 */

void Game::framebuffer_size_callback(GLFWwindow *wind, int width, int height) {
    glViewport(0, 0, width, height);
    INSTANCE->screen.width = width;
    INSTANCE->screen.height = height;
}

void Game::processInput(GLFWwindow *wind) {
    auto *camera = INSTANCE->stage->getCamera();
    float &deltaTime = INSTANCE->control.deltaTime;

    if (glfwGetKey(wind, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(wind, true);
    }
    // 移动相机
    if (glfwGetKey(wind, GLFW_KEY_W) == GLFW_PRESS)
        camera->processKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(wind, GLFW_KEY_S) == GLFW_PRESS)
        camera->processKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(wind, GLFW_KEY_A) == GLFW_PRESS)
        camera->processKeyboard(LEFT, deltaTime);
    if (glfwGetKey(wind, GLFW_KEY_D) == GLFW_PRESS)
        camera->processKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(wind, GLFW_KEY_UP) == GLFW_PRESS) {
        camera->processMouseScroll(deltaTime * 10);
    }
    if (glfwGetKey(wind, GLFW_KEY_DOWN) == GLFW_PRESS) {
        camera->processMouseScroll(-deltaTime * 10);
    }
    // 方向光源
    if (glfwGetKey(wind, GLFW_KEY_L) == GLFW_PRESS) {
        static double lstL = 0;
        double now = glfwGetTime();
        if (now - lstL > 0.2) {
            Lighting::getDefault()->setDir(!Lighting::getDefault()->isDir());
            lstL = now;
        }
    }
    // TODO 雪花材质
    if (glfwGetKey(wind, GLFW_KEY_T) == GLFW_PRESS) {
        static double lstT = 0;
        double now = glfwGetTime();
        if (now - lstT > 0.2) {
            //
            lstT = now;
        }
    }
    // 相机操作
    if (glfwGetKey(wind, GLFW_KEY_C) == GLFW_PRESS) {
        static double lstC = 0;
        double now = glfwGetTime();
        if (now - lstC > 0.2) {
            int mode = camera->changeMode();
            if (mode == Camera::MODE_DRAG) {
                INSTANCE->control.mouseCap = false;
                glfwSetInputMode(wind, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            } else if (mode == Camera::MODE_FPS) {
                INSTANCE->control.mouseCap = true;
                glfwSetInputMode(wind, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
            lstC = now;
        }
    }
    // 释放鼠标
    bool &mouseCap = INSTANCE->control.mouseCap;
    static double lstChangeMouse = 0;
    if (glfwGetKey(wind, GLFW_KEY_Z) == GLFW_PRESS) {
        double now = glfwGetTime();
        if (now - lstChangeMouse > 0.2) {
            lstChangeMouse = now;
            mouseCap = !mouseCap;
            if (camera->mode == Camera::MODE_DRAG)
                mouseCap = false;
            if (mouseCap) {
                glfwSetInputMode(wind, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            } else {
                glfwSetInputMode(wind, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
        }
    }
}

void Game::mouse_callback(GLFWwindow *wind, double xpos, double ypos) {
    bool &mouseCap = INSTANCE->control.mouseCap;
    static bool firstMouse = true;
    static float lastX = (float) INSTANCE->screen.width / 2;
    static float lastY = (float) INSTANCE->screen.height / 2;

    if (firstMouse) {
        lastX = (float) xpos;
        lastY = (float) ypos;
        firstMouse = false;
    }

    float xoffset = (float) xpos - lastX;
    auto yoffset = (float) (lastY - ypos);
    lastX = (float) xpos;
    lastY = (float) ypos;

    auto *camera = INSTANCE->stage->getCamera();
    if (camera->mode == Camera::MODE_DRAG && glfwGetMouseButton(INSTANCE->window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS
        || camera->mode == Camera::MODE_FPS && mouseCap) {
        INSTANCE->stage->getCamera()->processMouseMovement(xoffset, yoffset);
    }
}

void Game::mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            static double lstClick = 0;
            double now = glfwGetTime();
            if (now - lstClick > 0.2) {
                // 生成雪花
                double xpos, ypos;
                glfwGetCursorPos(INSTANCE->window, &xpos, &ypos);
                Game::curStage()->getSnow()->spawnOnScreen(xpos, ypos);
                lstClick = now;
            }
        }
    }
}

void Game::scroll_callback(GLFWwindow *wind, double xoffset, double yoffset) {
    INSTANCE->stage->getCamera()->processMouseScroll((float) yoffset);
}

Stage *Game::curStage() {
    return Game::INSTANCE->stage;
}
