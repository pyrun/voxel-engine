#include "md2.h"

#include <stdlib.h>
#include <string.h>

#include "../../game/objectcreator.h"

md2Model::md2Model() {
    // Buffer erzeugen
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_vboTexture);
    glGenBuffers(1, &m_vboIndex);

    // md2 buffer
	m_buffer	=NULL;
	m_texData	=NULL;
	m_triData	=NULL;
	m_vertData	=NULL;
	m_frameData	=NULL;
	m_glVertexData=NULL;
	m_glIndicesData=NULL;
	m_header.numFrames = 0;
	for (int index=0;index<m_header.numFrames;index++)
        m_frameData[index].pvertices = NULL;
}

md2Model::~md2Model() {
    // VBO löschen
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_vboTexture);
    glDeleteBuffers(1, &m_vboIndex);

    // dlete md2 data
	delete m_buffer;
	delete m_texData;
	delete m_triData;
	delete m_vertData;
	delete m_glVertexData;
	delete m_glIndicesData;
	for (int index=0;index<m_header.numFrames;index++)
        delete m_frameData[index].pvertices;
    delete m_frameData;
}

int md2Model::load(std::string fileName, std::string t_img) {
	int result=0;

	result=md2ReadFile(fileName.c_str());
	if (result!=0)
		return result;

	result=md2ReadHeader(m_buffer,&m_header);
	if (result!=0)
		return result;

	md2InitData();
	md2LoadData();
	md2ProcessData();

	setFrame(0); // Initially set it to Point to First Frame

	pmd2Header	phead= &m_header;
    printf( "numVertices %d\n", phead->numVertices);

    m_texture = new Texture( t_img);

	return 0;
}

void* md2Model::md2Malloc(size_t size) {
	void* p=malloc(size);
	if (p==NULL)
		return p;
	memset(p,0,size);

	return p;
}

int md2Model::md2ReadFile(const char* fileName) {
	FILE *fp;


	fp=fopen(fileName,"rb");
	if (!fp)
		return 1;

	//md2Free((void**)&m_buffer);
	delete m_buffer;

	long fileSize= md2FileSize(fp);
	if (fileSize<=0)
		return 1;

	m_buffer= new byte[fileSize];
	if (!m_buffer)
		return 1;

	if (fread(m_buffer,1,fileSize,fp)!=(size_t)fileSize)
		return 1;

	fclose(fp);
	return 0;
}

long md2Model::md2FileSize(FILE *fp) {
	long oldpos=ftell(fp);
	fseek(fp,0,SEEK_END);
	long curpos=ftell(fp);
	fseek(fp,oldpos,SEEK_SET);
	return curpos;
}

int md2Model::md2ReadHeader(byte *buffer,pmd2Header phead) {
	memcpy(phead,buffer,sizeof(*phead));

	if (phead->magic != MD2_MAGIC_NO)
		return 1;

	return 0;
}

void md2Model::md2InitData()
{
	int index=0;

	for (index=0;index<MAX_NO_SKIN;index++)
		memset(this->m_skinName[index],0,MAX_SKIN_NAME);

    // lösche daten falls vorhanden
    if( m_texData != NULL)
        delete m_texData;
    if( m_triData != NULL)
        delete m_triData;
    if( m_vertData != NULL)
        delete m_vertData;
    if( m_frameData != NULL)
        delete m_frameData;

	if (m_frameData!=NULL)
	{
		for (int index=0;index<m_header.numFrames;index++)
            if( m_frameData[index].pvertices != NULL)
                delete m_frameData[index].pvertices;
		delete m_frameData;
	}
}

void md2Model::md2LoadData()
{
	md2LoadSkinNames();
	md2LoadTextureCoord();
	md2LoadTriangles();
	md2LoadFrames();
	md2LoadGLCommands();
}

