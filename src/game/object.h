#ifndef OBJECT_H
#define OBJECT_H

#include "../graphic/graphic.h"
#include "../graphic/tiny_obj_loader.h"
#include "../graphic/texture.h"
#include "objectcreator.h"

class Object {
    public:
        Object();
        virtual ~Object();

        void Init() {
            std::vector<glm::vec3> vertices;
            std::vector<unsigned int> indices;
            std::vector< glm::vec2 > texcoords;

            float i_width = 0.5f;
            float i_hight = 1.0f;
            float i_depth = 1.0f;

            ObjectCreator Obj;

            //Obj.addXPlate();

//            Obj.addXPlate( glm::vec3( 0, 1 ,0), glm::vec3( 0.5, 0.5 ,0.5), glm::vec4( 0, 0, 0.5, 0.5));

            /*Obj.addXPlate( glm::vec3( 0, 0 ,0), glm::vec3( 1, 1 ,1), glm::vec4( 0, 0, 1, 1));
            Obj.addXPlate( glm::vec3( 0, 0 ,0), glm::vec3( 1, 1 ,1), glm::vec4( 0, 0, 1, 1), true);
            Obj.addZPlate( glm::vec3( 0, 0 ,0), glm::vec3( 1, 1 ,1), glm::vec4( 0, 0, 1, 1), true);
            Obj.addZPlate( glm::vec3( 0, 0 ,0), glm::vec3( 1, 1 ,1), glm::vec4( 0, 0, 1, 1), false);
            Obj.addYPlate( glm::vec3( 0, 0 ,0), glm::vec3( 1, 1 ,1), glm::vec4( 0, 0, 1, 1), true);
            Obj.addYPlate( glm::vec3( 0, 0 ,0), glm::vec3( 1, 1 ,1), glm::vec4( 0, 0, 1, 1), false);*/

            Obj.addCube( glm::vec3( 0, 2, 0), glm::vec3( 1, 2 ,1), glm::vec4( 0, 0, 1, 1));
            Obj.addCube( glm::vec3( 0, 0, 0), glm::vec3( 1, 1.5,1), glm::vec4( 0, 0, 1, 1));



            vertices = Obj.getVertices();
            indices = Obj.getIndices();
            texcoords = Obj.getTexcoords();
/*            // x -
            vertices.push_back( glm::vec3( 0.0f, 0.0f, 0.0f) ); // 0
            vertices.push_back( glm::vec3( 0.0f, i_hight, 0.0f) ); // 1
            vertices.push_back( glm::vec3( i_width, i_hight, 0.0f) ); // 2
            vertices.push_back( glm::vec3( i_width, 0.0f, 0.0f) ); // 3
            texcoords.push_back( glm::vec2( 1.0f, 1.0f) );
            texcoords.push_back( glm::vec2( 1.0f, 0.0f) );
            texcoords.push_back( glm::vec2( 0.0f, 0.0f) );
            texcoords.push_back( glm::vec2( 0.0f, 1.0f) );
            indices.push_back( 0 );
            indices.push_back( 1 );
            indices.push_back( 2 );
            indices.push_back( 0 );
            indices.push_back( 2 );
            indices.push_back( 3 );
            // x +
            vertices.push_back( glm::vec3( 0.0f, 0.0f, i_depth) ); // 4
            vertices.push_back( glm::vec3( 0.0f, i_hight, i_depth) ); // 5
            vertices.push_back( glm::vec3( i_width, i_hight, i_depth) ); // 6
            vertices.push_back( glm::vec3( i_width, 0.0f, i_depth) ); // 7
            texcoords.push_back( glm::vec2( 0.0f, 1.0f) );
            texcoords.push_back( glm::vec2( 0.0f, 0.0f) );
            texcoords.push_back( glm::vec2( 1.0f, 0.0f) );
            texcoords.push_back( glm::vec2( 1.0f, 1.0f) );
            indices.push_back( 6 ); 2
            indices.push_back( 5 );1
            indices.push_back( 4 );0
            indices.push_back( 7 );3
            indices.push_back( 6 );2
            indices.push_back( 4 );0
            // y -
            vertices.push_back( glm::vec3( 0.0f, 0.0f, i_depth) ); // 8
            vertices.push_back( glm::vec3( 0.0f, i_hight, i_depth) ); // 9
            vertices.push_back( glm::vec3( 0.0f, i_hight, 0.0f) ); // 10
            vertices.push_back( glm::vec3( 0.0f, 0.0f, 0.0f) ); // 11
            texcoords.push_back( glm::vec2( 1.0f, 1.0f) );
            texcoords.push_back( glm::vec2( 1.0f, 0.0f) );
            texcoords.push_back( glm::vec2( 0.0f, 0.0f) );
            texcoords.push_back( glm::vec2( 0.0f, 1.0f) );
            indices.push_back( 8 ); 0
            indices.push_back( 9 );1
            indices.push_back( 10 );2
            indices.push_back( 8 ); 0
            indices.push_back( 10 );2
            indices.push_back( 11 );3
            // y +
            vertices.push_back( glm::vec3( i_width, 0.0f, i_depth) ); // 12
            vertices.push_back( glm::vec3( i_width, i_hight, i_depth) ); // 13
            vertices.push_back( glm::vec3( i_width, i_hight, 0.0f) ); // 14
            vertices.push_back( glm::vec3( i_width, 0.0f, 0.0f) ); // 15
            texcoords.push_back( glm::vec2( 0.0f, 1.0f) );
            texcoords.push_back( glm::vec2( 0.0f, 0.0f) );
            texcoords.push_back( glm::vec2( 1.0f, 0.0f) );
            texcoords.push_back( glm::vec2( 1.0f, 1.0f) );
            indices.push_back( 14 );
            indices.push_back( 13 );
            indices.push_back( 12 );
            indices.push_back( 15 );
            indices.push_back( 14 );
            indices.push_back( 12 );
            // z -
            vertices.push_back( glm::vec3( 0.0f, 0.0f, i_depth) ); // 16
            vertices.push_back( glm::vec3( i_width, 0.0f, i_depth) ); // 17
            vertices.push_back( glm::vec3( i_width, 0.0f, 0.0f) ); // 18
            vertices.push_back( glm::vec3( 0.0f, 0.0f, 0.0f) ); // 19
            texcoords.push_back( glm::vec2( 1.0f, 0.0f) );
            texcoords.push_back( glm::vec2( 1.0f, 1.0f) );
            texcoords.push_back( glm::vec2( 0.0f, 1.0f) );
            texcoords.push_back( glm::vec2( 0.0f, 0.0f) );
            indices.push_back( 18 );
            indices.push_back( 17 );
            indices.push_back( 16 );
            indices.push_back( 19 );
            indices.push_back( 18 );
            indices.push_back( 16 );
            // z +
            vertices.push_back( glm::vec3( 0.0f, i_hight, i_depth) ); // 20
            vertices.push_back( glm::vec3( i_width, i_hight, i_depth) ); // 21
            vertices.push_back( glm::vec3( i_width, i_hight, 0.0f) ); // 22
            vertices.push_back( glm::vec3( 0.0f, i_hight, 0.0f) ); // 23
            texcoords.push_back( glm::vec2( 1.0f, 1.0f) );
            texcoords.push_back( glm::vec2( 1.0f, 0.0f) );
            texcoords.push_back( glm::vec2( 0.0f, 0.0f) );
            texcoords.push_back( glm::vec2( 0.0f, 1.0f) );
            indices.push_back( 20 );
            indices.push_back( 21 );
            indices.push_back( 22 );
            indices.push_back( 20 );
            indices.push_back( 22 );
            indices.push_back( 23 );*/

            /*indices.push_back( 4 );
            indices.push_back( 5 );
            indices.push_back( 0 );
            indices.push_back( 5 );
            indices.push_back( 1 );
            indices.push_back( 0 );
            // y +
            indices.push_back( 6 );
            indices.push_back( 7 );
            indices.push_back( 3 );
            indices.push_back( 3 );
            indices.push_back( 2 );
            indices.push_back( 6 );*/


            //vertices.push_back( glm::vec3( 0.0f, 1.0f, 0.0f) );0
            p_texture = new Texture( "cube.bmp");

            glBindBuffer(GL_ARRAY_BUFFER, p_vbo);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof( glm::vec3 ), &vertices[0], GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, p_vboTexture);
            glBufferData(GL_ARRAY_BUFFER, texcoords.size() * sizeof( glm::vec2 ), &texcoords[0], GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, p_vboIndex);
            glBufferData( GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof( unsigned int ), &indices[0], GL_STATIC_DRAW);
            size = indices.size();
        }

        void draw( Shader* l_shader, Camera* l_camera) {
            Transform f_form;
            f_form.GetPos().x = p_pos.x;
            f_form.GetPos().y = p_pos.y;
            f_form.GetPos().z = p_pos.z;

            Shader *shader = l_shader;

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, p_vboIndex);
            shader->BindArray( p_vbo, 0, GL_FLOAT, 3);
            shader->BindArray( p_vboTexture, 1, GL_FLOAT, 2);


            shader->Bind();
            shader->EnableVertexArray( 0);
            shader->EnableVertexArray( 1);

            p_texture->Bind();

            //shader->SetSize( (graphic->GetDisplay()->GetTilesetHeight()/16), ( graphic->GetDisplay()->GetTilesetWidth()/16) );
            shader->Update( f_form, l_camera, l_camera, glm::mat4(1));


            //shader->BindArray( p_vbo, 0, GL_FLOAT);
            //glDrawArrays( GL_TRIANGLES, 0, size);

            glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, 0);
            //    shader->EnableVertexArray( 1);

            //glDrawElements( GL_TRIANGLES, size, GL_UNSIGNED_INT, 0);
        }

    protected:
    private:
        GLuint p_vbo;
        GLuint p_vboIndex;
        GLuint p_vboTexture;
        glm::vec3 p_pos;
        glm::vec3 p_view;
        int size;
        Texture *p_texture;
};

#endif // OBJECT_H
