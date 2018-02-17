#ifndef GUI_H
#define GUI_H

#include "../graphic/graphic.h"

class Gui {
public:
    Gui() {
        glGenBuffers(1, &p_vbo);
    }

    ~Gui() {
        glDeleteBuffers(1, &p_vbo);
    }

    void SetBox( Graphic *graphic, float Width, float Height) {


        float p_width = Width;

        float p_height = Height;

        float factor = (float)graphic->GetWidth()/graphic->GetHeight();

        p_height = p_height * factor;

        GLfloat box[6][4] = {
                {+    0.0, +p_height, 0, 13},
                {+p_width, +     0.0, 0, 13},
                {+p_width, +p_height, 0, 13},
                {+    0.0, +     0.0, 0, 13},
                {+p_width, +     0.0, 0, 13},
                {+    0.0, +p_height, 0, 13},
        };
        // VBO ändern
        glBindBuffer(GL_ARRAY_BUFFER, p_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);
    }

    void DrawBox( Graphic *graphic, float x, float y) {
        float p_width = graphic->GetWidth();
        float p_hight = graphic->GetHeight();

        glm::mat4 one = glm::ortho( (float)-(p_width/2.0f), p_width/2.0f, (float)-(p_hight/2.0f), p_hight/2.0f);
        graphic->GetVertexShader()->BindArray( p_vbo, 0, GL_FLOAT);
        graphic->GetVertexShader()->Bind();// Shader
        graphic->GetVertexShader()->EnableVertexArray( 0);
        Transform f_form;
        f_form.GetPos().x = x;
        f_form.GetPos().y = y;

        graphic->GetVertexShader()->BindArray( p_vbo, 0, GL_FLOAT);
        graphic->GetVertexShader()->Bind();// Shader
        graphic->GetVertexShader()->EnableVertexArray( 0);

        //graphic->GetVertexShader()->Update( f_form, graphic->GetCamera(), graphic->GetCamera());
        graphic->GetVertexShader()->UpdateWithout( one, graphic->GetCamera(), f_form);
        //p_graphic->GetVoxelShader()->UpdateWithout( one, p_graphic->GetCamera());
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    void Draw( Graphic *graphic) {
        //SetBox( graphic, 0.08, 0.08);
        //DrawBox ( graphic, 0.2, 0.2);

        //glDisable(GL_DEPTH_TEST);


    }
protected:
private:
    GLuint p_vbo;
};

#endif // GUI_H