void md2Model::md2LoadFrames()
{
	int index=0;
	byte	*buf_t		= m_buffer+ m_header.offsetFrames;
	long	frameHeaderSize	= m_header.numFrames * sizeof(md2Frame);
	long	frameVertSize	= m_header.numVertices * sizeof(md2TriangleVertex);

	m_frameData = (pmd2Frame)md2Malloc(frameHeaderSize);
	if (!m_frameData)
		return;

	for (index=0;index< m_header.numFrames;index++)
	{
		m_frameData[index].pvertices = (pmd2TriangleVertex)md2Malloc(frameVertSize);
		if (!m_frameData[index].pvertices)
			return;
	}

	for (index=0;index< m_header.numFrames;index++)
	{
		memcpy(&m_frameData[index],buf_t, FRAME_HEADER_SIZE);
		buf_t+= FRAME_HEADER_SIZE;
		memcpy(m_frameData[index].pvertices, buf_t, frameVertSize);
		buf_t+=frameVertSize;
	}
}

void md2Model::md2LoadTriangles()
{
	byte	*buf_t		= m_buffer+ m_header.offsetTriangles;
	long	totalsize	= m_header.numTriangles * sizeof(md2Triangle);

	m_triData = (pmd2Triangle)md2Malloc(totalsize);
	if (!m_triData)
		return;

	memcpy(m_triData, buf_t, totalsize);
}

void md2Model::md2LoadTextureCoord()
{
	int		index=0;
	byte	*buf_t=m_buffer+m_header.offsetTexCoord;
	long	totalsize = m_header.numTexCoords*sizeof(md2TextureCoord);

	m_texData=(pmd2TextureCoord)md2Malloc(totalsize);
	if (!m_texData)
		return;

	memcpy(m_texData,buf_t,totalsize);
}

void md2Model::md2LoadSkinNames()
{
	int		index=0;
	byte*	buf_t=m_buffer+m_header.offsetSkins;

	for (index=0;index<m_header.numSkins;index++,buf_t+=MAX_SKIN_NAME)
		strcpy(m_skinName[index],(const char*)buf_t);
}

void md2Model::md2LoadGLCommands()
{
	return;
}

void md2Model::md2ProcessData()
{
    // free m_glVertexData data;
    if( m_glVertexData != NULL)
        delete m_glVertexData;
	m_glVertexData = (pmd2GLVertex) md2Malloc(m_header.numVertices*sizeof(md2GLVertex));
	if (!m_glVertexData)
	{
		printf("Error at md2ProcessData\n");
		return;
	}
}

