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
            std::vector<glm::vec3> normal;
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

            // normal errechnen
            int v;
            normal.resize( vertices.size());
            for( v = 0; v+3 <= vertices.size(); v+=3) {
                glm::vec3 a(vertices[v].x, vertices[v].y, vertices[v].z);
                glm::vec3 b(vertices[v+1].x, vertices[v+1].y, vertices[v+1].z);
                glm::vec3 c(vertices[v+2].x, vertices[v+2].y, vertices[v+2].z);
                glm::vec3 edge1 = b-a;
                glm::vec3 edge2 = c-a;
                glm::vec3 l_normal = glm::normalize( glm::cross( edge1, edge2));

                normal[v] = l_normal;
                normal[v+1] = l_normal;
                normal[v+2] = l_normal;
            }


            //vertices.push_back( glm::vec3( 0.0f, 1.0f, 0.0f) );0
            p_texture = new texture( "cube.bmp");

            glBindBuffer(GL_ARRAY_BUFFER, p_vbo);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof( glm::vec3 ), &vertices[0], GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, p_vboNormal);
            glBufferData(GL_ARRAY_BUFFER, normal.size() * sizeof( glm::vec3 ), &normal[0], GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, p_vboTexture);
            glBufferData(GL_ARRAY_BUFFER, texcoords.size() * sizeof( glm::vec2 ), &texcoords[0], GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, p_vboIndex);
            glBufferData( GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof( unsigned int ), &indices[0], GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            size = indices.size();

            glBindVertexArray(p_vao);

            glEnableVertexAttribArray(0);  // Vertex position
            glEnableVertexAttribArray(1);  // Vertex normal
            glEnableVertexAttribArray(2);  // Vertex color

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, p_vboIndex);

            glBindBuffer(GL_ARRAY_BUFFER, p_vbo);
            glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL );
            glBindBuffer(GL_ARRAY_BUFFER, p_vboNormal);
            glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL );
            glBindBuffer(GL_ARRAY_BUFFER, p_vboTexture);
            glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL );

            glBindVertexArray(0);
        }

        void draw( Shader* l_shader, Camera* l_camera) {
            Transform f_form;
            f_form.setPos( p_pos );

            Shader *shader = l_shader;



            shader->Bind();

            glBindVertexArray( p_vao);

            p_texture->Bind();

            shader->update( &f_form, l_camera->getViewProjection());

            glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }

    protected:
    private:
        GLuint p_vao;
        GLuint p_vbo;
        GLuint p_vboNormal;
        GLuint p_vboIndex;
        GLuint p_vboTexture;
        glm::vec3 p_pos;
        glm::vec3 p_view;
        int size;
        texture *p_texture;
};

#endif // OBJECT_H
