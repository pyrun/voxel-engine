#include "chunk.h"
#include "../system/timer.h"
#include <stdio.h>

//#define printf if

Chunk::Chunk( glm::ivec3 position, int seed) {
    // node reset
    p_x_pos = NULL;
    p_x_neg = NULL;
    p_y_pos = NULL;
    p_y_neg = NULL;
    p_z_pos = NULL;
    p_z_neg = NULL;

    next = NULL;

    p_body = NULL;

    p_vboVertex = 0;
    p_elements = 0;

    p_changed = false;
    p_updateVbo = false;
    p_updateRigidBody = false;

    // Set Position
    p_pos = position;

    int t = SDL_GetTicks();

    p_tile = new unsigned short[CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE]; //new (std::nothrow) tile*[ CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE]
    p_lighting = new unsigned char[CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE];

    for (int i = 0; i < CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE; i++) {
        p_tile[ i ] = EMPTY_BLOCK_ID;
        p_lighting[ i ] = 1;
    }

    updateForm();

    //printf( "Chunk::chunk take %dms creating x%d y%d z%d\n", SDL_GetTicks()-t, p_pos.x, p_pos.y, p_pos.z);
}

Chunk::~Chunk() {
    Timer timer;
    timer.Start();
    // node reset
    next = NULL;
    // Löschen p_tile
    for (int cz = 0; cz < CHUNK_SIZE; cz++) {
        for (int cx = 0; cx < CHUNK_SIZE; cx++) {
            for(int cy = 0; cy < CHUNK_SIZE; cy++) {
                    int Register = TILE_REGISTER( cx, cy, cz);
                }
        }
    }
    delete[] p_tile;
    delete[] p_lighting;

    //printf( "~Chunk(): remove p_tile in %dms\n", timer.GetTicks());
    timer.Start();

    // vbo daten löschen
    glDeleteBuffers(1, &p_vboIndex);
    glDeleteBuffers(1, &p_vboVertex);
    glDeleteBuffers(1, &p_vboNormal);
    glDeleteBuffers(1, &p_vboData);
    glDeleteBuffers(1, &p_vboLight);
    glDeleteVertexArrays( 1, &p_vboVao);

    //printf( "~Chunk(): remove data in %dms x%dy%dz%d\n", timer.GetTicks(), p_pos.x, p_pos.y, p_pos.z);
}

bool Chunk::createPhysicBody( b3World *world) {
    if( !p_updateRigidBody && world)
        return false;

    if( p_body) {
        world->DestroyBody( p_body);
        b3Free( p_mesh->vertices);
        b3Free( p_mesh->triangles);
        p_body = NULL;
        delete p_shape;
        delete p_mesh;
        return false;
    }

    b3BodyDef l_bodyDef;

    l_bodyDef.type = b3BodyType::e_staticBody;
    l_bodyDef.position.Set( p_pos.x*CHUNK_SIZE, p_pos.y*CHUNK_SIZE, p_pos.z*CHUNK_SIZE);

    p_body = world->CreateBody( l_bodyDef);

    //BuildGrid( &p_mesh, 50, 50);
    p_mesh = new b3Mesh();

    p_mesh->vertexCount = (int)p_vertices.size();
    p_mesh->vertices = (b3Vec3*)b3Alloc( p_mesh->vertexCount * sizeof(b3Vec3));

    for( int i = 0; i < (int)p_vertices.size(); i++) {
        p_mesh->vertices[i] = b3Vec3( p_vertices[i].x, p_vertices[i].y, p_vertices[i].z);
    }

    int l_triangleCount = (int)p_indices.size()/3;
    p_mesh->triangleCount  = l_triangleCount;
    p_mesh->triangles  = (b3Triangle*)b3Alloc(p_mesh->triangleCount * sizeof(b3Triangle));

    int l_triangle = 0;
    for( int i = 0; i < (int)p_indices.size(); i+=3) {
        p_mesh->triangles[ l_triangle].v1 = (int)p_indices[i+0];
        p_mesh->triangles[ l_triangle].v2 = (int)p_indices[i+1];
        p_mesh->triangles[ l_triangle].v3 = (int)p_indices[i+2];
        l_triangle++;
    }

    p_mesh->BuildTree();

    b3MeshShape l_meshShape;
    l_meshShape.m_mesh = p_mesh;


    p_shape = new b3ShapeDef();
    p_shape->shape = &l_meshShape;

    p_body->CreateShape( *p_shape);

    p_updateRigidBody = false;

    return true;
}

