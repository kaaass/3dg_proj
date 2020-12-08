#ifndef INC_3DG_PROJ_TEXT_H
#define INC_3DG_PROJ_TEXT_H

#include <vector>
#include <string>
#include <shader.h>

#include <object/text/freetype-gl.h>

class Text {

    std::vector<float> buffer;
    std::vector<uint> indices;
    std::vector<std::string> texts;

    texture_font_t *font;
    texture_atlas_t *atlas;

    Shader *shader = nullptr;

    uint VBO;
    uint VAO;

public:

    void init();

    void idle(float delta);

    void draw();

    virtual ~Text() {
        delete font;
        delete atlas;
    }

private:
    float addLine(float height, const std::string &text);

    void updateText(float delta);
};


#endif //INC_3DG_PROJ_TEXT_H
