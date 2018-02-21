#include "openvr.h"

#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <SDL2\SDL.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

static inline glm::mat4 hmd_mat4(const vr::HmdMatrix44_t & m) noexcept {
      return glm::transpose(glm::make_mat4((float*)&m.m));
    }

static inline glm::mat4 hmd_mat3x4(const vr::HmdMatrix34_t & m) noexcept {
  return glm::mat4(
    m.m[0][0], m.m[1][0], m.m[2][0], 0.0,
    m.m[0][1], m.m[1][1], m.m[2][1], 0.0,
    m.m[0][2], m.m[1][2], m.m[2][2], 0.0,
    m.m[0][3], m.m[1][3], m.m[2][3], 1.0f
  );
}

openvr::openvr()
{
    // set all up
	p_unRenderModelProgramID = 0;
	p_nRenderModelMatrixLocation = -1;

	p_hmd = NULL;

	p_fNearClip = 0.1f;
 	p_fFarClip = 30.0f;

 	p_iTrackedControllerCount = 0;
 	p_iTrackedControllerCount_Last = -1;
 	p_iValidPoseCount = 0;
 	p_iValidPoseCount_Last = -1;

 	p_strPoseClasses = "";

 	memset(p_rDevClassChar, 0, sizeof(p_rDevClassChar));


    // Loading the SteamVR Runtime
    vr::EVRInitError eError = vr::VRInitError_None;

    p_hmd = vr::VR_Init( &eError, vr::VRApplication_Scene );

    if ( eError != vr::VRInitError_None )
	{
		p_hmd = NULL;
		char buf[1024];
		snprintf( buf, sizeof( buf ), "Unable to init VR runtime: %s", vr::VR_GetVRInitErrorAsEnglishDescription( eError ) );
		SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "VR_Init Failed", buf, NULL );
		return;
	}

	// controll shader
    p_unRenderModelProgramID = compileGLShader(
		"render model",

		// vertex shader
		"#version 410\n"
		"uniform mat4 matrix;\n"
		"layout(location = 0) in vec4 position;\n"
		"layout(location = 1) in vec3 v3NormalIn;\n"
		"layout(location = 2) in vec2 v2TexCoordsIn;\n"
		"out vec2 v2TexCoord;\n"
		"void main()\n"
		"{\n"
		"	v2TexCoord = v2TexCoordsIn;\n"
		"	gl_Position = matrix * vec4(position.xyz, 1);\n"
		"}\n",

		//fragment shader
		"#version 410 core\n"
		"uniform sampler2D diffuse;\n"
		"in vec2 v2TexCoord;\n"
		"out vec4 outputColor;\n"
		"void main()\n"
		"{\n"
		"   outputColor = texture( diffuse, v2TexCoord);\n"
		"}\n"
		);
	p_nRenderModelMatrixLocation = glGetUniformLocation( p_unRenderModelProgramID, "matrix" );
	if( p_nRenderModelMatrixLocation == -1 )
	{
		printf( "Unable to find matrix uniform in render model shader\n" );
		return;
	}

	p_pRenderModels = (vr::IVRRenderModels *)vr::VR_GetGenericInterface( vr::IVRRenderModels_Version, &eError );
	if( !p_pRenderModels )
	{
		p_hmd = NULL;
		vr::VR_Shutdown();

		char buf[1024];
		snprintf( buf, sizeof( buf ), "Unable to get render model interface: %s", vr::VR_GetVRInitErrorAsEnglishDescription( eError ) );
		SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "VR_Init Failed", buf, NULL );
		return;
	}
	glGetError();

	// setup opengl
	setupCameras();
	setupStereoRenderTargets();
	setupRenderModels();

	// setup vr compositor
    vr::EVRInitError peError = vr::VRInitError_None;

	if ( !vr::VRCompositor() )
	{
		printf( "Compositor initialization failed. See log file for details\n" );
	}
}

openvr::~openvr()
{
    if( p_hmd )
	{
		vr::VR_Shutdown();
		p_hmd = NULL;
	}

    for( std::vector< openvr_models * >::iterator i = p_vecRenderModels.begin(); i != p_vecRenderModels.end(); i++ )
	{
		delete (*i);
	}
	p_vecRenderModels.clear();

    if ( p_unRenderModelProgramID )
        glDeleteProgram( p_unRenderModelProgramID );
}