bool Chunk::serialize(bool writeToBitstream, RakNet::BitStream *bitstream, int start, int end, block_list *blocks)
{
    for( int i = start; i < end; i++) {
        bitstream->Serialize( writeToBitstream, p_tile[i]);
        if( p_tile[i] > MAX_TILE_ID) {
            //printf( "chunk::serialize corrupt data x%d y%d z%d Data %d to %d tileID#%d\n", (int)p_pos.x, (int)p_pos.y, (int)p_pos.z, start, end, p_tile[i]);
            p_tile[i] = EMPTY_BLOCK_ID;
            return false;
        }
        if(!writeToBitstream) {
            if( blocks->get( p_tile[i]) == NULL && p_tile[i] != EMPTY_BLOCK_ID ) {
                p_tile[i] = 0;
                return false;
            }
        }
    }
    return true;
}

void Chunk::setSide( Chunk *chunk, Chunk_side side) {
    switch( side) {
        case CHUNK_SIDE_X_POS: p_x_pos = chunk; break;
        case CHUNK_SIDE_X_NEG: p_x_neg = chunk; break;
        case CHUNK_SIDE_Y_POS: p_y_pos = chunk; break;
        case CHUNK_SIDE_Y_NEG: p_y_neg = chunk; break;
        case CHUNK_SIDE_Z_POS: p_z_pos = chunk; break;
        case CHUNK_SIDE_Z_NEG: p_z_neg = chunk; break;
    }
}

Chunk *Chunk::getSide( Chunk_side side) {
    switch( side) {
        case CHUNK_SIDE_X_POS: return p_x_pos; break;
        case CHUNK_SIDE_X_NEG: return p_x_neg; break;
        case CHUNK_SIDE_Y_POS: return p_y_pos; break;
        case CHUNK_SIDE_Y_NEG: return p_y_neg; break;
        case CHUNK_SIDE_Z_POS: return p_z_pos; break;
        case CHUNK_SIDE_Z_NEG: return p_z_neg; break;
    }
    return NULL;
}

void Chunk::set( glm::ivec3 position, int ID, bool change) {
    // tile nehmen
    p_tile[ TILE_REGISTER( position.x, position.y, position.z)] = ID;

    // welt hat sich verändert
    p_changed = change;
}

unsigned short Chunk::getTile( int X, int Y, int Z) {
    if( X < 0)
        return EMPTY_BLOCK_ID;
    if( Y < 0)
        return EMPTY_BLOCK_ID;
    if( Z < 0)
        return EMPTY_BLOCK_ID;
    return p_tile[ TILE_REGISTER( X, Y, Z)];
}

unsigned short Chunk::getTile( glm::ivec3 position) {
    if( position.x < 0 || position.y < 0 ||position.z < 0)
        return EMPTY_BLOCK_ID;
    return p_tile[ TILE_REGISTER( position.x, position.y, position.z)];
}

bool Chunk::checkTile( glm::ivec3 position) {
    Chunk *l_side;

    // x
    l_side = getSide( CHUNK_SIDE_X_NEG);
    if( position.x < 0 && l_side ) {
        return l_side->checkTile( position + glm::ivec3( CHUNK_SIZE, 0, 0));
    }
    l_side = getSide( CHUNK_SIDE_X_POS);
    if( position.x >= CHUNK_SIZE && l_side ) {
        return l_side->checkTile( position - glm::ivec3( CHUNK_SIZE, 0, 0));
    }
    // y
    l_side = getSide( CHUNK_SIDE_Y_NEG);
    if( position.y < 0 && l_side ) {
        return l_side->checkTile( position + glm::ivec3( 0, CHUNK_SIZE, 0));
    }
    l_side = getSide( CHUNK_SIDE_Y_POS);
    if( position.y >= CHUNK_SIZE && l_side ) {
        return l_side->checkTile( position - glm::ivec3( 0, CHUNK_SIZE, 0));
    }
    // z
    l_side = getSide( CHUNK_SIDE_Z_NEG);
    if( position.z < 0 && l_side ) {
        return l_side->checkTile( position + glm::ivec3( 0, 0, CHUNK_SIZE));
    }
    l_side = getSide( CHUNK_SIDE_Z_POS);
    if( position.z >= CHUNK_SIZE && l_side ) {
        return l_side->checkTile( position - glm::ivec3( 0, 0, CHUNK_SIZE));
    }

    if( position.x < 0 || position.x >= CHUNK_SIZE ||
        position.y < 0 || position.y >= CHUNK_SIZE ||
        position.z < 0 || position.z >= CHUNK_SIZE)
        return false;
    return p_tile[ TILE_REGISTER( position.x, position.y, position.z) ] == EMPTY_BLOCK_ID?false:true;
}

