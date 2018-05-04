#include "block.h"

/**< block image */

block_image::block_image() {
    // Null setzen
    p_surface = NULL;

    p_side = BLOCK_SIDE_NONE;
}

block_image::~block_image() {
    // falls was geladen wurde wieder freigeben
    if( p_surface != NULL)
        SDL_FreeSurface( p_surface);
}

void block_image::loadImage( graphic* graphic) {
    // Lade Image
    printf( "block_image::loadImage \"%s\" loaded\n", p_imagename.c_str());
    p_surface = graphic->loadSurface( p_imagename);
}

void block_image::setImageName( std::string name) {
    p_imagename = name;
}

/**< block */

block::block() {
    p_imageloaded = false;
}

block::~block() {

}

void block::loadImage( graphic* t_graphic) {
    // Lade die Seiten
    for( int i = 0; i < p_image.size(); i++)
        p_image[i].loadImage( t_graphic);

    p_imageloaded = true;
}

block_image* block::get( block_side side)
{
    for( int i = 0; i < (int)p_image.size(); i++) {
        if( p_image[i].getSide() & side)
            return &p_image[i];
    }
    return &p_image[0];
}

// block list
block_list::block_list( config *config) {
    DIR* l_handle;
    struct dirent* l_dirEntry;
    std::string l_path;
    std::string l_name;
    std::string l_block_path;
    std::vector<std::string> ObjectList;

    // set path
    l_path = config->get( "path", "block_list", "blocks");
    this->p_path = l_path;

    // open folder
    l_handle = opendir( l_path.c_str());
    if ( l_handle != NULL ) {
        while ( 0 != ( l_dirEntry = readdir( l_handle ) ))  {
            l_name = l_dirEntry->d_name;
            l_block_path =  l_path + "/"+ l_name + "/";
            if( fileExists( l_block_path + "definition.xml" ))
                loadblock( l_block_path, l_name);
        }
    } else {
        printf( "block_list Ordner \"%s\" ist nicht vorhanden!\n", l_path.c_str());
    }
    closedir( l_handle );
}

block_list::~block_list() {
    delete p_tilemap;
    p_blocks.clear();
}

void block_list::init( graphic* graphic, config *config) {
    int l_block_x = 0;
    int l_block_y = 0;
    std::string l_filename_tilemap;

    const int l_maxwidth = TILESET_WIDTH/BLOCK_SIZE;

    // get file path
    l_filename_tilemap = config->get( "tilemap", "block_list", "tileset");

    // draw every block into a image
    for( int i = 0; i < (int)p_blocks.size(); i++) {
        block *block = &p_blocks[i];
        if( block->getLoadedImage() == false ) {
            block->loadImage( graphic);
        }

        // draw only once by side
        block_image* l_side;
        for( int n = 0; n < (int)block->getSideAmount(); n++) {
            l_side = block->getSide( n);
            // check limit width block texture
            if( l_block_x >= l_maxwidth) {
                l_block_x = 0;
                l_block_y++;
            }
            graphic->draw( l_side->getSurface(), BLOCK_SIZE*l_block_x, BLOCK_SIZE*l_block_y, BLOCK_SIZE, BLOCK_SIZE, 0, 0, false);
            // save data to the block (for later use)
            l_side->setPosition( glm::vec2( l_block_x, l_block_y));
            l_block_x++;
        }
    }

    // save the image
    graphic->saveImageBMP( l_filename_tilemap.c_str());

    // now load it :)
    p_tilemap = new texture( l_filename_tilemap + ".png");
}

glm::ivec2 block_list::getTexturByType( int Type, block_side side) {
    block* l_block = get(Type);
    if( l_block == NULL) {
       printf( "block not found\n");
       return glm::vec2( 0, 0);
    }
    block_image* l_image;
    // aus dein Seiten lesen
    switch( side) {
        case BLOCK_SIDE_FRONT: l_image = l_block->get( BLOCK_SIDE_FRONT); break;
        case BLOCK_SIDE_BACK: l_image = l_block->get( BLOCK_SIDE_BACK); break;
        case BLOCK_SIDE_LEFT: l_image = l_block->get( BLOCK_SIDE_LEFT); break;
        case BLOCK_SIDE_RIGHT: l_image = l_block->get( BLOCK_SIDE_RIGHT); break;
        case BLOCK_SIDE_TOP: l_image = l_block->get( BLOCK_SIDE_TOP); break;
        case BLOCK_SIDE_BUTTOM: l_image = l_block->get( BLOCK_SIDE_BUTTOM); break;
        default:
            l_image = l_block->get( BLOCK_SIDE_NONE);
            printf("block_list::GetTexturByType Hier läuft was DEFINITIV falsch <3\n");
        break; // falls mal was DEFINITIV falsch läuft mal melden <3
    }
    return l_image->getPosition();
}

