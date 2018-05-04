#ifndef BLOCK_H
#define BLOCK_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <dirent.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <time.h>
#include <math.h>

#include "../graphic/graphic.h"
#include "../xml/tinyxml2.h"

#define BLOCK_SIZE 16

enum block_side {
    BLOCK_SIDE_NONE = 0,
    BLOCK_SIDE_LEFT = 1,
    BLOCK_SIDE_RIGHT = 2,
    BLOCK_SIDE_TOP = 4,
    BLOCK_SIDE_BUTTOM = 8,
    BLOCK_SIDE_FRONT = 16,
    BLOCK_SIDE_BACK = 32
};

inline block_side operator|(block_side a, block_side b)
{
    return static_cast<block_side>(static_cast<int>(a) | static_cast<int>(b));
}

enum block_type {
    BLOCK_TYPE_NONE,
    BLOCK_TYPE_PLANT,
    BLOCK_TYPE_LIQUID
};

class block_image  {
    public:
        block_image();
        ~block_image();
        void loadImage( graphic* graphic);
        void setImage( SDL_Surface* p_surface);
        void setImageName( std::string name);
        glm::ivec2 getPosition() { return p_position; }
        void setPosition(glm::ivec2 position) { p_position = position; }
        SDL_Surface *getSurface() { return p_surface; }
        std::string getImageName() { return p_imagename; }
        block_side getSide() { return p_side; }
        void setSide( block_side side) { p_side = side; }
        void addSide( block_side side) { p_side = p_side | side; }
    private:
        SDL_Surface* p_surface;
        glm::ivec2 p_position;
        std::string p_imagename;
        block_side p_side;
};

class block {
    public:
        block();
        ~block();

        void loadImage( graphic* t_graphic);
        void setID( int id) { p_id = id; }
        void setName( std::string name) { n_name = name; }
        void setAlpha( bool flag) { p_alpha = flag; }

        void addFile( std::string image, block_side side) {
            for( int i = 0; i < (int)p_image.size(); i++) {
                block_image *l_image = &p_image[i];
                if( l_image->getImageName() == image ) {
                    l_image->addSide( side);
                    return;
                }
            }

            block_image l_image;
            l_image.setImageName( image);
            l_image.setSide( side);
            p_image.push_back( l_image);
        }

        block_image* get( block_side side);

        bool getLoadedImage() { return p_imageloaded; }
        int getID() { return p_id; }
        std::string getName() { return n_name; }
        bool getAlpha() { return p_alpha; }
        int getSideAmount() { return p_image.size(); }
        block_image* getSide( int i) { return &p_image[i]; }
    protected:
    private:
        // block
        int p_id;
        std::string n_name;
        bool p_alpha;
        std::vector<block_image> p_image;
        bool p_imageloaded;
};

class block_list {
    public:
        block_list( config *config);
        ~block_list();

        void init( graphic* graphic, config *config);

        glm::ivec2 getTexturByType( int Type, block_side side);

        block* get( int ID);
        block* getByName( std::string name);

        int getAmountblocks() { return (int)p_blocks.size(); }
        texture *getTilemapTexture() { return p_tilemap; }
    private:
        texture *p_tilemap;
        std::string p_path;
        std::vector<block> p_blocks;
    protected:
        bool fileExists(std::string filename);
        void loadblock (std::string path, std::string name);
};
#endif // BLOCK_H
