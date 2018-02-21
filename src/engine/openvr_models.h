#ifndef OPENVR_MODELS_H
#define OPENVR_MODELS_H

#include <openvr.h>
#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>

class openvr_models
{
    public:
        openvr_models( const std::string & sRenderModelName );
        virtual ~openvr_models();

        bool init( const vr::RenderModel_t & vrModel, const vr::RenderModel_TextureMap_t & vrDiffuseTexture );
        void cleanup();
        void draw();
        const std::string & getName() const { return p_sModelName; }

    protected:

    private:
        GLuint p_glVertBuffer;
        GLuint p_glIndexBuffer;
        GLuint p_glVertArray;
        GLuint p_glTexture;
        GLsizei p_unVertexCount;
        std::string p_sModelName;
};

#endif // OPENVR_MODELS_H
