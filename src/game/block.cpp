#include "block.h"

/**< block image */

block_image::block_image() {
    // Null setzen
    p_surface = NULL;
}

block_image::~block_image() {
    // falls was geladen wurde wieder freigeben
    if( p_surface != NULL)
        SDL_FreeSurface( p_surface);
}

void block_image::LoadImage( graphic* graphic) {
    // Lade Image
    //printf( "%s loaded\n", p_imagename.c_str());
    p_surface = graphic->loadSurface( p_imagename);
}

void block_image::SetImageName( std::string name) {
    p_imagename = name;
}

/**< block */

block::block() {
    p_imageloaded = false;
}

block::~block() {

}

void block::LoadImage( graphic* t_graphic) {
    // Lade die Seiten
    image_front.LoadImage( t_graphic);
    image_back.LoadImage( t_graphic);
    image_left.LoadImage( t_graphic);
    image_right.LoadImage( t_graphic);
    image_top.LoadImage( t_graphic);
    image_bottom.LoadImage( t_graphic);
    p_imageloaded = true;
}

/**< block list */
block_list::block_list( std::string Path) {
    this->Path = Path;
    DIR* dHandle;
    struct dirent* dirEntry;
    std::vector<std::string> ObjectList;

    List.clear();
    //List.reserve( RESERVE_BLOCKS);

    // Versuche verzeichnis zu Öffnen
    dHandle = opendir( Path.c_str());
    if ( dHandle != NULL ) {
        while ( 0 != ( dirEntry = readdir( dHandle ) ))  {
            std::string name = dirEntry->d_name;
            std::string block_path =  Path + "/"+ name + "/";
            //if( GetSuffix6502( name, "xml")) {
            if( FileExists( block_path + "definition.xml" ) ) {
                printf("%s\n", block_path.c_str());
                Loadblock( block_path, name);
            }
        }
    } else {
        // Fehler beim öffnen
        printf( "block_list: Ordner \"%s\" ist nicht vorhanden!\n", Path.c_str());
    }
    closedir( dHandle );
    printf ( "finish\n");
}

block_list::~block_list() {
    List.clear();
}

void block_list::Draw( graphic* graphic) {
    int t_x = 0;
    int t_y = 0;
    const int t_maxwidth = TILESET_WIDTH/BLOCK_SIZE;
    // asu allen blöcken eine Grafik erstellen
    for( int i = 0; i < (int)List.size(); i++) {
        block *block = &List[i];
        if( block->getLoadedImage() == false ) {
            block->LoadImage( graphic);
        }
        // Alle Seiten Zeichnen
        block_image* side;
        for( int n = 0; n < 6; n++) {
            // Seiten
            switch(n) {
                case 0: side = block->getFront(); break;
                case 1: side = block->getBack(); break;
                case 2: side = block->getLeft(); break;
                case 3: side = block->getRight(); break;
                case 4: side = block->getUp(); break;
                case 5: side = block->getDown(); break;
            }
            if( t_x >= t_maxwidth) {
                t_x = 0;
                t_y++;
            }
            graphic->draw( side->getSurface(), BLOCK_SIZE*t_x, BLOCK_SIZE*t_y, BLOCK_SIZE, BLOCK_SIZE, 0, 0, false);
            side->getPosX() = t_x;
            side->getPosY() = t_y;
            t_x++;
        }
    }
    // Bild speichern damit man es später nochmal laden kann(+ bild von texturen erhalten für andere anwendungen)
    graphic->saveImageBMP( "tileset");
}

glm::vec2 block_list::GetTexturByType( int Type, int Side) {
    block* t_block = get(Type);
    if( t_block == NULL)
       printf( "block not found\n");
    block_image* t_image;
    glm::vec2 v_possition;
    // aus dein Seiten lesen
    switch( Side) {
        case 0: t_image = t_block->getFront(); break;
        case 1: t_image = t_block->getBack(); break;
        case 2: t_image = t_block->getLeft(); break;
        case 3: t_image = t_block->getRight(); break;
        case 4: t_image = t_block->getUp(); break;
        case 5: t_image = t_block->getDown(); break;
        default:
            t_image = t_block->getFront();
            printf("block_list::GetTexturByType Hier läuft was DEFINITIV falsch <3\n");
        break; // falls mal was DEFINITIV falsch läuft mal melden <3
    }
    // Position vom bild auslesen
    v_possition.x = t_image->getPosX();
    v_possition.y = t_image->getPosY();
    return v_possition;
}

block* block_list::get( int ID) {
    for( int i = 0; i < (int)List.size(); i++)
        if( List[i].getID() == ID)
            return &List[i];
    return NULL;
}

block* block_list::getByID( std::string name) {
    for( int i = 0; i < (int)List.size(); i++) {
        block *block = &List[i];
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

void block_list::Loadblock (std::string Path, std::string Name) {
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement *pRoot;
    std::string File_Definition = Path + "definition.xml";
    int i_id;
    doc.LoadFile( File_Definition.c_str()); // Datei laden
    bool success = doc.LoadFile( File_Definition.c_str()); // Datei laden
    if( success == true ) {
        printf( "Loadblock: Fehler bei der Datei \"%s\"\n", File_Definition.c_str());
        return;
    }
    // Root verzeichnis wählen
    pRoot = doc.FirstChildElement( "block");

    tinyxml2::XMLElement *pgraphic;
    pgraphic = pRoot->FirstChildElement( "graphic");
    //i_id = atoi( pRoot->Attribute( "id" ));
    // Image dateiname herrauslesen

    bool Alpha = false;

    Alpha = atoi(pgraphic->Attribute("alpha"));

    printf( "Loadblock: %d %d %s\n", (int)List.size(), i_id, Name.c_str());
    // block erstellen
    block p_id;
    p_id.SetFile( Path + pgraphic->FirstChildElement( "front")->Attribute("image"),
                  Path + pgraphic->FirstChildElement( "back")->Attribute("image"),
                  Path + pgraphic->FirstChildElement( "left")->Attribute("image"),
                  Path + pgraphic->FirstChildElement( "right")->Attribute("image"),
                  Path + pgraphic->FirstChildElement( "top")->Attribute("image"),
                  Path + pgraphic->FirstChildElement( "bottom")->Attribute("image"));
    p_id.SetName( Name);
    p_id.SetAlpha( Alpha);
    p_id.SetID( (int)List.size()+1);
    List.push_back( p_id);
}
