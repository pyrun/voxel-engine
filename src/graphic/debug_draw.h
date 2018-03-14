#ifndef DEBUG_DRAW_H
#define DEBUG_DRAW_H

#include "btBulletDynamicsCommon.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "graphic.h"

#include <LinearMath/btIDebugDraw.h>

/*class debug_draw : public btIDebugDraw {
    public:
        void init( graphic *graphic);

        void draw( glm::mat4 viewmatrix);

        virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);

        virtual void drawContactPoint(const btVector3 &, const btVector3 &, btScalar, int, const btVector3 &) {}
        virtual void reportErrorWarning(const char *) {}
        virtual void draw3dText(const btVector3 &, const char *) {}
        virtual void setDebugMode(int modus) {
            p_modus = modus;
        }
        int getDebugMode(void) const { return 3; }
    private:
        int p_modus;

        bool p_change;

        int p_vector_length;
        int p_vector_size;
        std::vector<btVector3> p_lines;
        std::vector<btVector3> p_color;

        Shader *p_shader;

        GLuint p_vbo_lines;
        GLuint p_vbo_color;
        GLuint p_vao;

        GLuint p_mvp;
};*/

#endif // DEBUG_DRAW_H
