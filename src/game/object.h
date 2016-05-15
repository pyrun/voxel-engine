#ifndef OBJECT_H
#define OBJECT_H

#include "../graphic/graphic.h"
#include "../graphic/tiny_obj_loader.h"
#include "../graphic/texture.h"

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

            // x -
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








            // v+
            /*vertices.push_back( glm::vec3( 0.0f, 0.0f, i_depth) );
            vertices.push_back( glm::vec3( i_width, i_hight, i_depth) );
            vertices.push_back( glm::vec3( 0.0f, i_hight, i_depth) );
            vertices.push_back( glm::vec3( i_width, 0.0f, i_depth) );
            vertices.push_back( glm::vec3( i_width, i_hight, i_depth) );
            vertices.push_back( glm::vec3( 0.0f, 0.0f, i_depth) );
            // z -
            vertices.push_back( glm::vec3( 0.0f, 0.0f, 0.0f) );
            vertices.push_back( glm::vec3( 0.0f, i_hight, i_depth) );
            vertices.push_back( glm::vec3( 0.0f, i_hight, 0.0f) );
            vertices.push_back( glm::vec3( 0.0f, i_hight, i_depth) );
            vertices.push_back( glm::vec3( 0.0f, 0.0f, 0.0f) );
            vertices.push_back( glm::vec3( 0.0f, 0.0f, i_depth) );
            // x -
            texcoords.push_back( glm::vec2( 1.0f, 1.0f) );
            texcoords.push_back( glm::vec2( 1.0f, 0.0f) );
            texcoords.push_back( glm::vec2( 0.0f, 0.0f) );
            texcoords.push_back( glm::vec2( 0.0f, 0.0f) );
            texcoords.push_back( glm::vec2( 0.0f, 1.0f) );
            texcoords.push_back( glm::vec2( 1.0f, 1.0f) );
            // x-
            texcoords.push_back( glm::vec2( 0.0f, 1.0f) );
            texcoords.push_back( glm::vec2( 1.0f, 0.0f) );
            texcoords.push_back( glm::vec2( 0.0f, 0.0f) );
            texcoords.push_back( glm::vec2( 1.0f, 1.0f) );
            texcoords.push_back( glm::vec2( 1.0f, 0.0f) );
            texcoords.push_back( glm::vec2( 0.0f, 1.0f) );
            // z -
            texcoords.push_back( glm::vec2( 0.0f, 1.0f) );
            texcoords.push_back( glm::vec2( 1.0f, 0.0f) );
            texcoords.push_back( glm::vec2( 0.0f, 0.0f) );
            texcoords.push_back( glm::vec2( 1.0f, 0.0f) );
            texcoords.push_back( glm::vec2( 0.0f, 1.0f) );
            texcoords.push_back( glm::vec2( 1.0f, 1.0f) );*/

            /*texcoords.push_back( glm::vec2( 1.0f, 0.0f) );
            texcoords.push_back( glm::vec2( 0.0f, 1.0f) );
            texcoords.push_back( glm::vec2( 1.0f, 1.0f) );

            texcoords.push_back( glm::vec2( 1.0f, 0.0f) );
            texcoords.push_back( glm::vec2( 0.0f, 0.0f) );
            texcoords.push_back( glm::vec2( 0.0f, 1.0f) );*/


            //vertices.push_back( glm::vec3( 0.0f, 1.0f, 0.0f) );
            m_texture = new Texture( "cube.bmp");

            //vertices.push_back( glm:vec3 v( 0, 1, 1) );

            /*
            // Read our .obj file
            std::vector< glm::vec3 > vertices;
            std::vector< glm::vec2 > uvs;
            std::vector< glm::vec3 > normals; // Won't be used at the moment.

            //bool res = loadOBJ("untitled.obj", vertices, uvs, normals);
            std::vector<tinyobj::shape_t> shapes;
            std::vector<tinyobj::material_t> materials;
            std::string test;
            tinyobj::LoadObj( shapes, materials, test, "cube.obj", NULL, true);
            m_texture = new Texture( "cube.png");*/


            /*for (size_t i = 0; i < shapes.size(); i++) {
              printf("shape[%ld].name = %s\n", i, shapes[i].name.c_str());
              printf("Size of shape[%ld].indices: %ld\n", i, shapes[i].mesh.indices.size());
              printf("Size of shape[%ld].material_ids: %ld\n", i, shapes[i].mesh.material_ids.size());
              assert((shapes[i].mesh.indices.size() % 3) == 0);
              for (size_t f = 0; f < shapes[i].mesh.indices.size() / 3; f++) {
                printf("  idx[%ld] = %d, %d, %d. mat_id = %d\n", f, shapes[i].mesh.indices[3*f+0], shapes[i].mesh.indices[3*f+1], shapes[i].mesh.indices[3*f+2], shapes[i].mesh.material_ids[f]);
              }

              printf("shape[%ld].vertices: %ld\n", i, shapes[i].mesh.positions.size());
              assert((shapes[i].mesh.positions.size() % 3) == 0);
              for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
                printf("  v[%ld] = (%f, %f, %f)\n", v,
                  shapes[i].mesh.positions[3*v+0],
                  shapes[i].mesh.positions[3*v+1],
                  shapes[i].mesh.positions[3*v+2]);
              }
            }*/





            glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof( glm::vec3 ), &vertices[0], GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, m_vboTexture);
            glBufferData(GL_ARRAY_BUFFER, texcoords.size() * sizeof( glm::vec2 ), &texcoords[0], GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIndex);
            glBufferData( GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof( unsigned int ), &indices[0], GL_STATIC_DRAW);
            size = indices.size();
        }

        void draw( Shader* l_shader, Camera* l_camera) {
            Transform f_form;
            f_form.GetPos().x = m_pos.x;
            f_form.GetPos().y = m_pos.y;
            f_form.GetPos().z = m_pos.z;

            Shader *shader = l_shader;

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIndex);
            shader->BindArray( m_vbo, 0, GL_FLOAT, 3);
            shader->BindArray( m_vboTexture, 1, GL_FLOAT, 2);


            shader->Bind();
            shader->EnableVertexArray( 0);
            shader->EnableVertexArray( 1);

            m_texture->Bind();

            //shader->SetSize( (graphic->GetDisplay()->GetTilesetHeight()/16), ( graphic->GetDisplay()->GetTilesetWidth()/16) );
            shader->Update( f_form, l_camera, l_camera, glm::mat4(1));


            //shader->BindArray( m_vbo, 0, GL_FLOAT);
            //glDrawArrays( GL_TRIANGLES, 0, size);

            glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, 0);
            //    shader->EnableVertexArray( 1);

            //glDrawElements( GL_TRIANGLES, size, GL_UNSIGNED_INT, 0);
        }

    protected:
    private:
        GLuint m_vbo;
        GLuint m_vboIndex;
        GLuint m_vboTexture;
        glm::vec3 m_pos;
        glm::vec3 m_view;
        int size;
        Texture *m_texture;
};

#endif // OBJECT_H