void md2Model::setFrame(int frame)
{
    GLfloat v1[3], v2[3], v3[3];

	if (frame<0 || frame>= m_header.numFrames)
		return;

    printf( "md2Model::setFrame %d\n", m_obj.getVertices().size());
    printf( "md2Model::setFrame %d\n", m_obj.getTexcoords().size());
    printf( "md2Model::setFrame %d\n", m_obj.getIndices().size());

    // object array reseten, da neue array platz braucht
    m_obj.reset();

    // setze aktuele frame
    m_currentFrame = frame;
    printf( "set frame %d\n", frame);

	int index=0;
	if (m_glVertexData!=NULL)
	{
		memset(m_glVertexData,0,sizeof(md2GLVertex)*m_header.numVertices);

		for (index=0;index<m_header.numVertices;index++)
		{
			// Every vertex of frame is multiplied by it's respective scale and then the translation is added.
			m_glVertexData[index].x = (m_frameData[frame].pvertices[index].vertex[0] * m_frameData[frame].scale[0])+m_frameData[frame].translate[0];
			m_glVertexData[index].y = (m_frameData[frame].pvertices[index].vertex[1] * m_frameData[frame].scale[1])+m_frameData[frame].translate[1];
			m_glVertexData[index].z = (m_frameData[frame].pvertices[index].vertex[2] * m_frameData[frame].scale[2])+m_frameData[frame].translate[2];
		}
	}

	for (index=0;index< m_header.numTriangles;index++)
    {
        // Calculate the two vertex out of 3 by [v1 - v2] & [v2 - v3] (Read Bernard Koleman Elementary Linear Algebra P-193)
        v1[0] = m_glVertexData[m_triData[index].vertexIndices[2]].x - m_glVertexData[m_triData[index].vertexIndices[1]].x;
        v1[1] = m_glVertexData[m_triData[index].vertexIndices[2]].y - m_glVertexData[m_triData[index].vertexIndices[1]].y;
        v1[2] = m_glVertexData[m_triData[index].vertexIndices[2]].z - m_glVertexData[m_triData[index].vertexIndices[1]].z;

        v2[0] = m_glVertexData[m_triData[index].vertexIndices[1]].x - m_glVertexData[m_triData[index].vertexIndices[0]].x;
        v2[1] = m_glVertexData[m_triData[index].vertexIndices[1]].y - m_glVertexData[m_triData[index].vertexIndices[0]].y;
        v2[2] = m_glVertexData[m_triData[index].vertexIndices[1]].z - m_glVertexData[m_triData[index].vertexIndices[0]].z;

        // Take the Cross Product of v1 x v2
        v3[0] = v1[1]*v2[2] - v1[2]*v2[1];
        v3[1] = v1[2]*v2[0] - v1[0]*v2[2];
        v3[2] = v1[0]*v2[1] - v1[1]*v2[0];

        glm::vec3 vv2 = glm::vec3( m_glVertexData[m_triData[index].vertexIndices[2]].x, m_glVertexData[m_triData[index].vertexIndices[2]].y, m_glVertexData[m_triData[index].vertexIndices[2]].z);
        glm::vec3 vv1 = glm::vec3( m_glVertexData[m_triData[index].vertexIndices[1]].x, m_glVertexData[m_triData[index].vertexIndices[1]].y, m_glVertexData[m_triData[index].vertexIndices[1]].z);
        glm::vec3 vv0 = glm::vec3( m_glVertexData[m_triData[index].vertexIndices[0]].x, m_glVertexData[m_triData[index].vertexIndices[0]].y, m_glVertexData[m_triData[index].vertexIndices[0]].z);

        glm::vec2 tv2 = glm::vec2( (float)m_texData[ m_triData[index].textureIndices[2]].s/ m_header.skinWidth,(float)m_texData[ m_triData[index].textureIndices[2]].t/ m_header.skinHeight);
        glm::vec2 tv1 = glm::vec2( (float)m_texData[ m_triData[index].textureIndices[1]].s/ m_header.skinWidth,(float)m_texData[ m_triData[index].textureIndices[1]].t/ m_header.skinHeight);
        glm::vec2 tv0 = glm::vec2( (float)m_texData[ m_triData[index].textureIndices[0]].s/ m_header.skinWidth,(float)m_texData[ m_triData[index].textureIndices[0]].t/ m_header.skinHeight);


        m_obj.addTriangles( vv2, vv1, vv0, tv2, tv1, tv0);
    }

    printf( "md2Model::setFrame %d\n", m_obj.getVertices().size());
    printf( "md2Model::setFrame %d\n", m_obj.getTexcoords().size());
    printf( "md2Model::setFrame %d\n", m_obj.getIndices().size());


    // cahnge buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_obj.getVertices().size() * sizeof( glm::vec3 ), &m_obj.getVertices()[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboTexture);
    glBufferData(GL_ARRAY_BUFFER, m_obj.getTexcoords().size() * sizeof( glm::vec2 ), &m_obj.getTexcoords()[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIndex);
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, m_obj.getIndices().size() * sizeof( unsigned int ), &m_obj.getIndices()[0], GL_STATIC_DRAW);
}

int	md2Model::GetNumFrames()
{
	return this->m_header.numFrames;
}

void md2Model::draw(Shader* l_shader, Camera* l_camera)
{
        Transform f_form;
        f_form.GetPos().x = m_pos.x;
        f_form.GetPos().y = m_pos.y;
        f_form.GetPos().z = m_pos.z;

        f_form.GetRot().x = 3.14 + 3.14/2;

        Shader *shader = l_shader;

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIndex);
        shader->BindArray( m_vbo, 0, GL_FLOAT, 3);
        shader->BindArray( m_vboTexture, 1, GL_FLOAT, 2);

        shader->Bind();
        shader->EnableVertexArray( 0);
        shader->EnableVertexArray( 1);

        m_texture->Bind();

        shader->Update( f_form, l_camera, l_camera, glm::mat4(1));

        glDrawElements(GL_TRIANGLES, m_obj.getIndices().size(), GL_UNSIGNED_INT, 0);

}