block* block_list::get( int ID) {
    for( int i = 0; i < (int)p_blocks.size(); i++)
        if( p_blocks[i].getID() == ID)
            return &p_blocks[i];
    return NULL;
}

block* block_list::getByName( std::string name) {
    for( int i = 0; i < (int)p_blocks.size(); i++) {
        block *block = &p_blocks[i];
        if( block->getName() == name)
            return block;
    }
    return NULL;
}

bool block_list::fileExists(std::string filename) {
    std::ifstream l_file;
    // file open and close
    l_file.open ( filename.c_str());
    if (l_file.is_open()) {
        l_file.close();
        return true;
    }
    l_file.close();
    return false;
}

void block_list::loadblock (std::string path, std::string name) {
    int l_id = -1;
    bool l_alpha = false;
    tinyxml2::XMLDocument l_doc;
    tinyxml2::XMLElement *l_xml_graphic;
    tinyxml2::XMLElement *l_xml_block;
    std::string l_file_definition = path + "definition.xml";
    std::vector<std::string> l_image;
    std::vector<block_side> l_image_side;

    l_doc.LoadFile( l_file_definition.c_str()); // Datei laden
    bool success = l_doc.LoadFile( l_file_definition.c_str()); // Datei laden
    if( success == true ) {
        printf( "block_list::loadblock fehler beim laden der XML datei \"%s\"\n", l_file_definition.c_str());
        return;
    }

    // get root folder
    l_xml_block = l_doc.FirstChildElement( "block");

    // get id
    if( l_xml_block->Attribute( "id", "0") == "0" )
        return;
    l_id = atoi( l_xml_block->Attribute( "id") );

    // get graphic element
    l_xml_graphic = l_xml_block->FirstChildElement( "graphic");
    if( l_xml_graphic == NULL) {
        printf( "block_list::loadblock no graphic block found in XML file \"%s\"\n", l_file_definition.c_str());
        return;
    }

    if( l_xml_graphic->FirstChildElement( "front")) {
        l_image.push_back( path + l_xml_graphic->FirstChildElement( "front")->GetText());
        l_image_side.push_back( BLOCK_SIDE_FRONT);
    }
    if( l_xml_graphic->FirstChildElement( "back")) {
        l_image.push_back( path + l_xml_graphic->FirstChildElement( "back")->GetText());
        l_image_side.push_back( BLOCK_SIDE_BACK);
    }
    if( l_xml_graphic->FirstChildElement( "left")) {
        l_image.push_back( path + l_xml_graphic->FirstChildElement( "left")->GetText());
        l_image_side.push_back( BLOCK_SIDE_LEFT);
    }
    if( l_xml_graphic->FirstChildElement( "right")) {
        l_image.push_back( path + l_xml_graphic->FirstChildElement( "right")->GetText());
        l_image_side.push_back( BLOCK_SIDE_RIGHT);
    }
    if( l_xml_graphic->FirstChildElement( "top")) {
        l_image.push_back( path + l_xml_graphic->FirstChildElement( "top")->GetText());
        l_image_side.push_back( BLOCK_SIDE_TOP);
    }
    if( l_xml_graphic->FirstChildElement( "bottom")) {
        l_image.push_back( path + l_xml_graphic->FirstChildElement( "bottom")->GetText());
        l_image_side.push_back( BLOCK_SIDE_BUTTOM);
    }

    // transparency block?
    l_alpha = atoi(l_xml_graphic->Attribute("alpha"));

    // block images sides set
    block l_block;
    for( int i = 0; i < (int)l_image.size(); i++)
        l_block.addFile( l_image[i], l_image_side[i]);
    l_block.setName( name);
    l_block.setAlpha( l_alpha);
    l_block.setID( l_id);
    p_blocks.push_back( l_block);

    printf( "block_list::loadblock #%d \"%s\" added\n", l_id, name.c_str());
}
