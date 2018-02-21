#ifndef OPENVR_RENDERMODEL_H
#define OPENVR_RENDERMODEL_H

#include <string>
#include <openvr.h>
#include <GL/glew.h>

class openvr_rendermodel
{
    public:
        openvr_rendermodel( const std::string & sRenderModelName);
        virtual ~openvr_rendermodel();

        bool init( const vr::RenderModel_t & vrModel, const vr::RenderModel_TextureMap_t & vrDiffuseTexture );

        void cleanup();
        void draw();
        const std::string & getName() const { return m_modelName; }
    protected:

    private:
        GLuint p_glVertBuffer;
        GLuint p_glIndexBuffer;
        GLuint p_glVertArray;
        GLuint p_glTexture;
        GLsizei p_unVertexCount;
        std::string m_modelName;
};

#endif // OPENVR_RENDERMODEL_H