// get XXXX0000
int Chunk::getSunlight( glm::ivec3 position) {
    Chunk *l_side;

    // x
    l_side = getSide( CHUNK_SIDE_X_NEG);
    if( position.x < 0 && l_side ) {
        return l_side->getSunlight( position + glm::ivec3( CHUNK_SIZE, 0, 0));
    }
    l_side = getSide( CHUNK_SIDE_X_POS);
    if( position.x >= CHUNK_SIZE && l_side ) {
        return l_side->getSunlight( position - glm::ivec3( CHUNK_SIZE, 0, 0));
    }
    // y
    l_side = getSide( CHUNK_SIDE_Y_NEG);
    if( position.y < 0 && l_side ) {
        return l_side->getSunlight( position + glm::ivec3( 0, CHUNK_SIZE, 0));
    }
    l_side = getSide( CHUNK_SIDE_Y_POS);
    if( position.y >= CHUNK_SIZE && l_side ) {
        return l_side->getSunlight( position - glm::ivec3( 0, CHUNK_SIZE, 0));
    }
    // z
    l_side = getSide( CHUNK_SIDE_Z_NEG);
    if( position.z < 0 && l_side ) {
        return l_side->getSunlight( position + glm::ivec3( 0, 0, CHUNK_SIZE));
    }
    l_side = getSide( CHUNK_SIDE_Z_POS);
    if( position.z >= CHUNK_SIZE && l_side ) {
        return l_side->getSunlight( position - glm::ivec3( 0, 0, CHUNK_SIZE));
    }
    if( position.x < 0 || position.x >= CHUNK_SIZE ||
        position.y < 0 || position.y >= CHUNK_SIZE ||
        position.z < 0 || position.z >= CHUNK_SIZE)
        return false;
    return (p_lighting[ TILE_REGISTER((int)position.x, (int)position.y, (int)position.z)] >> 4) & 0xF;
}

// set XXXX0000
void Chunk::setSunlight( glm::ivec3 position, int val) {
    Chunk *l_side;
    // x
    if( position.x < 0 && getSide( CHUNK_SIDE_X_NEG)) {
        l_side = getSide( CHUNK_SIDE_X_NEG);
        l_side->setSunlight( position + glm::ivec3( CHUNK_SIZE, 0, 0), val );
        return;
    }
    if( position.x >= CHUNK_SIZE && getSide( CHUNK_SIDE_X_POS)) {
        l_side = getSide( CHUNK_SIDE_X_POS);
        l_side->setSunlight( position - glm::ivec3( CHUNK_SIZE, 0, 0), val );
        return;
    }
    // y
    if( position.y < 0 && getSide( CHUNK_SIDE_Y_NEG)) {
        l_side = getSide( CHUNK_SIDE_Y_POS);
        l_side->setSunlight( position + glm::ivec3( 0, CHUNK_SIZE, 0), val );
        return;
    }
    if( position.y >= CHUNK_SIZE && getSide( CHUNK_SIDE_Y_POS)) {
        l_side = getSide( CHUNK_SIDE_Y_POS);
        l_side->setSunlight( position - glm::ivec3( 0, CHUNK_SIZE, 0), val );
        return;
    }
    // z
    if( position.z < 0 && getSide( CHUNK_SIDE_Z_NEG)) {
        l_side = getSide( CHUNK_SIDE_Z_NEG);
        l_side->setSunlight( position + glm::ivec3( 0, 0, CHUNK_SIZE), val );
        return;
    }
    if( position.z >= CHUNK_SIZE && getSide( CHUNK_SIDE_Z_POS)) {
        l_side = getSide( CHUNK_SIDE_Z_POS);
        l_side->setSunlight( position - glm::ivec3( 0, 0, CHUNK_SIZE), val );
        return;
    }

    p_lighting[ TILE_REGISTER( position.x, position.y, position.z)] = (p_lighting[ TILE_REGISTER( position.x, position.y, position.z)] & 0xF) | (val << 4);
}


// get 0000XXXX
int Chunk::getTorchlight( glm::ivec3 position) {

    return p_lighting[ TILE_REGISTER( position.x, position.y, position.z)] & 0xF;
}

