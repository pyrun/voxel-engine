#ifndef DEBUG_DRAW_H
#define DEBUG_DRAW_H

#include <bounce/bounce.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "graphic.h"

class debug_draw : public b3Draw {
    public:
        debug_draw();
        ~debug_draw();

        void setVec3( glm::vec3 *vec, const b3Vec3 &vecValue) {
            vec->x = vecValue.x;
            vec->y = vecValue.y;
            vec->z = vecValue.z;
        }

        void drawHull(const b3HullShape* s, const b3Color& c, const b3Transform& xf);
        void drawMesh(const b3MeshShape* s, const b3Color& c, const b3Transform& xf);

        void drawShape(const b3Shape* s, const b3Color& c, const b3Transform& xf);

        void draw( const b3World& world, glm::mat4 viewmatrix, Shader *shader);

        void DrawPoint(const b3Vec3& p, float32 size, const b3Color& color) {}

        void DrawSegment(const b3Vec3& p1, const b3Vec3& p2, const b3Color& color) {}

        void DrawTriangle(const b3Vec3& p1, const b3Vec3& p2, const b3Vec3& p3, const b3Color& color) {
            DrawSolidTriangle( b3Vec3( 0, 0, 0), p1, p2, p3, color);
        }

        void DrawSolidTriangle(const b3Vec3& normal, const b3Vec3& p1, const b3Vec3& p2, const b3Vec3& p3, const b3Color& color) {
            setVec3( &p_vertex[p_vector_size+0], p1);
            setVec3( &p_vertex[p_vector_size+1], p2);
            setVec3( &p_vertex[p_vector_size+2], p3);

            setVec3( &p_normal[p_vector_size+0], normal);
            setVec3( &p_normal[p_vector_size+1], normal);
            setVec3( &p_normal[p_vector_size+2], normal);

            p_vector_size += 3;
            p_change = true;
        }

        void DrawPolygon(const b3Vec3* vertices, u32 count, const b3Color& color) {}

        void DrawSolidPolygon(const b3Vec3& normal, const b3Vec3* vertices, u32 count, const b3Color& color) {}

        void DrawCircle(const b3Vec3& normal, const b3Vec3& center, float32 radius, const b3Color& color) {}

        void DrawSolidCircle(const b3Vec3& normal, const b3Vec3& center, float32 radius, const b3Color& color) {}

        void DrawSphere(const b3Vec3& center, float32 radius, const b3Color& color) {}

        void DrawSolidSphere(const b3Vec3& center, float32 radius, const b3Color& color) {}

        void DrawCapsule(const b3Vec3& p1, const b3Vec3& p2, float32 radius, const b3Color& color) {}

        void DrawSolidCapsule(const b3Vec3& p1, const b3Vec3& p2, float32 radius, const b3Color& color) {}

        void DrawAABB(const b3AABB3& aabb, const b3Color& color) {}

        void DrawTransform(const b3Transform& xf) {}
        //
        void DrawString(const char* string, const b3Color& color, ...) {}

        void DrawSphere(const b3SphereShape* s, const b3Color& c, const b3Transform& xf) {}

        void DrawCapsule(const b3CapsuleShape* s, const b3Color& c, const b3Transform& xf) {}

        void DrawHull(const b3HullShape* s, const b3Color& c, const b3Transform& xf) {}

        void DrawMesh(const b3MeshShape* s, const b3Color& c, const b3Transform& xf) {}

    private:
        bool p_change;

        //int p_vector_length;
        int p_vector_size;

        glm::vec3 p_color_pen;
        glm::vec3 p_scale;
        glm::vec3 p_normal_pen;

        std::vector<glm::vec3> p_vertex;
        std::vector<glm::vec3> p_normal;

        GLuint p_vbo_vertex;
        GLuint p_vbo_normal;
        GLuint p_vao;

        GLuint p_mvp;
};

#endif // DEBUG_DRAW_H
