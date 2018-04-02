#include "debug_draw.h"

debug_draw::debug_draw() {
    p_vbo_vertex = 0;
    p_vbo_normal = 0;
    p_vao = 0;

    p_vector_size = 0;

    p_vertex.resize( 1024*512);
    p_normal.resize( 1024*512);

    p_change = true;
}

debug_draw::~debug_draw() {
    if( p_vbo_vertex) {
        glDeleteVertexArrays(1, &p_vao );
        glDeleteBuffers(1, &p_vbo_vertex);
        glDeleteBuffers(1, &p_vbo_normal);
    }
}

void debug_draw::drawHull(const b3HullShape* s, const b3Color& c, const b3Transform& xf)
{
	const b3Hull* hull = s->m_hull;

	for (u32 i = 0; i < hull->faceCount; ++i)
	{
		const b3Face* face = hull->GetFace(i);
		const b3HalfEdge* begin = hull->GetEdge(face->edge);

		b3Vec3 n = xf.rotation * hull->planes[i].normal;

		const b3HalfEdge* edge = hull->GetEdge(begin->next);
		do
		{
			u32 i1 = begin->origin;
			u32 i2 = edge->origin;
			const b3HalfEdge* next = hull->GetEdge(edge->next);
			u32 i3 = next->origin;

			b3Vec3 p1 = xf * hull->vertices[i1];
			b3Vec3 p2 = xf * hull->vertices[i2];
			b3Vec3 p3 = xf * hull->vertices[i3];

			DrawTriangle( p1, p2, p3, c);

			edge = next;
		} while (hull->GetEdge(edge->next) != begin);
	}
}

void debug_draw::drawMesh(const b3MeshShape* s, const b3Color& c, const b3Transform& xf)
{
	const b3Mesh* mesh = s->m_mesh;
	for (u32 i = 0; i < mesh->triangleCount; ++i)
	{
		const b3Triangle* t = mesh->triangles + i;

		b3Vec3 p1 = xf * mesh->vertices[t->v1];
		b3Vec3 p2 = xf * mesh->vertices[t->v2];
		b3Vec3 p3 = xf * mesh->vertices[t->v3];

		b3Vec3 n1 = b3Cross(p2 - p1, p3 - p1);
		n1.Normalize();

		DrawSolidTriangle( n1, p1, p2, p3, c);

		b3Vec3 n2 = -n1;

		DrawSolidTriangle( n1, p1, p3, p2, c);
	}
}

void debug_draw::drawShape(const b3Shape* s, const b3Color& c, const b3Transform& xf)
{
	switch (s->GetType())
	{
	case e_sphereShape:
	{
		//DrawSphere((b3SphereShape*)s, c, xf);
		break;
	}
	case e_capsuleShape:
	{
		//DrawCapsule((b3CapsuleShape*)s, c, xf);
		break;
	}
	case e_hullShape:
	{
		drawHull((b3HullShape*)s, c, xf);
		break;
	}
	case e_meshShape:
	{
		//drawMesh((b3MeshShape*)s, c, xf);
		break;
	}
	default:
	{
		break;
	}
	}
}

void debug_draw::draw(const b3World& world, glm::mat4 viewmatrix, Shader *shader) {
    for (b3Body* b = world.GetBodyList().m_head; b; b = b->GetNext())
	{
		b3Color c;
		if (b->IsAwake() == false)
		{
			c = b3Color(0.5f, 0.25f, 0.25f, 1.0f);
		}
		else if (b->GetType() == e_staticBody)
		{
			c = b3Color(0.5f, 0.5f, 0.5f, 1.0f);
		}
		else if (b->GetType() == e_dynamicBody)
		{
			c = b3Color(1.0f, 0.5f, 0.5f, 1.0f);
		}
		else
		{
			c = b3Color(0.5f, 0.5f, 1.0f, 1.0f);
		}

		b3Transform xf = b->GetTransform();
		for (b3Shape* s = b->GetShapeList().m_head; s; s = s->GetNext())
		{
			drawShape(s, c, xf);
		}
	}

    if( p_vector_size == 0)
        return;
    // create vbo and vao if not set
    if( !p_vbo_vertex) {
        printf( "p_vbo_vertex test\n");
        glGenBuffers( 1, &p_vbo_vertex);
        glGenBuffers( 1, &p_vbo_normal);
    }
    if( !p_vao) {
        printf( "glGenVertexArrays test\n");
        glGenVertexArrays(1, &p_vao);

        glBindVertexArray( p_vao);

        glEnableVertexAttribArray(0);
        glBindBuffer( GL_ARRAY_BUFFER, p_vbo_vertex);
        glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL );

        glEnableVertexAttribArray(1);
        glBindBuffer( GL_ARRAY_BUFFER, p_vbo_normal);
        glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL );

        glBindVertexArray(0);

        p_mvp = glGetUniformLocation( shader->GetProgram(), "g_mvp");
    }

    glUniformMatrix4fv( p_mvp, 1, GL_FALSE, glm::value_ptr( viewmatrix));
    glBindVertexArray( p_vao);

    if( p_change ) {
        p_change = false;
        glBindBuffer( GL_ARRAY_BUFFER, p_vbo_vertex);
        glBufferData( GL_ARRAY_BUFFER, p_vector_size * sizeof( glm::vec3 ), &p_vertex[0], GL_STATIC_DRAW);

        glBindBuffer( GL_ARRAY_BUFFER, p_vbo_normal);
        glBufferData( GL_ARRAY_BUFFER, p_vector_size * sizeof( glm::vec3 ), &p_normal[0], GL_STATIC_DRAW);
    }

    //glDrawElements( GL_TRIANGLES, p_vector_size, GL_UNSIGNED_INT, 0);

    glDrawArrays( GL_TRIANGLES, 0, p_vector_size);

    glBindVertexArray(0);

    p_vector_size = 0;
}