bool openvr::setupStereoRenderTargets()
{
	if ( !p_hmd )
		return false;

	p_hmd->GetRecommendedRenderTargetSize( &p_nRenderWidth, &p_nRenderHeight );

	// creates a frame buffer for both eys
	createFrameBuffer( p_nRenderWidth, p_nRenderHeight, leftEyeDesc );
	createFrameBuffer( p_nRenderWidth, p_nRenderHeight, rightEyeDesc );

	return true;
}

void openvr::setupCameras()
{
	p_mat4ProjectionLeft = getHMDMatrixProjectionEye( vr::Eye_Left );
	p_mat4ProjectionRight = getHMDMatrixProjectionEye( vr::Eye_Right );
	p_mat4eyePosLeft = getHMDMatrixPoseEye( vr::Eye_Left );
	p_mat4eyePosRight = getHMDMatrixPoseEye( vr::Eye_Right );
}

bool openvr::createFrameBuffer( int nWidth, int nHeight, openvr_framebufferDesc &framebufferDesc )
{
	glGenFramebuffers(1, &framebufferDesc.m_nRenderFramebufferId );
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferDesc.m_nRenderFramebufferId);

	glGenRenderbuffers(1, &framebufferDesc.m_nDepthBufferId);
	glBindRenderbuffer(GL_RENDERBUFFER, framebufferDesc.m_nDepthBufferId);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT, nWidth, nHeight );
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,	framebufferDesc.m_nDepthBufferId );

	glGenTextures(1, &framebufferDesc.m_nRenderTextureId );
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, framebufferDesc.m_nRenderTextureId );
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, nWidth, nHeight, true);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, framebufferDesc.m_nRenderTextureId, 0);

	glGenFramebuffers(1, &framebufferDesc.m_nResolveFramebufferId );
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferDesc.m_nResolveFramebufferId);

	glGenTextures(1, &framebufferDesc.m_nResolveTextureId );
	glBindTexture(GL_TEXTURE_2D, framebufferDesc.m_nResolveTextureId );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, nWidth, nHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferDesc.m_nResolveTextureId, 0);

	// check FBO status
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		return false;
	}

	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	return true;
}

void openvr::setupRenderModels()
{
	memset( p_rTrackedDeviceToRenderModel, 0, sizeof( p_rTrackedDeviceToRenderModel ) );

	if( !p_hmd )
		return;

	for( uint32_t unTrackedDevice = vr::k_unTrackedDeviceIndex_Hmd + 1; unTrackedDevice < vr::k_unMaxTrackedDeviceCount; unTrackedDevice++ )
	{
		if( !p_hmd->IsTrackedDeviceConnected( unTrackedDevice ) )
			continue;

		setupRenderModelForTrackedDevice( unTrackedDevice );
	}
}

void openvr::setupRenderModelForTrackedDevice( vr::TrackedDeviceIndex_t unTrackedDeviceIndex )
{
	if( unTrackedDeviceIndex >= vr::k_unMaxTrackedDeviceCount )
		return;

	// try to find a model we've already set up
	std::string sRenderModelName = getTrackedDeviceString( p_hmd, unTrackedDeviceIndex, vr::Prop_RenderModelName_String );
	openvr_models *pRenderModel = findOrLoadRenderModel( sRenderModelName.c_str() );
	if( !pRenderModel )
	{
		std::string sTrackingSystemName = getTrackedDeviceString( p_hmd, unTrackedDeviceIndex, vr::Prop_TrackingSystemName_String );
		printf( "Unable to load render model for tracked device %d (%s.%s)", unTrackedDeviceIndex, sTrackingSystemName.c_str(), sRenderModelName.c_str() );
	}
	else
	{
		p_rTrackedDeviceToRenderModel[ unTrackedDeviceIndex ] = pRenderModel;
		p_rbShowTrackedDevice[ unTrackedDeviceIndex ] = true;
	}
}

std::string openvr::getTrackedDeviceString( vr::IVRSystem *pHmd, vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop, vr::TrackedPropertyError *peError )
{
	uint32_t unRequiredBufferLen = pHmd->GetStringTrackedDeviceProperty( unDevice, prop, NULL, 0, peError );
	if( unRequiredBufferLen == 0 )
		return "";

	char *pchBuffer = new char[ unRequiredBufferLen ];
	unRequiredBufferLen = pHmd->GetStringTrackedDeviceProperty( unDevice, prop, pchBuffer, unRequiredBufferLen, peError );
	std::string sResult = pchBuffer;
	delete [] pchBuffer;
	return sResult;
}