// set 0000XXXX
void Chunk::setTorchlight( glm::ivec3 position, int val) {
    p_lighting[ TILE_REGISTER( position.x, position.y, position.z)] = (p_lighting[ TILE_REGISTER( position.x, position.y, position.z)] & 0xF0) | val;
}

void Chunk::addFace( Chunk_side side, glm::ivec3 pos, glm::ivec3 texture, glm::ivec3 blockPos) {
    int l_flip = 0;
    glm::ivec3 l_offset;
    int l_indices = p_indices_length;
    int l_vertices = p_vertices_length;

    int l_vertice_index = 0;
    glm::ivec3 l_vertice_pos[12] = {    glm::ivec3(1, 0, 0),// x
                                        glm::ivec3(1, 1, 0),
                                        glm::ivec3(1, 0, 1),
                                        glm::ivec3(1, 1, 1),
                                        glm::ivec3(0, 0, 0),// y
                                        glm::ivec3(1, 0, 0),
                                        glm::ivec3(0, 0, 1),
                                        glm::ivec3(1, 0, 1),
                                        glm::ivec3(0, 0, 0),// z
                                        glm::ivec3(0, 1, 0),
                                        glm::ivec3(1, 0, 0),
                                        glm::ivec3(1, 1, 0)};

    if( p_indices_length+6 > p_indices.size()) {
        p_indices.resize( p_indices.size() + 6);
        p_vertices.resize( p_vertices.size() + 4);
        p_texture.resize( p_texture.size() + 4);
        p_light.resize( p_light.size() + 4 );
    }

    p_indices_length +=6;
    p_vertices_length +=4;

    //
    switch( side) {
        case CHUNK_SIDE_X_POS: {
            l_flip = 5;
            l_vertice_index = 0;
            l_offset = -glm::ivec3( 1.f, 0.f, 0.f);
        } break;
        case CHUNK_SIDE_X_NEG: {
            l_flip = 0;
            l_vertice_index = 0;
            l_offset = glm::ivec3( 1.f, 0.f, 0.f);
        } break;
        case CHUNK_SIDE_Y_POS: {
            l_flip = 5;
            l_vertice_index = 4;
            l_offset = glm::ivec3( 0.f, 1.f, 0.f);
        } break;
        case CHUNK_SIDE_Y_NEG: {
            l_flip = 0;
            l_vertice_index = 4;
            l_offset = -glm::ivec3( 0.f, 1.f, 0.f);
        } break;
        case CHUNK_SIDE_Z_POS: {
            l_flip = 5;
            l_vertice_index = 8;
            l_offset = glm::ivec3( 0.f, 0.f, 1.f);
        } break;
        case CHUNK_SIDE_Z_NEG: {
            l_flip = 0;
            l_vertice_index = 8;
            l_offset = -glm::ivec3( 0.f, 0.f, 1.f);
        } break;
    }

    // set indices
    p_indices[ l_indices + abs(0-l_flip) ] = l_vertices+0;
    p_indices[ l_indices + abs(1-l_flip) ] = l_vertices+1;
    p_indices[ l_indices + abs(2-l_flip) ] = l_vertices+2;
    p_indices[ l_indices + abs(3-l_flip) ] = l_vertices+2;
    p_indices[ l_indices + abs(4-l_flip) ] = l_vertices+1;
    p_indices[ l_indices + abs(5-l_flip) ] = l_vertices+3;

    // set vertices
    p_vertices[ l_vertices + 0] = pos + l_vertice_pos[ l_vertice_index+0];
    p_vertices[ l_vertices + 1] = pos + l_vertice_pos[ l_vertice_index+1];
    p_vertices[ l_vertices + 2] = pos + l_vertice_pos[ l_vertice_index+2];
    p_vertices[ l_vertices + 3] = pos + l_vertice_pos[ l_vertice_index+3];

    // set texture
    p_texture[ l_vertices + 0] = texture;
    p_texture[ l_vertices + 1] = texture;
    p_texture[ l_vertices + 2] = texture;
    p_texture[ l_vertices + 3] = texture;

    // set lighting
    int l_lighting = getSunlight( blockPos + l_offset );
    p_light[ l_vertices + 0] = glm::ivec3( l_lighting);
    p_light[ l_vertices + 1] = glm::ivec3( l_lighting);
    p_light[ l_vertices + 2] = glm::ivec3( l_lighting);
    p_light[ l_vertices + 3] = glm::ivec3( l_lighting);
}

