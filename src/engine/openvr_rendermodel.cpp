#include "openvr_rendermodel.h"

openvr_rendermodel::openvr_rendermodel( const std::string & sRenderModelName )
	: m_modelName( sRenderModelName )
{
	p_glIndexBuffer = 0;
	p_glVertArray = 0;
	p_glVertBuffer = 0;
	p_glTexture = 0;
}

openvr_rendermodel::~openvr_rendermodel()
{
    cleanup();
}

bool openvr_rendermodel::init( const vr::RenderModel_t & vrModel, const vr::RenderModel_TextureMap_t & vrDiffuseTexture )
{
	// create and bind a VAO to hold state for this model
	glGenVertexArrays( 1, &p_glVertArray );
	glBindVertexArray( p_glVertArray );

	// Populate a vertex buffer
	glGenBuffers( 1, &p_glVertBuffer );
	glBindBuffer( GL_ARRAY_BUFFER, p_glVertBuffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof( vr::RenderModel_Vertex_t ) * vrModel.unVertexCount, vrModel.rVertexData, GL_STATIC_DRAW );

	// Identify the components in the vertex buffer
	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( vr::RenderModel_Vertex_t ), (void *)offsetof( vr::RenderModel_Vertex_t, vPosition ) );
	glEnableVertexAttribArray( 1 );
	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof( vr::RenderModel_Vertex_t ), (void *)offsetof( vr::RenderModel_Vertex_t, vNormal ) );
	glEnableVertexAttribArray( 2 );
	glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, sizeof( vr::RenderModel_Vertex_t ), (void *)offsetof( vr::RenderModel_Vertex_t, rfTextureCoord ) );

	// Create and populate the index buffer
	glGenBuffers( 1, &p_glIndexBuffer );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, p_glIndexBuffer );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( uint16_t ) * vrModel.unTriangleCount * 3, vrModel.rIndexData, GL_STATIC_DRAW );

	glBindVertexArray( 0 );

	// create and populate the texture
	glGenTextures(1, &p_glTexture );
	glBindTexture( GL_TEXTURE_2D, p_glTexture );

	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, vrDiffuseTexture.unWidth, vrDiffuseTexture.unHeight,
		0, GL_RGBA, GL_UNSIGNED_BYTE, vrDiffuseTexture.rubTextureMapData );

	// If this renders black ask McJohn what's wrong.
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );

	GLfloat fLargest;
	glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest );

	glBindTexture( GL_TEXTURE_2D, 0 );

	p_unVertexCount = vrModel.unTriangleCount * 3;

	return true;
}

void openvr_rendermodel::cleanup()
{
	if( p_glVertBuffer )
	{
		glDeleteBuffers(1, &p_glIndexBuffer);
		glDeleteVertexArrays( 1, &p_glVertArray );
		glDeleteBuffers(1, &p_glVertBuffer);
		p_glIndexBuffer = 0;
		p_glVertArray = 0;
		p_glVertBuffer = 0;
	}
}

void openvr_rendermodel::draw()
{
	glBindVertexArray( p_glVertArray );

	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, p_glTexture );

	glDrawElements( GL_TRIANGLES, p_unVertexCount, GL_UNSIGNED_SHORT, 0 );

	glBindVertexArray( 0 );
}