openvr_models *openvr::findOrLoadRenderModel( const char *pchRenderModelName )
{
	openvr_models *pRenderModel = NULL;
	for( std::vector< openvr_models * >::iterator i = p_vecRenderModels.begin(); i != p_vecRenderModels.end(); i++ )
	{
		if( !strcmp ( (*i)->getName().c_str(), pchRenderModelName ) )
		{
			pRenderModel = *i;
			break;
		}
	}

	// load the model if we didn't find one
	if( !pRenderModel )
	{
		vr::RenderModel_t *pModel;
		vr::EVRRenderModelError error;
		while ( 1 )
		{
			error = vr::VRRenderModels()->LoadRenderModel_Async( pchRenderModelName, &pModel );
			if ( error != vr::VRRenderModelError_Loading )
				break;

			SDL_Delay( 1);
		}

		if ( error != vr::VRRenderModelError_None )
		{
			printf( "Unable to load render model %s - %s\n", pchRenderModelName, vr::VRRenderModels()->GetRenderModelErrorNameFromEnum( error ) );
			return NULL; // move on to the next tracked device
		}

		vr::RenderModel_TextureMap_t *pTexture;
		while ( 1 )
		{
			error = vr::VRRenderModels()->LoadTexture_Async( pModel->diffuseTextureId, &pTexture );
			if ( error != vr::VRRenderModelError_Loading )
				break;

			SDL_Delay( 1 );
		}

		if ( error != vr::VRRenderModelError_None )
		{
			printf( "Unable to load render texture id:%d for render model %s\n", pModel->diffuseTextureId, pchRenderModelName );
			vr::VRRenderModels()->FreeRenderModel( pModel );
			return NULL; // move on to the next tracked device
		}

		pRenderModel = new openvr_models( pchRenderModelName );
		if ( !pRenderModel->init( *pModel, *pTexture ) )
		{
			printf( "Unable to create GL model from render model %s\n", pchRenderModelName );
			delete pRenderModel;
			pRenderModel = NULL;
		}
		else
		{
			p_vecRenderModels.push_back( pRenderModel );
		}
		vr::VRRenderModels()->FreeRenderModel( pModel );
		vr::VRRenderModels()->FreeTexture( pTexture );
	}
	return pRenderModel;
}


GLuint openvr::compileGLShader( const char *pchShaderName, const char *pchVertexShader, const char *pchFragmentShader )
{
	GLuint unProgramID = glCreateProgram();

	GLuint nSceneVertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource( nSceneVertexShader, 1, &pchVertexShader, NULL);
	glCompileShader( nSceneVertexShader );

	GLint vShaderCompiled = GL_FALSE;
	glGetShaderiv( nSceneVertexShader, GL_COMPILE_STATUS, &vShaderCompiled);
	if ( vShaderCompiled != GL_TRUE)
	{
		printf("%s - Unable to compile vertex shader %d!\n", pchShaderName, nSceneVertexShader);
		glDeleteProgram( unProgramID );
		glDeleteShader( nSceneVertexShader );
		return 0;
	}
	glAttachShader( unProgramID, nSceneVertexShader);
	glDeleteShader( nSceneVertexShader ); // the program hangs onto this once it's attached

	GLuint  nSceneFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource( nSceneFragmentShader, 1, &pchFragmentShader, NULL);
	glCompileShader( nSceneFragmentShader );

	GLint fShaderCompiled = GL_FALSE;
	glGetShaderiv( nSceneFragmentShader, GL_COMPILE_STATUS, &fShaderCompiled);
	if (fShaderCompiled != GL_TRUE)
	{
		printf("%s - Unable to compile fragment shader %d!\n", pchShaderName, nSceneFragmentShader );
		glDeleteProgram( unProgramID );
		glDeleteShader( nSceneFragmentShader );
		return 0;
	}

	glAttachShader( unProgramID, nSceneFragmentShader );
	glDeleteShader( nSceneFragmentShader ); // the program hangs onto this once it's attached

	glLinkProgram( unProgramID );

	GLint programSuccess = GL_TRUE;
	glGetProgramiv( unProgramID, GL_LINK_STATUS, &programSuccess);
	if ( programSuccess != GL_TRUE )
	{
		printf("%s - Error linking program %d!\n", pchShaderName, unProgramID);
		glDeleteProgram( unProgramID );
		return 0;
	}

	glUseProgram( unProgramID );
	glUseProgram( 0 );

	return unProgramID;
}

