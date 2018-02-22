#ifndef OPENVR_H
#define OPENVR_H

#include <stdio.h>
#include <openvr.h>
#include <string>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <sstream>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "openvr_models.h"

struct openvr_framebufferDesc
{
    GLuint m_nDepthBufferId;
    GLuint m_nRenderTextureId;
    GLuint m_nRenderFramebufferId;
    GLuint m_nResolveTextureId;
    GLuint m_nResolveFramebufferId;
};

class openvr
{
    public:
        openvr();
        virtual ~openvr();

        bool setupStereoRenderTargets();
        void setupCameras();
        void setupRenderModels();
        void setupRenderModelForTrackedDevice( vr::TrackedDeviceIndex_t unTrackedDeviceIndex );

        std::string getTrackedDeviceString( vr::IVRSystem *pHmd, vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop, vr::TrackedPropertyError *peError = NULL );
        openvr_models *findOrLoadRenderModel( const char *pchRenderModelName );

        bool createFrameBuffer( int nWidth, int nHeight, openvr_framebufferDesc &framebufferDesc );
        GLuint compileGLShader( const char *pchShaderName, const char *pchVertexShader, const char *pchFragmentShader );

        void updateHMDMatrixPose();

        void renderForLeftEye();
        glm::mat4 getViewProjectionMatrixLeft() { return getCurrentViewProjectionMatrix(vr::Eye_Left); }
        void renderEndLeftEye();

        void renderForRightEye();
        glm::mat4 getViewProjectionMatrixRight() { return getCurrentViewProjectionMatrix(vr::Eye_Right); }
        void renderEndRightEye();

        void renderModels( glm::mat4 matMVP);

        void renderFrame();

        void processVREvent( const vr::VREvent_t & event );

        glm::mat4 getCurrentViewProjectionMatrix( vr::Hmd_Eye nEye );
        glm::mat4 getHMDMatrixProjectionEye( vr::Hmd_Eye nEye );
        glm::mat4 getHMDMatrixPoseEye( vr::Hmd_Eye nEye );
    protected:

    private:
        float p_fNearClip;
        float p_fFarClip;

        int p_iTrackedControllerCount;
        int p_iTrackedControllerCount_Last;
        int p_iValidPoseCount;
        int p_iValidPoseCount_Last;

        GLuint p_unRenderModelProgramID; // shader for draw controller
        GLint p_nRenderModelMatrixLocation;

        std::string p_strPoseClasses;
        char p_rDevClassChar[ vr::k_unMaxTrackedDeviceCount ];

        vr::IVRSystem *p_hmd;
        vr::IVRRenderModels *p_pRenderModels;
        vr::TrackedDevicePose_t p_rTrackedDevicePose[ vr::k_unMaxTrackedDeviceCount ];
        glm::mat4 p_rmat4DevicePose[ vr::k_unMaxTrackedDeviceCount ];
        bool p_rbShowTrackedDevice[ vr::k_unMaxTrackedDeviceCount ];

        glm::mat4 p_mat4HMDPose;
        glm::mat4 p_mat4eyePosLeft;
        glm::mat4 p_mat4eyePosRight;

        glm::mat4 p_mat4ProjectionCenter;
        glm::mat4 p_mat4ProjectionLeft;
        glm::mat4 p_mat4ProjectionRight;

        openvr_framebufferDesc leftEyeDesc;
        openvr_framebufferDesc rightEyeDesc;

        uint32_t p_nRenderWidth;
        uint32_t p_nRenderHeight;

        std::vector< openvr_models* > p_vecRenderModels;
        openvr_models *p_rTrackedDeviceToRenderModel[ vr::k_unMaxTrackedDeviceCount ];
};

#endif // OPENVR_H