void Chunk::updateArray( block_list *List) {
    int i = 0;
    glm::ivec2 l_side_textur_position;
    glm::ivec3 l_block;
    Timer timer;

    if( !List )
        return;

    timer.Start();

    /*p_indices.clear( );
    p_vertices.clear( );
    p_texture.clear( );
   */
    p_indices_length = 0;
    p_vertices_length = 0;

    bool b_visibility = false;

    int l_tile = 0;

    glm::ivec3 l_pos = getPos();

    // View from positive x
    for( l_block.z = 0; l_block.z < CHUNK_SIZE; l_block.z++) {
        for( l_block.x = CHUNK_SIZE - 1; l_block.x >= 0; l_block.x--) {
            for( l_block.y = 0; l_block.y < CHUNK_SIZE; l_block.y++) {
                l_tile = getTile( l_block);
                if( l_tile == EMPTY_BLOCK_ID) {// tile nicht vorhanden
                    b_visibility = false;
                    continue;
                }
                int type = l_tile;
                if( type == 0) {
                    b_visibility = false;
                    continue;
                }
                // Line of sight blocked?
                if( l_block.x != 0 && checkTile( l_block + glm::ivec3( -1, 0, 0) ) && List->get( type)->getAlpha() == List->get( getTile( l_block - glm::ivec3( 1, 0, 0) ) )->getAlpha() )
                {
                    b_visibility = false;
                    continue;
                }
                // View from negative x
                /*if( x == 0 && getSide( CHUNK_SIDE_X_POS) != NULL && Back->checkTile(CHUNK_SIZE-1, y, z) && Back->getTile( CHUNK_SIZE-1, y, z)) {
                    b_visibility = false;
                    continue;
                }*/
                if( 0 && b_visibility && l_block.y != 0 && checkTile( l_block + glm::ivec3( 0, -1, 0)) && ( l_tile == getTile( l_block - glm::ivec3( 0, -1, 0)) ) && getSunlight( glm::ivec3( l_block.x, l_block.y-1, l_block.z) ) == getSunlight( glm::ivec3( l_block.x, l_block.y-2, l_block.z) )) {
                    p_vertices[ p_vertices_length - 3] = l_block + glm::ivec3( 0, +1, 0);
                    p_vertices[ p_vertices_length - 1] = l_block + glm::ivec3( 0, +1, +1);
                } else {
                    l_side_textur_position = List->getTexturByType( type, BLOCK_SIDE_FRONT);
                    addFace( CHUNK_SIDE_X_POS, glm::ivec3( l_block.x-1, l_block.y, l_block.z), glm::ivec3( l_side_textur_position.x, l_side_textur_position.y, 0), l_block);
                }
                b_visibility = true;
            }
        }
    }

    // View from negative x
    for( l_block.z = 0; l_block.z < CHUNK_SIZE; l_block.z++) {
        for( l_block.x = 0; l_block.x < CHUNK_SIZE; l_block.x++)  {
            for( l_block.y = 0; l_block.y < CHUNK_SIZE; l_block.y++) {
                l_tile = getTile( l_block);
                if( !l_tile)
                    continue;

                if( l_tile == EMPTY_BLOCK_ID) {// tile nicht vorhanden
                    b_visibility = false;
                    continue;
                }
                int type = l_tile;
                if( type == 0) {
                    b_visibility = false;
                    continue;
                }

                if( l_block.x != CHUNK_SIZE-1 && checkTile( l_block + glm::ivec3( +1, 0, 0) ) && List->get( type)->getAlpha() == List->get( getTile( l_block.x+1, l_block.y, l_block.z))->getAlpha() ) {
                    b_visibility = false;
                    continue;
                }
                // View from positive x
                /*if( x == CHUNK_SIZE-1 && Front != NULL && Front->checkTile( 0, y, z) && Front->getTile( 0, y, z)) {
                    b_visibility = false;
                    continue;
                }*/
                if( 0 && b_visibility && l_block.y != 0 && checkTile( l_block + glm::ivec3( 0, -1, 0)) && l_tile == getTile( l_block.x, l_block.y-1, l_block.z) && getSunlight(l_block) == getSunlight( l_block) ){
                    p_vertices[ p_vertices_length - 3] = glm::ivec3( l_block.x+1, l_block.y+1, l_block.z);
                    p_vertices[ p_vertices_length - 1] = glm::ivec3( l_block.x+1, l_block.y+1, l_block.z+1);
                } else {
                    l_side_textur_position = List->getTexturByType( type, BLOCK_SIDE_BACK);
                    addFace( CHUNK_SIDE_X_NEG, l_block, glm::ivec3( l_side_textur_position.x, l_side_textur_position.y, 0), l_block);
                }
                b_visibility = true;
            }
        }
    }

    // View from positive y
    for( int z = 0; z < CHUNK_SIZE; z++) {
         for(int y = 0; y < CHUNK_SIZE; y++){
             for(int x = 0; x < CHUNK_SIZE; x++) {
                if( getTile( x, y, z) == NULL) // tile nicht vorhanden
                    continue;
                int type = getTile( x, y, z);
                if( type == 0) {
                    b_visibility = false;
                    continue;
                }
                if(  y != CHUNK_SIZE-1 && checkTile( glm::ivec3(x, y+1, z)) && List->get( type)->getAlpha() == List->get( getTile( x, y+1, z))->getAlpha() ) {
                    b_visibility = false;
                    continue;
                }
                /*if( y == CHUNK_SIZE-1 && Up != NULL && Up->checkTile(x, 0, z) && Up->getTile( x, 0, z)) {
                    b_visibility = false;
                    continue;
                }*/
                if( 0 && b_visibility && x != 0 && checkTile( glm::ivec3( x-1, y, z)) && getTile( x-1, y, z) == getTile( x, y, z) && getSunlight( glm::ivec3( x, y, z)) == getSunlight( glm::ivec3( x-1, y, z))) {
                    p_vertices[ p_vertices_length - 3] = glm::ivec3( x+1, y+1, z);
                    p_vertices[ p_vertices_length - 1] = glm::ivec3( x+1, y+1, z+1);
                } else {
                    l_side_textur_position = List->getTexturByType( type, BLOCK_SIDE_TOP);
                    addFace( CHUNK_SIDE_Y_POS, glm::ivec3( x, y+1, z), glm::ivec3( l_side_textur_position.x, l_side_textur_position.y, 1), glm::ivec3( x, y, z));
                }
                b_visibility = true;
            }
        }
    }

    // View from negative y
    for(int z = 0; z < CHUNK_SIZE; z++) {
        for(int y = CHUNK_SIZE - 1; y >= 0; y--) {
            for(int x = 0; x < CHUNK_SIZE; x++) {
                if( getTile( x, y, z) == NULL) // tile nicht vorhanden
                    continue;
                int type = getTile( x, y, z);
                if( type == 0) {
                    b_visibility = false;
                    continue;
                }
                if( y != 0 && checkTile( glm::ivec3( x, y-1, z)) && List->get( type)->getAlpha() == List->get( getTile( x, y-1, z))->getAlpha() ) {
                    b_visibility = false;
                    continue;
                }
                /*if( y == 0 && Down != NULL && Down->checkTile(x, CHUNK_SIZE-1, z) && Down->getTile( x, CHUNK_SIZE-1, z)) {
                    b_visibility = false;
                    continue;
                }*/
                if( 0 && b_visibility && x != 0 && checkTile( glm::ivec3( x-1, y, z)) && getTile( x-1, y, z) == getTile( x, y, z) && getSunlight( glm::ivec3( x, y, z)) == getSunlight( glm::ivec3( x-1, y, z))) {
                    p_vertices[ p_vertices_length - 3] = glm::ivec3( x+1, y, z);
                    p_vertices[ p_vertices_length - 1] = glm::ivec3( x+1, y, z+1);
                } else {
                    l_side_textur_position = List->getTexturByType( type, BLOCK_SIDE_BUTTOM);
                    addFace( CHUNK_SIDE_Y_NEG, glm::ivec3( x, y, z), glm::ivec3( l_side_textur_position.x, l_side_textur_position.y, 1), glm::ivec3( x, y, z));
                }
                b_visibility = true;
            }
        }
    }

    // View from positive z
    for(int z = 0; z < CHUNK_SIZE; z++) {
        for(int x = 0; x < CHUNK_SIZE; x++) {
            for(int y = 0; y < CHUNK_SIZE; y++) {
                if( getTile( x, y, z) == NULL) // tile nicht vorhanden
                    continue;
                int type = getTile( x, y, z);
                if( type == 0) {
                    b_visibility = false;
                    continue;
                }
                if( z != CHUNK_SIZE-1 && checkTile( glm::ivec3( x, y, z+1)) && List->get( type)->getAlpha() == List->get( getTile( x, y, z+1))->getAlpha() ) {
                    b_visibility = false;
                    continue;
                }
                // View from positive z
                /*if( z == CHUNK_SIZE-1 && Right != NULL && Right->checkTile(x, y, 0) && Right->getTile( x, y, 0)) {
                    b_visibility = false;
                    continue;
                }*/
                if( 0 && b_visibility && y != 0 && checkTile( glm::ivec3( x, y-1, z)) && getTile( x, y-1, z) == getTile( x, y, z) && getSunlight( glm::ivec3( x, y, z)) == getSunlight( glm::ivec3( x, y-1, z))) {
                    p_vertices[ p_vertices_length - 3] = glm::ivec3( x, y+1, z+1);
                    p_vertices[ p_vertices_length - 1] = glm::ivec3( x+1, y+1, z+1);
                } else {
                    l_side_textur_position = List->getTexturByType( type, BLOCK_SIDE_RIGHT);
                    addFace( CHUNK_SIDE_Z_POS, glm::ivec3( x, y, z+1), glm::ivec3( l_side_textur_position.x, l_side_textur_position.y, 0), glm::ivec3( x, y, z));
                }
                b_visibility = true;
            }
        }
    }

    // View from negative z
    for( l_block.z = CHUNK_SIZE - 1; l_block.z >= 0; l_block.z-=1) {
         for( l_block.x = 0; l_block.x < CHUNK_SIZE; l_block.x++){
            for( l_block.y = 0; l_block.y < CHUNK_SIZE; l_block.y++) {
                if( getTile( l_block) == NULL) // tile nicht vorhanden
                    continue;
                int type = getTile( l_block);
                if( type == 0) {
                    b_visibility = false;
                    continue;
                }
                if(  l_block.z != 0 && checkTile( l_block - glm::ivec3( 0, 0, 1)) && List->get( type)->getAlpha() == List->get( getTile( l_block.x, l_block.y, l_block.z-1))->getAlpha() ) {
                    b_visibility = false;
                    continue;
                }
                /*if( z == 0 && Left != NULL && Left->checkTile(x, y, CHUNK_SIZE-1) && Left->getTile( x, y, CHUNK_SIZE-1)) {
                    b_visibility = false;
                    continue;
                }*/
                if( 0 && b_visibility && l_block.y != 0 && checkTile( l_block - glm::ivec3( 0, 1, 0)) && getTile( l_block) == getTile( l_block.x, l_block.y-1, l_block.z) && getSunlight( l_block) == getSunlight( l_block - glm::ivec3( 0, 1, 0) )) {
                    p_vertices[ p_vertices_length - 3] = l_block + glm::ivec3( 0, 1, 0);
                    p_vertices[ p_vertices_length - 1] = l_block + glm::ivec3( 1, 1, 0);
                } else {
                    l_side_textur_position = List->getTexturByType( type, BLOCK_SIDE_LEFT);
                    addFace( CHUNK_SIDE_Z_NEG, l_block, glm::ivec3( l_side_textur_position.x, l_side_textur_position.y, 0), l_block);
                }
                b_visibility = true;
            }
        }
    }
    //printf( "Chunk::updateArray One %dms\n", timer.GetTicks());

    // normal errechnen
    /*p_normal.clear();*/
    if( p_normal.size() < p_vertices_length)
        p_normal.resize( p_vertices_length);
    for( int i = 0; i < p_indices_length; i+=3) {
        glm::vec3 a(p_vertices[ p_indices[i+0]].x, p_vertices[p_indices[i+0]].y, p_vertices[p_indices[i+0]].z);
        glm::vec3 b(p_vertices[ p_indices[i+1]].x, p_vertices[p_indices[i+1]].y, p_vertices[p_indices[i+1]].z);
        glm::vec3 c(p_vertices[ p_indices[i+2]].x, p_vertices[p_indices[i+2]].y, p_vertices[p_indices[i+2]].z);
        glm::vec3 edge1 = b-a;
        glm::vec3 edge2 = c-a;
        glm::vec3 normal = glm::normalize( glm::cross( edge1, edge2));

        p_normal[p_indices[i+0]] = normal;
        p_normal[p_indices[i+1]] = normal;
        p_normal[p_indices[i+2]] = normal;
    }
    p_elements = p_indices_length;

    p_changed = false;

    if( p_elements == 0) {// Kein Speicher resavieren weil leer
        return;
    }

    p_updateVbo = true;
    p_updateRigidBody = true;

    //printf( "Chunk::updateArray %dms %d %d %d\n", timer.GetTicks(), p_elements, getAmount());
}