void openvr::updateHMDMatrixPose()
{
	if ( !p_hmd )
		return;

    // wait 11.1 ms -> 90hz
	vr::VRCompositor()->WaitGetPoses(p_rTrackedDevicePose, vr::k_unMaxTrackedDeviceCount, NULL, 0 );

	p_iValidPoseCount = 0;
	p_strPoseClasses = "";
	for ( int nDevice = 0; nDevice < vr::k_unMaxTrackedDeviceCount; ++nDevice )
	{
		if ( p_rTrackedDevicePose[nDevice].bPoseIsValid )
		{
			p_iValidPoseCount++;
			p_rmat4DevicePose[nDevice] = glm::inverse( glm::transpose(glm::mat4(glm::make_mat3x4((float*)p_rTrackedDevicePose[nDevice].mDeviceToAbsoluteTracking.m ))));
			if ( p_rDevClassChar[nDevice]==0)
			{
				switch (p_hmd->GetTrackedDeviceClass(nDevice))
				{
				case vr::TrackedDeviceClass_Controller:        p_rDevClassChar[nDevice] = 'C'; break;
				case vr::TrackedDeviceClass_HMD:               p_rDevClassChar[nDevice] = 'H'; break;
				case vr::TrackedDeviceClass_Invalid:           p_rDevClassChar[nDevice] = 'I'; break;
				case vr::TrackedDeviceClass_GenericTracker:    p_rDevClassChar[nDevice] = 'G'; break;
				case vr::TrackedDeviceClass_TrackingReference: p_rDevClassChar[nDevice] = 'T'; break;
				default:                                       p_rDevClassChar[nDevice] = '?'; break;
				}
			}
			p_strPoseClasses += p_rDevClassChar[nDevice];
		}
	}

	if ( p_rTrackedDevicePose[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid )
	{
		p_mat4HMDPose = p_rmat4DevicePose[vr::k_unTrackedDeviceIndex_Hmd];
		//p_mat4HMDPose.invert();
	}
}

void openvr::renderForLeftEye() {
    glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
	glEnable( GL_MULTISAMPLE );

	// Left Eye
	glBindFramebuffer( GL_FRAMEBUFFER, leftEyeDesc.m_nRenderFramebufferId );
 	glViewport(0, 0, p_nRenderWidth, p_nRenderHeight );

 	// now back
 	//RenderScene( vr::Eye_Left );
 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

}

void openvr::renderEndLeftEye() {
    // finish left eye
 	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	glDisable( GL_MULTISAMPLE );

 	glBindFramebuffer(GL_READ_FRAMEBUFFER, leftEyeDesc.m_nRenderFramebufferId);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, leftEyeDesc.m_nResolveFramebufferId );

    glBlitFramebuffer( 0, 0, p_nRenderWidth, p_nRenderHeight, 0, 0, p_nRenderWidth, p_nRenderHeight,
		GL_COLOR_BUFFER_BIT,
 		GL_LINEAR );

 	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0 );

    glEnable( GL_MULTISAMPLE );
}

void openvr::renderForRightEye() {
    // Right Eye
	glBindFramebuffer( GL_FRAMEBUFFER, rightEyeDesc.m_nRenderFramebufferId );
 	glViewport(0, 0, p_nRenderWidth, p_nRenderHeight );

 	// back
 	//RenderScene( vr::Eye_Right );
 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
}

void openvr::renderEndRightEye() {
 	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	glDisable( GL_MULTISAMPLE );

 	glBindFramebuffer(GL_READ_FRAMEBUFFER, rightEyeDesc.m_nRenderFramebufferId );
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, rightEyeDesc.m_nResolveFramebufferId );

    glBlitFramebuffer( 0, 0, p_nRenderWidth, p_nRenderHeight, 0, 0, p_nRenderWidth, p_nRenderHeight,
		GL_COLOR_BUFFER_BIT,
 		GL_LINEAR  );

 	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0 );
}

