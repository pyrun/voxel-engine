// View from negative x
    for(int z = 0; z < CHUNK_DEPTH; z++) {
        for(int x = CHUNK_WIDTH - 1; x >= 0; x--) {
            for(int y = 0; y < CHUNK_HEIGHT; y++) {
                l_tile = getTile( x, y, z);
                if( !l_tile)
                    continue;

                if( l_tile->ID == EMPTY_BLOCK_ID) {// Tile nicht vorhanden
                    b_visibility = false;
                    continue;
                }
                unsigned int type = l_tile->ID;
                if( type == 0) {
                    b_visibility = false;
                    continue;
                }
                // Line of sight blocked?
                if(  x != 0 && CheckTile(x-1, y, z) && List->GetBlock( type)->getAlpha() == List->GetBlock( getTile( x-1, y, z)->ID )->getAlpha() )
                {
                    b_visibility = false;
                    continue;
                }
                // View from negative x
                if( x == 0 && Back != NULL && Back->CheckTile(CHUNK_WIDTH-1, y, z) && Back->getTile( CHUNK_WIDTH-1, y, z)->ID) {
                    b_visibility = false;
                    continue;
                }
                if( 0&& b_visibility && y != 0 && CheckTile(x, y-1, z) && ( l_tile->ID == getTile( x, y-1, z)->ID) ) {
                    p_vertex[i - 4] = block_vertex( x,     y+1,     z, 0);
                    p_vertex[i - 3] = block_vertex( x, y+1, z, 0);
                    p_vertex[i - 1] = block_vertex( x, y+1, z+1, 0);
                } else {
                    Side_Textur_Pos = List->GetTexturByType( type, 0);

                    p_vertex.resize( p_vertex.size() + 6);
                    p_data.resize( p_data.size() + 6);

                    p_data[i] = block_data( 0.f, Side_Textur_Pos.x, Side_Textur_Pos.y, 0.f);
                    p_vertex[i++] = block_vertex(x, y, z, 0);

                    p_data[i] = block_data( 0, Side_Textur_Pos.x, Side_Textur_Pos.y, 0.f);
                    p_vertex[i++] = block_vertex(x, y, z + 1, 0);

                    p_data[i] = block_data( 0, Side_Textur_Pos.x, Side_Textur_Pos.y, 0.f);
                    p_vertex[i++] = block_vertex(x, y + 1, z, 0);

                    p_data[i] = block_data( 0, Side_Textur_Pos.x, Side_Textur_Pos.y, 0.f);
                    p_vertex[i++] = block_vertex(x, y + 1, z, 0);

                    p_data[i] = block_data( 0, Side_Textur_Pos.x, Side_Textur_Pos.y, 0.f);
                    p_vertex[i++] = block_vertex(x, y, z + 1, 0);

                    p_data[i] = block_data( 0, Side_Textur_Pos.x, Side_Textur_Pos.y, 0.f);
                    p_vertex[i++] = block_vertex(x, y + 1, z + 1, 0);
                }

                b_visibility = true;
            }
        }
    }
    // View from positive x
    for(int z = 0; z < CHUNK_DEPTH; z++) {
        for(int x = 0; x < CHUNK_WIDTH; x++)  {
            for(int y = 0; y < CHUNK_HEIGHT; y++) {
                if( getTile( x, y, z) == NULL) // Tile nicht vorhanden
                    continue;
                uint8_t type = getTile( x, y, z)->ID;
                if( type == 0) {
                    b_visibility = false;
                    continue;
                }
                if(  x != CHUNK_WIDTH-1 && CheckTile(x+1, y, z) && List->GetBlock( type)->getAlpha() == List->GetBlock( getTile( x+1, y, z)->ID)->getAlpha() ) {
                    b_visibility = false;
                    continue;
                }
                // View from positive x
                if( x == CHUNK_WIDTH-1 && Front != NULL && Front->CheckTile( 0, y, z) && Front->getTile( 0, y, z)->ID) {
                    b_visibility = false;
                    continue;
                }
                if(b_visibility && y != 0 && CheckTile( x, y-1, z) && getTile( x, y, z)->ID == getTile( x, y-1, z)->ID) {
                    p_vertex[i - 5] = block_vertex(x + 1, y+1, z, 0);
                    p_vertex[i - 3] = block_vertex(x +1, y+1, z, 0);
                    p_vertex[i - 2] = block_vertex(x +1, y+1, z+1, 0);
                } else {
                    p_vertex.resize( p_vertex.size() + 6);
                    p_data.resize( p_data.size() + 6);

                    Side_Textur_Pos = List->GetTexturByType( type, 1);

                    p_data[i] = block_data( 0, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertex[i++] = block_vertex(x + 1, y, z, 0);

                    p_data[i] = block_data( 0, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertex[i++] = block_vertex(x + 1, y + 1, z, 0);

                    p_data[i] = block_data( 0, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertex[i++] = block_vertex(x + 1, y, z + 1, 0);

                    p_data[i] = block_data( 0, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertex[i++] = block_vertex(x + 1, y + 1, z, 0);

                    p_data[i] = block_data( 0, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertex[i++] = block_vertex(x + 1, y + 1, z + 1, 0);

                    p_data[i] = block_data( 0, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertex[i++] = block_vertex(x + 1, y, z + 1, 0);
                }
                b_visibility = true;
            }
        }
    }

    // View from negative y
    for(int z = 0; z < CHUNK_DEPTH; z++) {
        for(int y = CHUNK_HEIGHT - 1; y >= 0; y--)
            for(int x = 0; x < CHUNK_WIDTH; x++) {
              {
                if( getTile( x, y, z) == NULL) // Tile nicht vorhanden
                    continue;
                uint8_t type = getTile( x, y, z)->ID;
                if( type == 0) {
                    b_visibility = false;
                    continue;
                }
                if( y != 0 && CheckTile(x, y-1, z) && List->GetBlock( type)->getAlpha() == List->GetBlock( getTile( x, y-1, z)->ID)->getAlpha() ) {
                    b_visibility = false;
                    continue;
                }
                if( y == 0 && Down != NULL && Down->CheckTile(x, CHUNK_HEIGHT-1, z) && Down->getTile( x, CHUNK_HEIGHT-1, z)->ID) {
                    b_visibility = false;
                    continue;
                }
                if(b_visibility && x != 0 && CheckTile(x-1, y, z) && getTile( x-1, y, z)->ID == getTile( x, y, z)->ID) {
                    p_vertex[i - 5] = block_vertex(x+1    , y, z, 0);
                    p_vertex[i - 3] = block_vertex(x + 1, y, z, 0);
                    p_vertex[i - 2] = block_vertex(x + 1, y, z+1, 0);
                } else {
                    Side_Textur_Pos = List->GetTexturByType( type, 5);

                    p_vertex.resize( p_vertex.size() + 6);
                    p_data.resize( p_data.size() + 6);

                    p_data[i] = block_data( 1, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertex[i++] = block_vertex(x, y, z, 0);

                    p_data[i] = block_data( 1, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertex[i++] = block_vertex(x + 1, y, z, 0);

                    p_data[i] = block_data( 1, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertex[i++] = block_vertex(x, y, z + 1, 0);

                    p_data[i] = block_data( 1, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertex[i++] = block_vertex(x + 1, y, z, 0);

                    p_data[i] = block_data( 1, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertex[i++] = block_vertex(x + 1, y, z + 1, 0);

                    p_data[i] = block_data( 1, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertex[i++] = block_vertex(x, y, z + 1, 0);
                }
                b_visibility = true;
            }
        }
    }
    // View from positive y
    for(int z = 0; z < CHUNK_DEPTH; z++) {
         for(int y = 0; y < CHUNK_HEIGHT; y++){
             for(int x = 0; x < CHUNK_WIDTH; x++) {
                if( getTile( x, y, z) == NULL) // Tile nicht vorhanden
                    continue;
                uint8_t type = getTile( x, y, z)->ID;
                if( type == 0) {
                    b_visibility = false;
                    continue;
                }
                if(  y != CHUNK_HEIGHT-1 && CheckTile(x, y+1, z) && List->GetBlock( type)->getAlpha() == List->GetBlock( getTile( x, y+1, z)->ID)->getAlpha() ) {
                    b_visibility = false;
                    continue;
                }
                if( y == CHUNK_HEIGHT-1 && Up != NULL && Up->CheckTile(x, 0, z) && Up->getTile( x, 0, z)->ID) {
                    b_visibility = false;
                    continue;
                }
                if(b_visibility && x != 0 && CheckTile(x-1, y, z) && getTile( x-1, y, z)->ID == getTile( x, y, z)->ID) {
                    p_vertex[i - 4] = block_vertex( x+1,     y+1,     z, 0);
                    p_vertex[i - 3] = block_vertex( x+1, y+1, z, 0);
                    p_vertex[i - 1] = block_vertex( x+1, y+1, z+1, 0);
                } else {
                    Side_Textur_Pos = List->GetTexturByType( type, 4);

                    p_vertex.resize( p_vertex.size() + 6);
                    p_data.resize( p_data.size() + 6);

                    p_data[i] = block_data( 1, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertex[i++] = block_vertex(x, y + 1, z, 0);

                    p_data[i] = block_data( 1, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertex[i++] = block_vertex(x, y + 1, z + 1, 0);

                    p_data[i] = block_data( 1, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertex[i++] = block_vertex(x + 1, y + 1, z, 0);

                    p_data[i] = block_data( 1, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertex[i++] = block_vertex(x + 1, y + 1, z, 0);

                    p_data[i] = block_data( 1, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertex[i++] = block_vertex(x, y + 1, z + 1, 0);

                    p_data[i] = block_data( 1, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertex[i++] = block_vertex(x + 1, y + 1, z + 1, 0);
                }
                b_visibility = true;
            }
        }
    }
    // View from negative z
    for(int z = CHUNK_DEPTH - 1; z >= 0; z--) {
         for(int x = 0; x < CHUNK_WIDTH; x++){
            for(int y = 0; y < CHUNK_HEIGHT; y++) {
                if( getTile( x, y, z) == NULL) // Tile nicht vorhanden
                    continue;
                uint8_t type = getTile( x, y, z)->ID;
                if( type == 0) {
                    b_visibility = false;
                    continue;
                }
                if(  z != 0 && CheckTile(x, y, z-1) && List->GetBlock( type)->getAlpha() == List->GetBlock( getTile( x, y, z-1)->ID)->getAlpha() ) {
                    b_visibility = false;
                    continue;
                }
                if( z == 0 && Left != NULL && Left->CheckTile(x, y, CHUNK_DEPTH-1) && Left->getTile( x, y, CHUNK_DEPTH-1)->ID) {
                    b_visibility = false;
                    continue;
                }
                if(b_visibility && y != 0 && CheckTile(x, y-1, z) && getTile( x, y, z)->ID == getTile( x, y-1, z)->ID) {
                    p_vertex[i - 5] = block_vertex(x, y+1, z, 0);
                    p_vertex[i - 2] = block_vertex(x+1, y+1, z, 0);
                    p_vertex[i - 3] = block_vertex(x, y+1, z, 0);
                } else {
                    Side_Textur_Pos = List->GetTexturByType( type, 2);

                    p_vertex.resize( p_vertex.size() + 6);
                    p_data.resize( p_data.size() + 6);

                    p_data[i] = block_data( 0, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertex[i++] = block_vertex(x, y, z, 0);

                    p_data[i] = block_data( 0, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertex[i++] = block_vertex(x, y + 1, z, 0);

                    p_data[i] = block_data( 0, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertex[i++] = block_vertex(x + 1, y, z, 0);

                    p_data[i] = block_data( 0, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertex[i++] = block_vertex(x, y + 1, z, 0);

                    p_data[i] = block_data( 0, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertex[i++] = block_vertex(x + 1, y + 1, z, 0);

                    p_data[i] = block_data( 0, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertex[i++] = block_vertex(x + 1, y, z, 0);
                }
                b_visibility = true;
            }
        }
    }
    // View from positive z
    for(int z = 0; z < CHUNK_DEPTH; z++) {
        for(int x = 0; x < CHUNK_WIDTH; x++) {
            for(int y = 0; y < CHUNK_HEIGHT; y++) {
                if( getTile( x, y, z) == NULL) // Tile nicht vorhanden
                    continue;
                uint8_t type = getTile( x, y, z)->ID;
                if( type == 0) {
                    b_visibility = false;
                    continue;
                }
                if( z != CHUNK_DEPTH-1 && CheckTile(x, y, z+1) && List->GetBlock( type)->getAlpha() == List->GetBlock( getTile( x, y, z+1)->ID)->getAlpha() ) {
                    b_visibility = false;
                    continue;
                }
                // View from positive z
                if( z == CHUNK_DEPTH-1 && Right != NULL && Right->CheckTile(x, y, 0) && Right->getTile( x, y, 0)->ID) {
                    b_visibility = false;
                    continue;
                }
                if(b_visibility && y != 0 && CheckTile(x, y-1, z) && getTile( x, y-1, z)->ID == getTile( x, y, z)->ID) {
                    p_vertex[i - 4] = block_vertex( x,     y+1,     z+1, 0);
                    p_vertex[i - 3] = block_vertex( x, y+1, z+1, 0);
                    p_vertex[i - 1] = block_vertex( x+1, y+1, z+1, 0);
                } else {
                    Side_Textur_Pos = List->GetTexturByType( type, 3);

                    p_vertex.resize( p_vertex.size() + 6);
                    p_data.resize( p_data.size() + 6);

                    p_data[i] = block_data( 0, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertex[i++] = block_vertex(x, y, z + 1, 0);

                    p_data[i] = block_data( 0, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertex[i++] = block_vertex(x + 1, y, z + 1, 0);

                    p_data[i] = block_data( 0, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertex[i++] = block_vertex(x, y + 1, z + 1, 0);

                    p_data[i] = block_data( 0, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertex[i++] = block_vertex(x, y + 1, z + 1, 0);

                    p_data[i] = block_data( 0, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertex[i++] = block_vertex(x + 1, y, z + 1, 0);

                    p_data[i] = block_data( 0, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertex[i++] = block_vertex(x + 1, y + 1, z + 1, 0);
                }
                b_visibility = true;
            }

        }
    }
