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

/**< block list */
block_list::block_list( std::string path) {
    this->p_path = path;
    DIR* l_handle;
    struct dirent* l_dirEntry;
    std::vector<std::string> ObjectList;

    p_blocks.clear();

    // open folder
    l_handle = opendir( path.c_str());
    if ( l_handle != NULL ) {
        while ( 0 != ( l_dirEntry = readdir( l_handle ) ))  {
            std::string l_name = l_dirEntry->d_name;
            std::string l_block_path =  path + "/"+ l_name + "/";
            if( FileExists( l_block_path + "definition.xml" ) ) {
                loadblock( l_block_path, l_name);
            }
        }
    } else {
        // Fehler beim öffnen
        printf( "block_list Ordner \"%s\" ist nicht vorhanden!\n", path.c_str());
    }
    closedir( l_handle );
}

block_list::~block_list() {
    p_blocks.clear();
}

void block_list::draw( graphic* graphic) {
    int t_x = 0;
    int t_y = 0;

    const int l_maxwidth = TILESET_WIDTH/BLOCK_SIZE;

    // asu allen blöcken eine Grafik erstellen
    for( int i = 0; i < (int)p_blocks.size(); i++) {
        block *block = &p_blocks[i];
        if( block->getLoadedImage() == false ) {
            block->loadImage( graphic);
        }

        // Alle Seiten Zeichnen
        block_image* l_side;
        for( int n = 0; n < (int)block->getSideAmount(); n++) {
            // Seiten
            l_side = block->getSide( n);

            if( t_x >= l_maxwidth) {
                t_x = 0;
                t_y++;
            }
            graphic->draw( l_side->getSurface(), BLOCK_SIZE*t_x, BLOCK_SIZE*t_y, BLOCK_SIZE, BLOCK_SIZE, 0, 0, false);
            l_side->getPosX() = t_x;
            l_side->getPosY() = t_y;
            t_x++;
        }
    }
    // Bild speichern damit man es später nochmal laden kann(+ bild von texturen erhalten für andere anwendungen)
    graphic->saveImageBMP( "tileset");
}

glm::vec2 block_list::getTexturByType( int Type, block_side side) {
    block* l_block = get(Type);
    if( l_block == NULL)
       printf( "block not found\n");
    block_image* t_image;
    glm::vec2 v_possition;
    // aus dein Seiten lesen
    switch( side) {
        case BLOCK_SIDE_FRONT: t_image = l_block->get( BLOCK_SIDE_FRONT); break;
        case BLOCK_SIDE_BACK: t_image = l_block->get( BLOCK_SIDE_BACK); break;
        case BLOCK_SIDE_LEFT: t_image = l_block->get( BLOCK_SIDE_LEFT); break;
        case BLOCK_SIDE_RIGHT: t_image = l_block->get( BLOCK_SIDE_RIGHT); break;
        case BLOCK_SIDE_TOP: t_image = l_block->get( BLOCK_SIDE_TOP); break;
        case BLOCK_SIDE_BUTTOM: t_image = l_block->get( BLOCK_SIDE_BUTTOM); break;
        default:
            t_image = l_block->get( BLOCK_SIDE_NONE);
            printf("block_list::GetTexturByType Hier läuft was DEFINITIV falsch <3\n");
        break; // falls mal was DEFINITIV falsch läuft mal melden <3
    }
    // Position vom bild auslesen
    v_possition.x = t_image->getPosX();
    v_possition.y = t_image->getPosY();
    return v_possition;
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

bool block_list::GetSuffix(const std::string &file, const std::string &suffix) {
    return file.size() >= suffix.size() && file.compare(file.size() - suffix.size(), suffix.size(), suffix) == 0;
}

bool block_list::GetSuffix6502(const std::string& str, const std::string& end) {
    size_t slen = str.size(), elen = end.size();
    if (slen <= elen) return false;
    while (elen) {
        if (str[--slen] != end[--elen]) return false;
    }
    return true;
}

bool block_list::FileExists(std::string StrFilename) {
    std::ifstream file;
    // file open
    file.open ( StrFilename.c_str());
    if (file.is_open()) {
        file.close();
        return true;
    }
    // file close
    file.close();
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