void openvr::renderModels( glm::mat4 matMVP)
{
	// ----- Render Model rendering -----
	glUseProgram( p_unRenderModelProgramID );

	bool bIsInputAvailable = p_hmd->IsInputAvailable();

	for( uint32_t unTrackedDevice = 0; unTrackedDevice < vr::k_unMaxTrackedDeviceCount; unTrackedDevice++ )
	{
		if( !p_rTrackedDeviceToRenderModel[ unTrackedDevice ] || !p_rbShowTrackedDevice[ unTrackedDevice ] )
			continue;

		const vr::TrackedDevicePose_t & pose = p_rTrackedDevicePose[ unTrackedDevice ];
		if( !pose.bPoseIsValid )
			continue;

		if( !bIsInputAvailable && p_hmd->GetTrackedDeviceClass( unTrackedDevice ) == vr::TrackedDeviceClass_Controller )
			continue;

		glm::mat4 matDeviceToTracking = p_rmat4DevicePose[ unTrackedDevice ];
		glm::mat4 matMVP = matMVP * matDeviceToTracking;
		glUniformMatrix4fv( p_nRenderModelMatrixLocation, 1, GL_FALSE, &matMVP[0][0] );

		p_rTrackedDeviceToRenderModel[ unTrackedDevice ]->draw();
	}


	glUseProgram( 0 );

}

void openvr::renderFrame()
{
    // Process SteamVR events
	vr::VREvent_t event;
	while( p_hmd->PollNextEvent( &event, sizeof( event ) ) )
	{
		//ProcessVREvent( event );
	}

	// Process SteamVR controller state
	for( vr::TrackedDeviceIndex_t unDevice = 0; unDevice < vr::k_unMaxTrackedDeviceCount; unDevice++ )
	{
		vr::VRControllerState_t state;
		if( p_hmd->GetControllerState( unDevice, &state, sizeof(state) ) )
		{
			p_rbShowTrackedDevice[ unDevice ] = state.ulButtonPressed == 0;
		}
	}

    // for now as fast as possible
	if ( p_hmd )
	{
		//RenderControllerAxes(); xy thing
		//RenderStereoTargets();
		//RenderCompanionWindow();

		vr::Texture_t leftEyeTexture = {(void*)(uintptr_t)leftEyeDesc.m_nResolveTextureId, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
		vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture );
		vr::Texture_t rightEyeTexture = {(void*)(uintptr_t)rightEyeDesc.m_nResolveTextureId, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
		vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture );
	}

	// Spew out the controller and pose count whenever they change.
	if ( p_iTrackedControllerCount != p_iTrackedControllerCount_Last || p_iValidPoseCount != p_iValidPoseCount_Last )
	{
		p_iValidPoseCount_Last = p_iValidPoseCount;
		p_iTrackedControllerCount_Last = p_iTrackedControllerCount;

		printf( "PoseCount:%d(%s) Controllers:%d\n", p_iValidPoseCount, p_strPoseClasses.c_str(), p_iTrackedControllerCount );
	}

	updateHMDMatrixPose();
}

glm::mat4 openvr::getCurrentViewProjectionMatrix( vr::Hmd_Eye nEye )
{
	glm::mat4 matMVP;
	if( nEye == vr::Eye_Left )
	{
		matMVP = p_mat4ProjectionLeft * p_mat4eyePosLeft * p_mat4HMDPose;
	}
	else if( nEye == vr::Eye_Right )
	{
		matMVP = p_mat4ProjectionRight * p_mat4eyePosRight *  p_mat4HMDPose;
	}

	return matMVP;
}

glm::mat4 openvr::getHMDMatrixProjectionEye( vr::Hmd_Eye nEye )
{
	if ( !p_hmd )
		return glm::mat4();

	vr::HmdMatrix44_t mat = p_hmd->GetProjectionMatrix( nEye, p_fNearClip, p_fFarClip );

	return hmd_mat4( mat);
}

glm::mat4 openvr::getHMDMatrixPoseEye( vr::Hmd_Eye nEye )
{
	if ( !p_hmd )
		return glm::mat4();

	vr::HmdMatrix34_t matEyeRight = p_hmd->GetEyeToHeadTransform( nEye );


	glm::mat4 matrixObj = hmd_mat3x4(matEyeRight);

	return matrixObj;
}
