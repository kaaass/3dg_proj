#ifndef INC_3DG_PROJ_GAME_H
#define INC_3DG_PROJ_GAME_H

#include <GLFW/glfw3.h>
#include "stage.h"

/**
 * 全局控制类
 */
class Game {
public:
    struct Screen {
        int width = 1280;
        int height = 720;
    };

    struct Control {
        bool mouseCap = false;

        float deltaTime = 0.0f;
        float lastFrame = 0.0f;
    };

    Screen screen;
    Control control;

private:
    Stage *stage = nullptr;
    GLFWwindow *window = nullptr;

public:

    /**
     * 开始绘制
     */
    int start();

    /**
     * 主循环
     */
    void mainLoop();

    virtual ~Game();

private:

    /*
     * 回调
     */

    static void framebuffer_size_callback(GLFWwindow *wind, int width, int height);

    static void mouse_callback(GLFWwindow *wind, double xpos, double ypos);

    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

    static void scroll_callback(GLFWwindow *wind, double xoffset, double yoffset);

    static void processInput(GLFWwindow *wind);

private:

    Game() = default;

    static Game *INSTANCE;

public:

    static Game * getInstance();

    static Stage *curStage() ;
};

#endif //INC_3DG_PROJ_GAME_H