void Chunk::updateForm()
{
    p_form.setPos( glm::vec3( p_pos.x*CHUNK_SIZE, p_pos.y*CHUNK_SIZE, p_pos.z*CHUNK_SIZE) );
    //p_form.setScale( glm::vec3( CHUNK_SCALE));
}

void Chunk::updateVbo() {
    Timer timer;
    timer.Start();

    // Nicht bearbeiten falls es anderweilig bearbeitet wird
    if( p_elements == 0)
        return;

    if( getVbo() == 0) {
        // create vao
        glGenVertexArrays(1, &p_vboVao);
        // create index buffer
        glGenBuffers(1, &p_vboIndex);

        // Create vbo
        glGenBuffers(1, &p_vboVertex);
        glGenBuffers(1, &p_vboNormal);
        glGenBuffers(1, &p_vboData);
        glGenBuffers(1, &p_vboLight);
    }

    // index buffer
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, p_vboIndex);
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, p_indices_length * sizeof( unsigned int ), &p_indices[0], GL_DYNAMIC_DRAW);

    // vbo updaten
    // vertex
    glBindBuffer(GL_ARRAY_BUFFER, p_vboVertex);
    glBufferData(GL_ARRAY_BUFFER, p_vertices_length * sizeof(glm::ivec3), &p_vertices[0], GL_STATIC_DRAW);
    // normal
    glBindBuffer(GL_ARRAY_BUFFER, p_vboNormal);
    glBufferData(GL_ARRAY_BUFFER, p_vertices_length * sizeof(glm::ivec3), &p_normal[0], GL_STATIC_DRAW);
    // data
    glBindBuffer(GL_ARRAY_BUFFER, p_vboData);
    glBufferData(GL_ARRAY_BUFFER, p_vertices_length * sizeof(glm::ivec3), &p_texture[0], GL_STATIC_DRAW);
    // light
    glBindBuffer(GL_ARRAY_BUFFER, p_vboLight);
    glBufferData(GL_ARRAY_BUFFER, p_vertices_length * sizeof(glm::ivec3), &p_light[0], GL_STATIC_DRAW);

    // VAO
    glBindVertexArray( p_vboVao);

    //array buffer bind
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, p_vboIndex);

    // vertex
    glEnableVertexAttribArray(0);
    glBindBuffer( GL_ARRAY_BUFFER, p_vboVertex);
    glVertexAttribPointer( 0, 3, GL_INT, GL_FALSE, 0, (void*)0);

    // normal
    glEnableVertexAttribArray(1);
    glBindBuffer( GL_ARRAY_BUFFER, p_vboNormal);
    glVertexAttribPointer( 1, 3, GL_INT, GL_FALSE, 0, (void*)0);

    // data
    glEnableVertexAttribArray( 2);
    glBindBuffer( GL_ARRAY_BUFFER, p_vboData);
    glVertexAttribPointer( 2, 3, GL_INT, GL_FALSE, 0, (void*)0);

    // data
    glEnableVertexAttribArray( 3);
    glBindBuffer( GL_ARRAY_BUFFER, p_vboLight);
    glVertexAttribPointer( 3, 3, GL_INT, GL_FALSE, 0, (void*)0);

    glBindVertexArray(0);

    p_updateVbo = false;

    // print
    //printf( "UpdateVbo %dms %d * %d = %d\n", timer.GetTicks(), sizeof(glm::vec3), getAmount(), getAmount() * sizeof(glm::vec3));
}

bool Chunk::draw( Shader* shader) {
    if( p_vboVertex == 0 && !p_updateVbo)
        return false;
    if( p_updateVbo)
        updateVbo();

    // calculation view
    float l_diameter = sqrtf( CHUNK_SIZE*CHUNK_SIZE*3);
    glm::mat4 l_mvp = shader->getVP() * p_form.getModel();
    glm::vec4 l_center =  l_mvp * glm::vec4( glm::vec3(CHUNK_SIZE/2), 1); // center of the chunk
    l_center.x /= l_center.w;
    l_center.y /= l_center.w;

    // behind the camera
    if( l_center.z < -CHUNK_SIZE)
        return false;

    // outside the draw view
    if( fabsf( l_center.x) > 1 + fabsf( CHUNK_SIZE * 2 / l_center.w) || fabsf( l_center.y) > 1 + fabsf( CHUNK_SIZE * 2 / l_center.w))
        return false;

    // set model matrix
    shader->update( MAT_MODEL, p_form.getModel());

    // draw
    glBindVertexArray( p_vboVao);
    glDrawElements( GL_TRIANGLES, p_elements, GL_UNSIGNED_INT, NULL);
    glBindVertexArray( 0);

    return true;
}
